#pragma  once

/// for each allocated memory pointed by void* ptr, the size of memory indicated by (int *)(ptr-1)
/// case 1
///   ||..empty..|....allocated...|.....empty....||
//     |_buf     |_tail           |_head,_end    |_MAX_SIZE
/// case 2 (full, _tail==_end==max_size and _head==_buf)
///   ||...allocated.........allocated....||
//     |_buf,_tail                        |_head,_end,_MAX_SIZE
/// case 3
//    ||...allocated...|.....empty....|....allocated....|....empty.....||
///    |_buf           |_head         |_tail            |_end          |_MAX_SIZE
/// case 4 (full, _head=_tail-1)
//    ||...allocated...|..one-byte-empty..|....allocated....|....empty.....||
///    |_buf           |_head             |_tail            |_end          |_MAX_SIZE
namespace ftl{

class FifoPool
{
public:
    typedef unsigned int HEADER_T;
    enum {HEADER_SIZE=sizeof(unsigned int), FULL_GAP = 1};
protected:
    char                *_buf;
    const size_t        MAX_SIZE;
    size_t               nBuffers;

    char                *_head, *_tail;     //   |... 
    char                *_end;   // point to end of buf


    inline char *advance(char *ptr, size_t size) {
        *(int*)ptr = size;
        ptr += HEADER_SIZE + size;
        return ptr;
    }
    inline char *next(char *ptr, size_t size) {
        ptr += HEADER_SIZE + size;
        return ptr;
    }
public:

    FifoPool(size_t maxsize): MAX_SIZE(maxsize+HEADER_SIZE+FULL_GAP), nBuffers(0)
    {
        assert(MAX_SIZE > 1);
        _buf = new char[MAX_SIZE];
        _head = _buf;
        _tail = _buf;
        _end =  _buf;
    }
    ~FifoPool()
    {
        delete[] _buf;
    }

    inline bool empty() { return _head == _tail;}
    size_t size(){return nBuffers;}
    size_t capacity() { return MAX_SIZE; }

    void  *allocate(size_t size) {
        if( size == 0 ) return NULL;
        void *ret = NULL;
        if(_head >= _tail) {
            if( MAX_SIZE-(_head-_buf) >= size+HEADER_SIZE ) {// check back space
                ret = _head + HEADER_SIZE;
                _end = _head = advance(_head, size);
                nBuffers++;
            }else if( (size_t)(_tail - _buf)>= size+HEADER_SIZE ){ // not enough apace at back, check front space
                ret = _buf + HEADER_SIZE;
                _end = _head;                      // set at end of last back one
                _head = advance(_buf, size);
                nBuffers++;
            }else{ // not enough space
                return NULL;
            }
        }else {
            if( (size_t)(_tail-_head)>= size + FULL_GAP +HEADER_SIZE ) { // head catches up from front
                ret = _head + HEADER_SIZE;
                _head = advance(_head, size);
                nBuffers++;
            }else{ // not enough space
                return NULL;
            }
        }
        return ret;
    }
    void *peek(size_t *size) {
        if( empty() ) return NULL;
        if(size)
            *size = *(int*)_tail;
        return _tail + HEADER_SIZE;
    }

    // deallocate
    // return popped up address
    void *deallocate() {
        return pop();
    }
    // deallocate
    // return popped up address
    void *pop() {
        if( empty() ) return NULL;
        char * ret = NULL;

        size_t n = *(int*)_tail;
        size_t *size=NULL;
        if(size)
            *size = n;
        ret = _tail + HEADER_SIZE;
        _tail = next(_tail, n);
        if( _tail == _head ){  // empty
            _head = _tail = _end = _buf;
        }else if( _tail == _end ) {  // remove last back one
            _end = _head;
            _tail = _buf;
        }
        nBuffers--;
        return ret;
    }

public:
    struct BufPointer {
        int        len;
        void      *ptr;
    };
    //typedef std::list<BufPointer> BufPointerList;

    bool GetFirstBuf(BufPointer& bp) {
        if( empty() ) return false;
        bp.ptr = _tail + HEADER_SIZE;
        bp.len = *(int*)_tail;
        return true;
    }
    bool GetNextBuf(BufPointer& bp) {
        void *ptr = advance((char*)bp.ptr - HEADER_SIZE, bp.len);
        if( ptr == _head )
            return false;
        if( ptr == _end ) {  // at back end
            bp.ptr = _buf;
            bp.len = *(int*)_buf;
        }else{
            bp.ptr = (char*)ptr + HEADER_SIZE;
            bp.len = *(int*)ptr;
        }
        return true;
    }
};
}  // namespace ftl