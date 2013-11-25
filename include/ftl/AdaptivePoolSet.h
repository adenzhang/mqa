#ifndef _FLT_ADAPTIVEPOOLSET_H_
#define _FLT_ADAPTIVEPOOLSET_H_
#include <list>

#include "FixedSizePool.h"

namespace ftl {
// AdaptivePoolSet manages multiple of FixedSizePool or FifoPool.
// it may allocate new pool when all of pools are full.
// And it may delete a pool when it is empty.
// User can specify a strategy about when to allocate and delete and the number of pools.
//
//
template <typename ObjectPoolClass>
struct AdaptivePoolSet
{
    // the number of pools can increase or decrease when ST_INC or ST_DEC is specified, or both.
    // ST_SPEED_CONSTANT indicates that each time allocating the same size pool with factor K.
    // ST_SPEED_LINEAR   is to allocate a pool with speed factor K*m, where m is the number pools.
    // ST_SPEED_POWER    to allocate a pool with speed factor K*Kprev, where Kprev is the previous factor K.
    enum STRATEGY_TYPE {ST_INC=0x01, ST_DEC=0x02,
                        ST_SPEED_CONSTANT=0x10, ST_SPEED_LINEAR=0x20,
                        ST_DEFAULT=ST_INC|ST_DEC|ST_SPEED_CONSTANT};
    enum STRATEGY_MASK { ST_DIRECTION_MASK=0x0F, ST_SPEED_MASK=0xF0};
    enum {ALLOC_FACTOR_MAX=10, ALLOC_MIN=16};
};

struct FixedSizePoolSet
    : public AdaptivePoolSet<FixedSizePool>
{
    typedef FixedSizePoolSet               THIS_TYPE;
    typedef FixedSizePool                  POOL_TYPE;
    typedef FixedSizePool                 *POOL_PTR;
    typedef AdaptivePoolSet<POOL_TYPE>     SUPER_TYPE;

    typedef std::list<POOL_PTR>            POOL_LIST;
    typedef POOL_LIST::iterator            POOL_LIST_ITER;
    typedef POOL_LIST::const_iterator      POOL_LIST_CONSTITER;

    POOL_LIST                              _pools, _fullPools;
    const size_t                           _elemSize, _elemCount;  // per pool
    const SUPER_TYPE::STRATEGY_TYPE        _strategy;
    const float                            _K;            // speed factor
    unsigned int                           _nPrevAlloc, _countPrevAlloc;
    unsigned int                           _nPrevPeak;
    size_t                                 _nCapacity;
    
    FixedSizePoolSet(size_t elemSize, size_t elemCount, float speedFactor=0.5, SUPER_TYPE::STRATEGY_TYPE strategy=SUPER_TYPE::ST_DEFAULT)
        : _elemSize(elemSize), _elemCount(elemCount), _strategy(strategy), _K(speedFactor)
        , _nPrevPeak(elemCount), _nPrevAlloc(elemCount), _countPrevAlloc(1)
        , _nCapacity(elemCount)
    {
        _pools.push_back(_newPool(elemCount));
    }

    ~FixedSizePoolSet(){
        for(POOL_LIST_ITER it=_pools.begin(); it!=_pools.end(); ++it) {
            if(*it) delete *it;
        }
        for(POOL_LIST_ITER it=_fullPools.begin(); it!=_fullPools.end(); ++it) {
            if(*it) delete *it;
        }
    }
    POOL_PTR _newPool(size_t elemCount) {
        return POOL_PTR(new POOL_TYPE(_elemSize, elemCount));
    }

    size_t size() const{
        size_t n = 0;
        for(POOL_LIST_CONSTITER it=_pools.begin(); it!=_pools.end(); ++it) {
            n += (*it)->size();
        }
        for(POOL_LIST_CONSTITER it=_fullPools.begin(); it!=_fullPools.end(); ++it) {
            n += (*it)->size();
        }
        return n;
    }
    size_t capacity() const {
        return _nCapacity;
    }
    size_t pools_count() const {
        return _pools.size() + _fullPools.size();
    }
    bool empty() const {
        if(!_fullPools.empty()) return false;
        for(POOL_LIST_CONSTITER it=_pools.begin(); it!=_pools.end(); ++it) {
            if( ! (*it)->empty() ) return false;
        }
        return true;
    }
    POOL_LIST* _findPool2delalloc(POOL_LIST_ITER& itPool, void *ptr) {
        for(POOL_LIST_ITER it=_pools.begin(); it!=_pools.end(); ++it) {
            if( (*it)->_buf < (char*)ptr &&  (*it)->_pEnd > (char*)ptr) {
                itPool = it;
                return &_pools;
            }
        }
        for(POOL_LIST_ITER it=_fullPools.begin(); it!=_fullPools.end(); ++it) {
            if( (*it)->_buf < (char*)ptr &&  (*it)->_pEnd > (char*)ptr) {
                itPool = _swap(_fullPools, it, _pools);
                return &_pools;
            }
        }
        return NULL;
    }
    POOL_LIST_ITER _swap(POOL_LIST& fromPool, POOL_LIST_ITER& it, POOL_LIST& toPool) {
        POOL_LIST_ITER t = toPool.insert(toPool.end(),(*it));
        fromPool.erase(it);
        return t;
    }
    bool deallocate(void *buf){
        POOL_LIST_ITER   itPool;
        POOL_LIST       *pPools=NULL;
        if( !(pPools=_findPool2delalloc(itPool, buf)) )
            return false;

        (*itPool)->deallocate(buf);
        if( (*itPool)->empty() ) {
            if(_strategy & SUPER_TYPE::ST_DEC) {
                _nPrevPeak = _nCapacity;
                _nPrevAlloc = -(*itPool)->capacity();
                _nCapacity -= (*itPool)->capacity();
                pPools->erase(itPool);
                _countPrevAlloc = 0;
            }
        }
        return true;
    }

    // return absolute value.
    POOL_PTR _newPool() {
        unsigned int prev=_nPrevAlloc>0?_nPrevAlloc:-_nPrevAlloc;
        unsigned int nAlloc = prev;
        if( _nPrevAlloc>0 )
        {
            switch(_strategy&SUPER_TYPE::ST_SPEED_MASK) {
            case SUPER_TYPE::ST_SPEED_CONSTANT:
                nAlloc = _elemCount*_K;
                break;
            case SUPER_TYPE::ST_SPEED_LINEAR:
                nAlloc = _countPrevAlloc * (1-_K);
                break;
            }
        }else{  // previously deleted pool
            nAlloc = _nPrevPeak;
        }
        if( nAlloc < SUPER_TYPE::ALLOC_MIN ) nAlloc = SUPER_TYPE::ALLOC_MIN;
        // update status
        _nPrevAlloc = nAlloc;
        _nCapacity += nAlloc;
        _countPrevAlloc++;
        return POOL_PTR(new POOL_TYPE(_elemSize, nAlloc));
    }
    void  *allocate() {
        void *ret = NULL;
        POOL_LIST& pools( _pools );
        if( pools.empty() ) {
            // now pools is _emptyPools, all the pools are full
            if( _strategy & SUPER_TYPE::ST_INC ) {  // handle full pools
                // allocate new pools
                POOL_PTR pPool = _newPool();
                _pools.push_back(pPool);
                ret = pPool->allocate();
            }
        }else{
            POOL_LIST_ITER itPool = pools.begin();
            ret = (*itPool)->allocate();
            assert(ret);
            if( (*itPool)->full() ) {
                _swap(pools, itPool, _fullPools);
            }
        }
        return ret;
    }

};
} // namespace ftl
#endif // _FLT_ADAPTIVEPOOLSET_H_
