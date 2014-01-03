#pragma  once
#include <new>
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

public:
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
        if(!_buf) throw std::bad_alloc();
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
    // number of elements allocated
    size_t size(){return nBuffers;}
    // max number of elements can be allocated
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
struct DefaultMallocator
{
    static void *malloc(size_t size) {
        return ::malloc(size);
    }
    static void free(void *p) {
        ::free(p);
    }
};
#ifndef FIELD_OFFSET
#define FIELD_OFFSET(type, field)    ((usigned int)(unsigned int *)&(((type *)0)->field))
#endif

template < size_t ElemSizeT, typename Mallocator = DefaultMallocator >
class FixedSizeMemory
{
public:
    typedef char      byte_t;
    typedef void     *pointer_type;
    typedef byte_t   *byte_pointer;

    struct Node {
        Node *next;
        Node *prev;

        Node()
            : next(NULL), prev(NULL) {}

        Node *insert_before(Node* beforeNode)
        {
            Node *pNode = this;
            pNode->next = beforeNode;
            pNode->prev = beforeNode->prev;
            if(beforeNode->prev) {
                beforeNode->prev->next = pNode;
            }
            beforeNode->prev = pNode;
            return pNode;
        }
        Node *append(Node* afterNode)
        {
            Node *pNode = this;
            pNode->prev = afterNode;
            pNode->next = afterNode->next;
            if(afterNode->next) {
                afterNode->next->prev = pNode;
            }
            afterNode->next = pNode;
            return pNode;
        }
        // detach from list
        void detach() 
        {
            Node *pNode = this;
            if(pNode->prev) {
                pNode->prev->next = pNode->next;
            }
            if(pNode->next) {
                pNode->next->prev = pNode->prev;
            }
        }
    };
    const size_t      ElemCount;
    const size_t      ElemSize;
protected:
    struct BufNode
        : public Node 
    {
        byte_t        buf[ElemSizeT];
    };

    Node               _alloc;    // allocated list
    Node               _free;     // free list
    size_t             _nalloc;   // count of allocated


    static BufNode *Buf2Node(pointer_type abuf) 
    {
        return (BufNode*)(((byte_pointer)abuf)-FIELD_OFFSET(BufNode, buf));
    }
    Node *insert(Node *pNode, Node &toList)
    {
        pNode->detach();
        return pNode->append(&toList);
    }

public:

    FixedSizeMemory(size_t elemCount)
        : ElemSize(ElemSizeT), ElemCount(elemCount)
        , _alloc(NULL), _free(NULL)
        , _nalloc(0)
    {
        BufNode *pNode = Mallocator::malloc(sizeof(BufNode)*elemCount);
        if(pNode==NULL) throw std::bad_alloc();
        memset(pNode, 0, sizeof(BufNode)*elemCount);
        for(int i=0; i<elemCount-1; ++i)
            pNode[i].next = &pNode[i+1];
        pNode[elemCount-1].next = NULL;
        _free.next = pNode;
    }
    pointer_type allocate()
    {
        BufNode *pNode = _free.next;
        if(pNode == NULL) return NULL;
        insert(pNode, _alloc);
        _nalloc++;
        return (pointer_type)pNode->buf;
    }
    void deallocate(pointer_type p)
    {
        BufNode *pNode = Buf2Node(p);
        insert(p, _free);
        _nalloc--;
    }
    size_t size() { return _nalloc;}

};  // class FixedSizeMemory
}  // namespace ftl
