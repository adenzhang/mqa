#pragma  once

// size of each allocated buffer is fixed to multiple elements
// buffer layout:   |1 int header|.....multiple elements buffer ...|
//                      | number of elements
namespace ftl {
class FixedSizePool
{
public:
    typedef unsigned int HEADER_TYPE;
    enum {HEADER_SIZE=sizeof(unsigned int)
        , BUFFER_ALLOCATED_MASK=1<<(HEADER_SIZE*8-1)
        , BUFFER_MAX_ELEM = ~BUFFER_ALLOCATED_MASK};

    struct HEADER_T{
        HEADER_TYPE bAlloc:1;
        HEADER_TYPE nElem:(HEADER_SIZE*8-1);
    };

protected:
    char                *_buf, *_pEnd, *_pAvail;
    const size_t         _elemSize, _elemMaxCount, _oneBufSize;
    const size_t         MAX_SIZE;
    size_t               nBuffers;


    inline HEADER_T& getHeader(char *buf) {
        return *(HEADER_T*)(buf-HEADER_SIZE);
    }
    //inline size_t getElemCount(HEADER_T& h) {
    //    return h & BUFFER_MAX_ELEM;
    //}
    inline bool isBufferAllocated(HEADER_T& h) {
        return h.bAlloc;
    }
    inline void setAlloc(HEADER_T& h, bool bAlloc) {
        h.bAlloc = bAlloc;
    }
    //inline void setBuffer(HEADER_T& h, bool bAlloc, size_t elmCount) {
    //    //h = bAlloc ? BUFFER_ALLOCATED_MASK : 0;
    //    setAlloc(h, bAlloc);
    //    h |= BUFFER_MAX_ELEM & elmCount;
    //}
    //inline size_t getBufferSize(void *buf) { // the whole size including all unused headers, must be multiple of oneBufSize
    //    return _oneBufSize;// getElemCount(buf)*_oneBufSize;
    //}
    //inline bool merge(void *buf) {
    //    // merge adjacent unallocated buffers
    //    HEADER_T& h = getHeader(buf);
    //    if(isBufferAllocated(h)) return false;
    //    void *nextBuf =NULL;
    //    size_t  n = 0;
    //    while(nextBuf = nextBuffer(buf)) {
    //        HEADER_T& h1 = getHeader(nextBuf);
    //        if(isBufferAllocated(h1)) break;
    //        n += getElemCount(h1);
    //    }
    //    if( n == 0 ) return false;
    //    setBuffer(h, true, getElemCount(h) + n);
    //    return true;
    //}
    inline void *firstBuffer(){
        return ((char*)_buf) + HEADER_SIZE;
    }

    inline void * nextBuffer(void *buf=NULL) {
        return ((char*)buf) + _oneBufSize; //getBufferSize(buf);
    }
public:
    FixedSizePool(size_t elemSize, size_t elemMaxCount)
        :_elemSize(elemSize), _elemMaxCount(elemMaxCount)
        , nBuffers(0), MAX_SIZE((elemSize+HEADER_SIZE) * elemMaxCount)
        , _oneBufSize(_elemSize + HEADER_SIZE)
    {
        _buf = new char[MAX_SIZE];
        _pEnd = _buf + MAX_SIZE;
        char *ptr=(char*)firstBuffer();
        do{
            setAlloc(getHeader(ptr), false);
        }while( (ptr = (char*)nextBuffer(ptr)) < _pEnd );
        _pAvail = (char*)firstBuffer();
    }
    ~FixedSizePool()
    {
        delete[] _buf;
    }
    size_t size(){return nBuffers;}
    size_t capacity() {return _elemMaxCount;}

    //inline size_t getElemCount(void *buf) {
    //    return getElemCount(getHeader(buf));
    //}
    inline void deallocate(void *buf){
        assert(buf > _buf && buf < _pEnd);
        setAlloc(getHeader((char*)buf), false);
        _pAvail = (char*) buf;
        nBuffers--;
        //merge(buf);
    }
    inline bool full() {
        return nBuffers == _elemMaxCount;
    }
    bool empty() { return nBuffers == 0; }
    void  *allocate() {
        char *ret;
        if(_pAvail) {
            ret = _pAvail;
            HEADER_T& h = getHeader(ret);
            setAlloc(h, true);
            nBuffers++;

            if( full() )
                _pAvail = NULL;
            // look for next available block
            char *buf = ret;
            while( (buf=(char*)nextBuffer(buf)) < _pEnd ) {
                if( ! isBufferAllocated(getHeader(buf)) ) {
                    _pAvail = buf;
                    return ret;
                }
            }
            buf = (char*)firstBuffer();
            while( (buf=(char*)nextBuffer(buf)) < ret ) {
                if( ! isBufferAllocated(getHeader(buf)) ) {
                    _pAvail = buf;
                    return ret;
                }
            }
            
            return ret;
        }else{  // full
            assert(full());
            return NULL;
        }
    }

};
}  // namespace ftl