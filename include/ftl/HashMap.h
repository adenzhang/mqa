// fatl::HashMap, a fast and flexible hash map.
//
// Created by Zhang Jie in Nov-2013
//
// It adopts dual interfaces, one compatible with boost::unordered_map iterator interace and one similar with ATL map interface.
// Usually ATL interface will be slightly faster.
// So far, it has implemented all major functions -- bucket number set, look up, insert, erase, and clear.
// The enhancements are:
//     - Customizable HashBinPolicy, which is used to calculate number of hash bins (by ReserveBins) and bin index (by GetBinIndex). Now two policies are supported.
//       PrimeNumberHashPolicy, popularly used ATL and other maps.
//       MaskHashPolicy, a faster policy, requiring number of bins be power of 2 size. The bin index is fast located by bit masking of hash value.
//     - When looking up or setting value, a hash value can be directly passed so that it will not be recalculated internally.
//     - Internal node buffering is supported, which speeds up allocation/deallocation.
//
// And there are some limitations and some functions have not been implemented.
//     - Can not get previous key-value pair, which means iterator can not do operator--(). But operator++() is supported and useful enough most of the time.
//     - location/iterator based insertion is not supported. This is not a problem in most hash map use cased.
//     - Copy constructor and assignment are not implemented yet.
//
// @revision 0.1 2013-12-03: major functions implemented.
//
// @revision 0.2 2013-12-06: add rehash functions.
//
//

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#ifdef _DEBUG
#define FSTASSERT(a) assert(a)
#define FSTENSURE(a) assert(a)
#define FSTASSUME(a)
#else
#define FSTENSURE(a)
#define FSTASSUME(a)
#define FSTASSERT(a)
#endif


namespace fastl{
    struct __POSITION
    {
    };
    typedef __POSITION* POSITION;

    template <typename K, typename V>
    class Pair :
        public __POSITION
    {
    public:
        typedef const K& KINARG;
        typedef K&       KOUTARG;
        typedef const V& VINARG;
        typedef V&       VOUTARG;
    protected:
        Pair( KINARG key )
            : _key( key )
            , first(key), second(_value)
        {}

    public:
        Pair(){}
        Pair( KINARG key, VINARG v ) :
        _key(key), _value(v)
            , first(key), second(_value) {}

        const K     _key;
        V           _value;
        const K      first;
        V&           second;
    };
    template< typename K, typename V>
    class Node :
        public Pair<K, V>
    {
    public:
        typedef const K& KINARG;
        typedef K&       KOUTARG;
        typedef const V& VINARG;
        typedef V&       VOUTARG;
    public:
        Node( KINARG key, size_t nHash )
            : Pair<K, V>( key )
            , _nHash( nHash )
            , _pNext( NULL )
        {}
        Node()
            : _nHash( 0 )
            , _pNext( NULL )
        {}

    public:
        size_t GetHash() const throw()
        {
            return( _nHash );
        }
        void SetNext(Node *pNext) {
            _pNext = pNext;
        }
        // return removed next
        Node *RemoveNext() {
            Node *ret = _pNext;
            if( _pNext )
                _pNext = _pNext->_pNext;
            return ret;
        }

    public:
        Node* _pNext;
        size_t _nHash;
    };

    template <typename Node_T>
    struct NodeBuffer
    {
        typedef Node_T node_type;

        NodeBuffer(size_t nReserve=1)
            : _nReserve(nReserve)
            , _nHiReserve(2*_nReserve)
            , _nCount(0)
            , _pElem(NULL)
        {}

        node_type *allocate() {
            node_type *ret = NULL;
            if( _nCount == 0 ) {
                ret = (node_type*)malloc(sizeof(node_type));
                while( _nCount < _nReserve ){
                    node_type *pNode = (node_type*)malloc(sizeof(node_type));
                    pNode->SetNext(_pElem);
                    _pElem = pNode;
                    _nCount++;
                }
            }else{
                ret = _pElem;
                _pElem = _pElem->_pNext;
                _nCount--;
            }
            return ret;
        }
        void deallocate(node_type *node) {
            node->SetNext(_pElem);
            _pElem = node;
            _nCount++;
            if( _nCount >= _nHiReserve ) {
                while(_nCount > _nReserve) {
                    node_type *pNode = _pElem;
                    _pElem = _pElem->_pNext;
                    free(pNode);
                    _nCount--;
                }
            }
        }
        void clear() {
            while( _nCount > 0 ) {
                node_type *pNode = _pElem;
                _pElem = _pElem->_pNext;
                free(pNode);
                _nCount--;
            }
        }

        const size_t  _nReserve;
        const size_t  _nHiReserve;
        node_type    *_pElem;
        size_t        _nCount;
    };

    class PrimeNumberHashPolicy
    {
    private:
        size_t _nBins;
    public:
        PrimeNumberHashPolicy(size_t nBins){
            ReserveBins(nBins);
        }
        inline size_t GetBinSize() const{ return _nBins;}

        inline size_t GetBinIndex(size_t hash) const{
            return hash%_nBins;
        }
        inline size_t ReserveBins(size_t nElements) {
            // List of primes such that s_anPrimes[i] is the smallest prime greater than 2^(5+i/3)
            const size_t nMax = 0xffffffff;
            static const size_t s_anPrimes[] =
            {
                17, 23, 29, 37, 41, 53, 67, 83, 103, 131, 163, 211, 257, 331, 409, 521, 647, 821,
                1031, 1291, 1627, 2053, 2591, 3251, 4099, 5167, 6521, 8209, 10331,
                13007, 16411, 20663, 26017, 32771, 41299, 52021, 65537, 82571, 104033,
                131101, 165161, 208067, 262147, 330287, 416147, 524309, 660563,
                832291, 1048583, 1321139, 1664543, 2097169, 2642257, 3329023, 4194319,
                5284493, 6658049, 8388617, 10568993, 13316089, nMax
            };

            size_t nBins = (size_t)(nElements);
            size_t nBinsEstimate = size_t(  nMax < nBins ? nMax : nBins );

            // Find the smallest prime greater than our estimate
            int iPrime = 0;
            while( nBinsEstimate > s_anPrimes[iPrime] )
                iPrime++;

            if( s_anPrimes[iPrime] == nMax )
                return( _nBins = nBinsEstimate );
            else
                return( _nBins = s_anPrimes[iPrime] );
        }
    };
    struct MaskHashPolicy
    {
    private:
        size_t _nBins;
        size_t _BinsMask;
    public:
        MaskHashPolicy(size_t nBins){
            ReserveBins(nBins);
        }
        inline size_t GetBinSize() const { return _nBins;}
        inline size_t GetBinIndex(size_t hash) const {
            return hash&_BinsMask;
        }
        inline size_t ReserveBins(size_t nElements) {
            const size_t nMax = -1;
            if(nElements<16) nElements = 16;
            nElements--;
            _nBins = size_t(  nMax < nElements ? nMax : nElements );
            // find the ceiling 2^i;
            size_t k = 0, nBits = sizeof(size_t)*8;
            for(int i=0;i<nBits-1; ++i) {
                if( (_nBins>>i) & 1 )k = i;
            }
            _nBins = ((size_t)1)<<(k+1);
            if(_nBins<nElements+1)
                _nBins <<= 1;
            _BinsMask = _nBins-1;
            return _nBins;
        }
    };

    template< typename KeyType >
    struct DefaultKeyTraits
    {
        static size_t Hash(const KeyType& k) {
            return size_t(k);
        }
        static bool CompareElements(const KeyType& k1, const KeyType& k2) {
            return k1 == k2;
        }
    };

    template< typename K, typename V, class KTraits = DefaultKeyTraits< K >, class HashBinPolicy=MaskHashPolicy >
    class HashMap
        : public HashBinPolicy
    {
    private:
        // copy constructor and assignment are not allowed.
        HashMap(const HashMap&){}
        HashMap& operator=(const HashMap&){}
    public:
        // Atl compatible types
        typedef const K& KINARG;
        typedef K&       KOUTARG;
        typedef const V& VINARG;
        typedef V&       VOUTARG;

    public:

        // boost compatible types
        typedef Pair<K, V>      value_type;
        typedef V               mapped_type;
        typedef K               key_type;
        typedef value_type&     reference;    // lvalude of value_type

        typedef Node<K,V>       node_type;
    public:
        //-------- stl like iterator operations
        // forward iterator
        template<typename Pair_t>
        class iterator_t
        {
            Pair_t         *pair;
            HashMap        *pMap;
        public:
            iterator_t(Pair_t* p, HashMap* m):pair(p), pMap(m){}

            Pair_t* operator->() {
                return pair;
            }
            Pair_t& operator*() {
                return *pair;
            }
            iterator_t& operator++() {
                if( pair == NULL) return *this;

                node_type* pNode = static_cast< node_type* >( pair );
                node_type* pNext = pMap->FindNextNode( pNode );

                pair = (Pair_t*) pNext ;
                return *this;
            }
            iterator_t operator++(int) {
                iterator_t tmp(*this);
                operator++();
                return tmp;
            }
            bool operator==(const iterator_t& it) {
                return pMap == it.pMap && pair == it.pair;
            }
            bool operator!=(const iterator_t& it) {
                return !operator==(it);
            }
        };
        typedef iterator_t<value_type> iterator;
        typedef iterator_t<const value_type> const_iterator;

        inline void insert(const value_type& p) {
            SetAt(p.first, p.second);
        }
        inline void insert(const std::pair<K const, V>& p) {
            SetAt(p.first, p.second);
        }
        iterator begin(){
            return iterator(static_cast<value_type*>(GetStartPosition()), this);
        }
        iterator end() {
            return iterator(NULL, this);
        }
        inline iterator erase(KINARG key) {
            node_type *p = Lookup(key);
            if(p==NULL) return iterator(NULL, this);
            node_type *next = FindNextNode(p);
            RemoveNode(p, NULL);
            return iterator(next, this);
        }
        template <typename iterator_T>
        inline iterator erase(iterator_T it) {
            iterator_T itNext = it;
            itNext++;
            RemoveAtPos(&(*it));
            return itNext;
        }
        const_iterator cbegin(){
            return const_iterator(static_cast<const value_type*>(GetStartPosition()), this);
        }
        const_iterator cend(){
            return const_iterator(NULL, this);
        }
        iterator find(KINARG key){
            return iterator(Lookup(key), this);
        }
        const_iterator find(KINARG key) const {
            return const_iterator(Lookup(key), this);
        }
        size_t size()const{
            return GetCount();
        }
        bool empty()const{
            return IsEmpty();
        }
        void clear(){
            RemoveAll();
        }

        size_t bucket_count() {
            return HashBinPolicy::GetBinSize();
        }
        void rehash(size_t n) {
            Rehash(n);
        }

    public:
        HashMap(size_t nBins=16, size_t nBufferNode=32)
            :_ppBins( NULL )
            , HashBinPolicy( nBins )
            , _nElements( 0 )
            , _nodeBuffer(nBufferNode)
        {
            InitHashTable(HashBinPolicy::GetBinSize());
        }
        ~HashMap() {
            RemoveAll();
        }

        inline bool Lookup( KINARG key, VOUTARG value ) const {
            size_t iBin;
            size_t nHash;
            node_type* pNode;
            node_type* pPrev;

            pNode = (node_type*)GetNode( key, iBin, nHash, pPrev );
            if( pNode == NULL )
                return( false );

            value = pNode->_value;
            return( true );
        }
        inline value_type* Lookup( KINARG key ) const {
            size_t iBin;
            size_t nHash;
            //node_type* pNode;
            node_type* pPrev;

            return (value_type*)GetNode( key, iBin, nHash, pPrev );
        }
        inline value_type* LookupHash( KINARG key, size_t hash )
        {
            size_t iBin;
            //size_t nHash;
            node_type* pNode;
            node_type* pPrev;

            pNode = GetNode( key, iBin, pPrev, hash );

            return( pNode );
        }
        inline bool RemoveKey( KINARG key ) throw()
        {
            node_type* pNode;
            size_t iBin;
            size_t nHash;
            node_type* pPrev;

            pPrev = NULL;
            pNode = GetNode( key, iBin, nHash, pPrev );
            if( pNode == NULL )
                return( false );

            RemoveNode( pNode, pPrev );
            return( true );
        }
        inline void RemoveAtPos( POSITION pos ) throw()
        {
            FSTENSURE( pos != NULL );

            node_type* pNode = static_cast< node_type* >( pos );
            node_type* pPrev = NULL;
            size_t iBin = HashBinPolicy::GetBinIndex( pNode->GetHash() );

            FSTASSUME( _ppBins[iBin] != NULL );
            if( pNode == _ppBins[iBin] )
            {
                pPrev = NULL;
            }
            else
            {
                pPrev = _ppBins[iBin];
                while( pPrev->_pNext != pNode )
                {
                    pPrev = pPrev->_pNext;
                    FSTASSERT( pPrev != NULL );
                }
            }
            RemoveNode( pNode, pPrev );
        }
        inline void SetValueAt( POSITION pos, VINARG value )
        {
            node_type* pNode = static_cast< node_type* >( pos );
            pNode->_value = value;
        }
        inline POSITION SetAt( KINARG key, VINARG value )
        {
            node_type* pNode;
            size_t iBin;
            size_t nHash;
            node_type* pPrev;

            pNode = GetNode( key, iBin, nHash, pPrev );
            if( pNode == NULL )
            {
                pNode = CreateNode( key, iBin, nHash );
                pNode->_value = value;
            }
            else
            {
                pNode->_value = value;
            }

            return( POSITION( pNode ) );
        }
        inline POSITION SetAtHash( KINARG key, size_t nHash, VINARG value )
        {
            node_type* pNode;
            size_t iBin;
            node_type* pPrev;

            pNode = GetNode( key, iBin, pPrev, nHash );
            if( pNode == NULL )
            {
                pNode = CreateNode( key, iBin, nHash );
                pNode->_value = value;
            }
            else
            {
                pNode->_value = value;
            }

            return( POSITION( pNode ) );
        }
        void RemoveAll()
        {
            if( _ppBins != NULL )
            {
                for( size_t iBin = 0; iBin < HashBinPolicy::GetBinSize(); iBin++ )
                {
                    node_type* pNext;
                    pNext = _ppBins[iBin];
                    while( pNext != NULL ){
                        node_type* pKill;
                        pKill = pNext;
                        pNext = pNext->_pNext;
                        FreeNode( pKill );
                    }
                }
            }

            delete[] _ppBins;
            _ppBins = NULL;
            _nElements = 0;
            _nodeBuffer.clear();
        }
        POSITION GetStartPosition() const
        {
            if( IsEmpty() ) {
                return( NULL );
            }

            for( size_t iBin = 0; iBin < HashBinPolicy::GetBinSize(); iBin++ )
            {
                if( _ppBins[iBin] != NULL )
                {
                    return( POSITION( _ppBins[iBin] ) );
                }
            }

            return( NULL );
        }
        inline const K& GetKeyAt( POSITION pos ) const
        {
            FSTENSURE( pos != NULL );

            node_type* pNode = (node_type*)pos;

            return( pNode->_key );
        }
        inline void GetAt( POSITION pos, KOUTARG key, VOUTARG value ) const
        {
            FSTENSURE( pos != NULL );

            node_type* pNode = static_cast< node_type* >( pos );

            key = pNode->_key;
            value = pNode->_value;
        }
        inline bool IsEmpty() const
        {
            return( _nElements == 0 );
        }
        inline size_t GetCount() const
        {
            return( _nElements );
        }
        inline const value_type *GetNext( POSITION& pos ) const
        {
            node_type* pNode;
            node_type* pNext;

            FSTASSUME( _ppBins != NULL );
            FSTASSERT( pos != NULL );

            pNode = (node_type*)pos;
            pNext = FindNextNode( pNode );

            pos = POSITION( pNext );

            return( pNode );
        }
        inline value_type *GetNext( POSITION& pos ){
            node_type* pNode;
            node_type* pNext;

            FSTASSUME( _ppBins != NULL );
            FSTASSERT( pos != NULL );

            pNode = (node_type*)pos;
            pNext = FindNextNode( pNode );

            pos = POSITION( pNext );

            return( pNode );
        }
        inline V& operator[]( KINARG key )
        {
            node_type* pNode;
            size_t iBin;
            size_t nHash;
            node_type* pPrev;

            pNode = GetNode( key, iBin, nHash, pPrev );
            if( pNode == NULL )
            {
                pNode = CreateNode( key, iBin, nHash );
            }

            return( pNode->_value );
        }
        // if nBins==0, set it to elements numbers.
        void Rehash(size_t nBins=0)
        {
            size_t oldBinSize = HashBinPolicy::GetBinSize();
            HashBinPolicy::ReserveBins( nBins == 0? _nElements: nBins );

            if( oldBinSize == HashBinPolicy::GetBinSize() ) // no changes
                return;

            if( _ppBins == NULL ) {
                InitHashTable(HashBinPolicy::GetBinSize());
            }else{
                node_type** ppBins = new node_type*[nBins];
                FSTENSURE( ppBins != NULL );
                memset( ppBins, 0, nBins*sizeof( node_type* ) );
                for( size_t iSrcBin = 0; iSrcBin < oldBinSize; iSrcBin++ )
                {
                    node_type* pNode;

                    pNode = _ppBins[iSrcBin];
                    while( pNode != NULL ){
                        node_type* pNext;
                        size_t iDestBin;

                        pNext = pNode->_pNext;
                        iDestBin = HashBinPolicy::GetBinIndex(pNode->GetHash());
                        pNode->_pNext = ppBins[iDestBin];
                        ppBins[iDestBin] = pNode;

                        pNode = pNext;
                    }
                }
                delete[] _ppBins;
                _ppBins = ppBins;
            }
        }

    protected:
        bool InitHashTable(size_t nBins)
        {
            if( _ppBins != NULL )
            {
                delete[] _ppBins;
                _ppBins = NULL;
            }
            {
                _ppBins = new node_type*[nBins];
                if( _ppBins == NULL )
                {
                    return false;
                }
                memset( _ppBins, 0, sizeof( node_type* )*nBins );
            }
            return true;
        }
        inline node_type *CreateNode( KINARG key, size_t iBin, size_t nHash )
        {
            node_type* pNode;

            if( _ppBins == NULL )
            {
                bool bSuccess;

                bSuccess = InitHashTable( HashBinPolicy::GetBinSize() );
                if( !bSuccess )
                {
                    throw std::bad_alloc();
                }
            }

            pNode = NewNode( key, iBin, nHash );

            return( pNode );
        }

        inline node_type* GetNode(KINARG key, size_t& iBin, size_t& nHash, node_type*& pPrev )const{
            node_type* pFollow;

            nHash = KTraits::Hash( key );
            iBin = HashBinPolicy::GetBinIndex(nHash);

            if( _ppBins == NULL )
            {
                return( NULL );
            }

            pFollow = NULL;
            pPrev = NULL;
            for( node_type* pNode = _ppBins[iBin]; pNode != NULL; pNode = pNode->_pNext )
            {
                if( (pNode->GetHash() == nHash) && KTraits::CompareElements( pNode->_key, key ) )
                {
                    pPrev = pFollow;
                    return( pNode );
                }
                pFollow = pNode;
            }

            return( NULL );
        }
        inline node_type* GetNode(KINARG key, size_t& iBin, node_type*& pPrev, size_t nHash )const{
            node_type* pFollow;

            iBin = HashBinPolicy::GetBinIndex(nHash);

            if( _ppBins == NULL )
            {
                return( NULL );
            }

            pFollow = NULL;
            pPrev = NULL;
            for( node_type* pNode = _ppBins[iBin]; pNode != NULL; pNode = pNode->_pNext )
            {
                if( (pNode->GetHash() == nHash) && KTraits::CompareElements( pNode->_key, key ) )
                {
                    pPrev = pFollow;
                    return( pNode );
                }
                pFollow = pNode;
            }

            return( NULL );
        }
        inline void RemoveNode( node_type* pNode, node_type* pPrev ) throw()
        {
            FSTENSURE( pNode != NULL );

            size_t iBin = HashBinPolicy::GetBinIndex( pNode->GetHash() );

            if( pPrev == NULL )
            {
                FSTASSUME( _ppBins[iBin] == pNode );
                _ppBins[iBin] = pNode->_pNext;
            }
            else
            {
                FSTASSERT( pPrev->_pNext == pNode );
                pPrev->_pNext = pNode->_pNext;
            }
            FreeNode( pNode );
        }
        inline void FreeNode( node_type* pNode )
        {
            FSTENSURE( pNode != NULL );

            pNode->~node_type();
            _nodeBuffer.deallocate(pNode);

            FSTASSUME( _nElements > 0 );
            _nElements--;

        }
        inline node_type *NewNode( KINARG key, size_t iBin, size_t nHash )
        {
            node_type* pNewNode = _nodeBuffer.allocate();
            FSTENSURE( pNewNode != NULL );
            new (pNewNode) node_type(key, nHash);
            _nElements++;

            // add to head
            if(NULL != _ppBins[iBin])
                pNewNode->SetNext(_ppBins[iBin]);
            _ppBins[iBin] = pNewNode;

            return( pNewNode );
        }
        inline node_type *FindNextNode( node_type* pNode ) const throw()
        {
            node_type* pNext;

            if(pNode == NULL)
            {
                return NULL;
            }

            if( pNode->_pNext != NULL )
            {
                pNext = pNode->_pNext;
            }
            else
            {
                size_t iBin;

                pNext = NULL;
                iBin = HashBinPolicy::GetBinIndex(pNode->GetHash())+1;
                while( (pNext == NULL) && (iBin < HashBinPolicy::GetBinSize()) )
                {
                    if( _ppBins[iBin] != NULL )
                    {
                        pNext = _ppBins[iBin];
                    }

                    iBin++;
                }
            }

            return( pNext );
        }
        node_type               **_ppBins;
        NodeBuffer<node_type>     _nodeBuffer;
        size_t                    _nElements;
    };
}  // namespace fastl
#endif // HASHTABLE_H_

