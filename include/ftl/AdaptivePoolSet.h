#ifndef _FLT_ADAPTIVEPOOLSET_H_
#define _FLT_ADAPTIVEPOOLSET_H_
#include <list>

#include "FixedSizePool.h"

namespace ftl {
// AdaptivePoolSet manages multiple of FixedSizePool or FifoPool.
// it may allocate new pools when all of them are full.
// And it may delete some pools when there are too many Pools.
// User can specify a strategy about when to allocate and delete and the number of pools.
template <typename ObjectPoolClass>
struct AdaptivePoolSet
{
    // the number of pools can increase or decrease when ST_INC or ST_DEC is specified, or both.
    // ST_SPEED_CONSTANT indicates that each time allocating the same number of pools.
    // ST_SPEED_LINEAR   is to allocate k-more than the previous allocated pools.
    // ST_SPEED_POWER      means allocating times of the previous allocated pools.
    enum STRATEGY_TYPE {ST_INC=0x01, ST_DEC=0x02, //ST_UPPER_LIMIT=0x04, ST_LOWER_LIMIT=0x08,
                        ST_SPEED_CONSTANT=0x10, ST_SPEED_LINEAR=0x20,  ST_SPEED_POWER=0x30,
                        ST_DEFAULT=ST_INC|ST_DEC|ST_SPEED_CONSTANT};
    enum STRATEGY_MASK { ST_DIRECTION_MASK=0x0F, ST_SPEED_MASK=0xF0};
};

template <>
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

    POOL_LIST                              _pools, _emptyPools, _fullPools;
    const size_t                           _elemSize, _elemCount;  // per pool
    const SUPER_TYPE::STRATEGY_TYPE        _strategy;
    const unsigned int                     _speedFactor;
    int                                    _nPrevAlloc;            // number previous allocated pools, can be minus if deallocated.
    
    FixedSizePoolSet(size_t elemSize, size_t elemCount, SUPER_TYPE::STRATEGY_TYPE strategy=SUPER_TYPE::ST_DEFAULT, unsigned int speedFactor=1)
        : _elemSize(elemSize), _elemCount(elemCount), _strategy(strategy), _speedFactor(speedFactor)
    {
        _emptyPools.push_back(_newPool());
        _nPrevAlloc = 1;
    }

    ~FixedSizePoolSet(){
        for(POOL_LIST_ITER it=_pools.begin(); it!=_pools.end(); ++it) {
            if(*it) delete *it;
        }
        for(POOL_LIST_ITER it=_emptyPools.begin(); it!=_emptyPools.end(); ++it) {
            if(*it) delete *it;
        }
        for(POOL_LIST_ITER it=_fullPools.begin(); it!=_fullPools.end(); ++it) {
            if(*it) delete *it;
        }
    }
    POOL_PTR _newPool() {
        POOL_PTR(new POOL_TYPE(_elemSize, _elemCount);
    }

    size_t size() const{
        size_t n = 0;
        for(POOL_LIST_CONSTITER it=_pools.begin(); it!=_pools.end(); ++it) {
            n += (*it)->size();
        }
        return n + _fullPools.size() * _elemCount;
    }
    size_t capacity() const {
        return _pools.size() * _elemCount;
    }
    bool empty() const {
        for(POOL_LIST_CONSTITER it=_pools.begin(); it!=_pools.end(); ++it) {
            if( ! (*it)->empty() ) return false;
        }
        return true;
    }
    POOL_LIST* findPool(POOL_LIST_ITER& itPool, void *ptr) {
        for(POOL_LIST_ITER it=_pools.begin(); it!=_pools.end(); ++it) {
            if( (*it)->_buf < (char*)ptr &&  (*it)->_pEnd > (char*)ptr) {
                itPool = it;
                return &_pools;
            }
        }
        for(POOL_LIST_ITER it=_fullPools.begin(); it!=_fullPools.end(); ++it) {
            if( (*it)->_buf < (char*)ptr &&  (*it)->_pEnd > (char*)ptr) {
                itPool = it;
                return &_fullPools;
            }
        }
        return NULL;
    }
    void swap(POOL_LIST& fromPool, POOL_LIST_ITER& it, POOL_LIST& toPool) {
        toPool.push_back(*it);
        fromPool.erase(it);
    }
    bool deallocate(void *buf){
        POOL_LIST_ITER   itPool;
        POOL_LIST       *pPools;
        if( !(pPools=findPool(itPool, buf)) )
            return false;

        (*itPool)->deallocate(buf);
        if( (*itPool)->empty() ) {
            swap(*pPools, itPool, _emptyPools);
            if((_strategy & SUPER_TYPE::ST_DEC) {
            }
            // calculate the number of pools to be deleted.
            int nDealloc = calcCurrentSpeed(0);
        }
    }

    inline int isDiffDirection(int bInc) const {
        return (int(_nPrevAlloc>0)) ^ bInc & 0x01;
    }
    // return absolute value.
    int calcCurrentSpeed(int bInc) const  {
        int prevSpeed = _nPrevAlloc>0?_nPrevAlloc:-_nPrevAlloc;
        int n = _speedFactor;
        if( !isDiffDirection(bInc) ) {
            switch(_strategy&SUPER_TYPE::ST_SPEED_MASK) {
            case SUPER_TYPE::ST_SPEED_CONSTANT:
                n = _speedFactor;
                break;
            case SUPER_TYPE::ST_SPEED_LINEAR:
                n = prevSpeed + _speedFactor;
                break;
            case SUPER_TYPE::ST_SPEED_POWER:
                n = prevSpeed * _speedFactor;
                break;
            }
        }
        return n; 
    }
    void  *allocate() {
        void *ret = NULL;
        POOL_LIST& pools( _pools.empty()? _emptyPools: _pools );
        if( pools.empty() ) {
            // now pools is _emptyPools, all the pools are full
            if( _strategy & SUPER_TYPE::ST_INC ) {  // handle full pools
                int nAlloc = calcCurrentSpeed(1);
                // allocate new pools
                // calculate the number of pools to be allocated with strategy
                for(int i=0; i<nAlloc; ++i) {
                    _emptyPools.push_back(_newPool());
                }
                _nPrevAlloc = nAlloc;

                POOL_LIST_ITER itPool = _emptyPools.begin();
                ret = (*itPool)->allocate();
                swap(_emptyPools, itPool, _pools);
            }
        }else{
            POOL_LIST_ITER itPool = pools.begin();
            ret = (*itPool)->allocate();
            assert(ret);
            if( (*itPool)->full() ) {
                swap(pools, itPool, _fullPools);
            }
        }
        return ret;
    }

};
} // namespace ftl
#endif // _FLT_ADAPTIVEPOOLSET_H_