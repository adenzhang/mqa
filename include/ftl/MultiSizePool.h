#pragma  once

// size of each allocated buffer is fixed to multiple elements
// buffer layout:   |1 int header|.....multiple elements buffer ...|
//                      | number of elements
namespace ftl {
class MultiSizePool
{
public:
    typedef unsigned int HEADER_TYPE;
    enum {HEADER_SIZE=sizeof(HEADER_TYPE)
        , BUFFER_ALLOCATED_MASK=1<<(HEADER_SIZE*8-1)
        , BUFFER_MAX_ELEM = ~BUFFER_ALLOCATED_MASK};

    struct HEADER_T{
        HEADER_TYPE bAlloc:1;
        HEADER_TYPE nElem:(HEADER_SIZE*8-1);
    };

protected:
    const char                *_buf, *_pEnd;
    const size_t         _elemSize, _elemMaxCount, _oneBufSize;
    const size_t         MAX_SIZE;
    size_t               nBuffers;

    inline HEADER_T& getHeader(char *buf) {
        return *(HEADER_T*)(buf-HEADER_SIZE);
    }
    inline size_t getElemCount(HEADER_T& h) {
        return h.nElem;
    }
    inline bool isBufferAllocated(HEADER_T& h) {
        return h.bAlloc;
    }
    inline void setAlloc(HEADER_T& h, bool bAlloc) {
        h.bAlloc = bAlloc;
        //if(bAlloc)
        //    h |= BUFFER_ALLOCATED_MASK;
        //else
        //    h &= ~BUFFER_ALLOCATED_MASK;
    }
    inline void setBuffer(HEADER_T& h, bool bAlloc, size_t elmCount) {
        //h = bAlloc ? BUFFER_ALLOCATED_MASK : 0;
        setAlloc(h, bAlloc);
        h.nElem = elmCount;
        //h &= ~BUFFER_MAX_ELEM;
        //h |= BUFFER_MAX_ELEM & elmCount;
    }
    inline size_t getBufferSize(void *buf) { // the whole size including all unused headers, must be multiple of oneBufSize
        return getElemCount(buf)*_oneBufSize;
    }
    // merge adjacent unallocated buffers
    // return count of elements merged.
    inline size_t merge_free(void *& buf) {
        size_t  n = 0;
        void   *nextBuf = buf;
        do{
            HEADER_T& h1 = getHeader((char*)nextBuf);
            if(isBufferAllocated(h1)) break;
            n += getElemCount(h1);
        }while( (nextBuf=nextBuffer(nextBuf)) < _pEnd );

        if(n == 0) return 0;

        nextBuf = buf;
        while( (buf=prevBuffer(buf)) > _buf ) {
            HEADER_T& h1 = getHeader((char*)buf);
            if(isBufferAllocated(h1)) break;
            n += getElemCount(h1);
            nextBuf = buf;
        }

        HEADER_T& h = getHeader((char*)nextBuf);
        setBuffer(h, true, n);
        return n;
    }
    inline void *firstBuffer(){
        return ((char*)_buf) + HEADER_SIZE;
    }

    inline void * nextBuffer(void *buf=NULL) {
        return ((char*)buf) + getBufferSize(buf);
    }
    inline void * prevBuffer(void *buf=NULL) {
        return ((char*)buf) - getBufferSize(buf);
    }
public:
    MultiSizePool(size_t elemSize, size_t elemMaxCount)
        :_elemSize(elemSize), _elemMaxCount(elemMaxCount)
        , nBuffers(0), MAX_SIZE((elemSize+HEADER_SIZE) * elemMaxCount)
        , _oneBufSize(_elemSize + HEADER_SIZE)
        //, _buf(new char[MAX_SIZE])
        //, _pEnd(_buf + MAX_SIZE)
    {
        _buf = new char[MAX_SIZE];
        _pEnd = _buf + MAX_SIZE;
        setBuffer(getHeader((char*)firstBuffer()), false, elemMaxCount);
    }
    ~MultiSizePool()
    {
        delete[] _buf;
    }
    size_t size(){return nBuffers;}
    size_t capacity() {return _elemMaxCount;}
    bool empty() { return nBuffers == 0; }


    inline size_t getElemCount(void *buf) {
        return getElemCount(getHeader((char*)buf));
    }
    inline void deallocate(void *buf){
        assert(buf > _buf && buf < _pEnd);
        HEADER_T& h = getHeader((char*)buf);
        setAlloc(h, false);
        nBuffers -= merge_free(buf);
    }
    inline bool full() {
        return nBuffers == _elemMaxCount;
    }
    void  *allocate(size_t elemCount=1, int bCompact=0) {
        enum ALLOC_POLICY{POLICY_COMPACT, POLICY_FAST};
        ALLOC_POLICY PL = POLICY_COMPACT;
        char *ret=NULL;
        if( full() ) return NULL;
        if( PL == POLICY_FAST) {  //-- POLICY_FAST
            char *buf = (char *)firstBuffer();
            do{
                HEADER_T& h = getHeader(buf);
                size_t mCount;
                char *mBuf;
                if( !isBufferAllocated(h) && (mCount=getElemCount(h)) >= elemCount ) {
                    setBuffer(h, true, elemCount);
                    if(mCount > elemCount) { // update next free block
                        mBuf = (char*)nextBuffer(buf);
                        setBuffer(getHeader(mBuf), false, mCount-elemCount);
                    }
                    nBuffers += elemCount;
                    return buf;
                }
            }while( (buf=(char*)nextBuffer(buf)) < _pEnd );
        }else{   //-- POLICY_COMPACT
            char *buf = (char *)firstBuffer();
            ret = NULL;
            size_t mCount=_elemMaxCount+1; // look for min elemcount;
            do{
                HEADER_T& h = getHeader(buf);
                size_t m;
                if( !isBufferAllocated(h) && (m=getElemCount(h)) >= elemCount ) {
                    if( mCount > m ) {
                        mCount = m;
                        ret = buf;
                    }
                }
            }while( (buf=(char*)nextBuffer(buf)) < _pEnd );
            if(ret) {
                setBuffer(getHeader(ret), true, elemCount);
                nBuffers += elemCount;
                if(mCount > elemCount) { // update next free block
                    buf = (char*)nextBuffer(ret);
                    setBuffer(getHeader(buf), false, mCount-elemCount);
                }
                return ret;
            }
        }
        return NULL;
    }

};
}  // namespace ftl