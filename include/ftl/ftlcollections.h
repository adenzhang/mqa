#ifndef FTLCOLLECTIONS_HPP
#define FTLCOLLECTIONS_HPP

/// ftlcollections implements array, map, list, RBMap with the same algorithm as ATL collections.
/// Author: Jie.Zhang
/// Date: 2013-August
/// 
#include <ctype.h>
#include <wctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32
#include <string>
#endif

#ifndef _MSC_VER
#include <stdint.h>
#endif

#ifndef FtlThrow
#ifndef _FTL_CUSTOM_THROW
#define FtlThrow ftl::FtlThrowImpl
#endif
#endif // FtlThrow

#ifndef FTLENSURE_THROW
#define FTLENSURE_THROW(expr, hr)          \
	do {                                       \
	int __atl_condVal=!!(expr);            \
	FTLASSERT(__atl_condVal);              \
	if(!(__atl_condVal)) FtlThrow(hr);     \
	} while (0)
#endif // FTLENSURE

#ifndef _ASSERTE
#define _ASSERTE(expr) ((void)0)
#endif

#ifndef FTLENSURE
#define FTLENSURE(expr) FTLENSURE_THROW(expr, E_FAIL)
#endif // FTLENSURE

#ifndef FTLASSERT
#define FTLASSERT(expr) _ASSERTE(expr)
#endif // FTLASSERT

#define FTLASSUME(expr) do { FTLASSERT(expr); /*__analysis_assume(!!(expr));*/ } while(0)

#ifndef FTLTRYALLOC
#define FTLTRYALLOC(x) x;
#endif	//FTLTRYALLOC

#define NOOP ((void)0)

// if _FTLTRY is defined before including this file then 
// _FTLCATCH and _FTLRETHROW should be defined as well.
#ifndef _FTLTRY
#define _FTLTRY try
#define _FTLCATCH( e ) catch(FltException& e)
#define _FTLCATCHALL(...) catch(...)
#define _FTLDELETEEXCEPTION(e)
#define _FTLRETHROW throw
//#define _FTLCATCH( e ) NOOP; // __pragma(warning(push)) __pragma(warning(disable: 4127)) if( false ) __pragma(warning(pop))
//#define _FTLCATCHALL() NOOP; //__pragma(warning(push)) __pragma(warning(disable: 4127)) if( false ) __pragma(warning(pop))
//#define _FTLDELETEEXCEPTION(e)
//#define _FTLRETHROW
#endif	// _FTLTRY

//#endif	//_CPPUNWIND

#ifndef FTLTRY
#define FTLTRY(x) FTLTRYALLOC(x)
#endif	//FTLTRY

#define FtlChecked
#define FTL_NOINLINE
#define FTLTRACE            NOOP
#define FTLASSERT_VALID(x)  NOOP;
//========================== namespace ftl ===================================================
namespace ftl{

	typedef unsigned long       DWORD;
    typedef long                HRESULT;
    typedef unsigned long       ULONG;
    typedef unsigned int        UINT;
    //typedef uintptr_t           ULONG_PTR;
#ifdef WIN32
    typedef __int64             INT64;
    typedef unsigned __int64    UINT64;
#else
    typedef long long           INT64;
    typedef unsigned long long  UINT64;
    typedef intptr_t            INT_PTR;
#endif

#define STDCALL
#define SUCCEEDED(hr) ((HRESULT)(hr) >= 0)
#define FAILED(hr) ((HRESULT)(hr) < 0)

#ifndef S_OK
#define S_OK                                   ((HRESULT)0x00000000L)
#endif
#ifndef E_INVALIDARG
#define E_INVALIDARG                           ((HRESULT)0x80070057L)
#endif
#ifndef E_OUTOFMEMORY
#define E_OUTOFMEMORY                          ((HRESULT)0x8007000EL)
#endif
#ifndef E_FAIL
#define E_FAIL                                 ((HRESULT)0x80004005L)
#endif

//#define EXCEPTION_MAXIMUM_PARAMETERS 15 // maximum number of exception parameters

#ifndef NULL
#define NULL 0
#endif

//#endif

	const DWORD atlTraceMap = 0;
    struct FtlException {
        ULONG code;
        FtlException(ULONG code):code(code){}
    };

    inline void /*__declspec(noreturn)*/ _FtlRaiseException( DWORD dwExceptionCode)
	{
//        RaiseException( dwExceptionCode, dwExceptionFlags, 0, NULL );
        throw(FtlException(dwExceptionCode));
	}
	// Throw a Exception with th given HRESULT
#if !defined( _FTL_CUSTOM_THROW )  // You can define your own FtlThrow

	FTL_NOINLINE inline void STDCALL FtlThrowImpl( HRESULT hr )
	{
		//FTLTRACE(atlTraceException, 0, _T("FtlThrow: hr = 0x%x\n"), hr );
//		FTLASSERT( false );
		DWORD dwExceptionCode = hr;
//		switch(hr)
//		{
//		case E_OUTOFMEMORY:
//			dwExceptionCode = STATUS_NO_MEMORY;
//			break;
//		default:
//			dwExceptionCode = EXCEPTION_ILLEGAL_INSTRUCTION;
//		}
		_FtlRaiseException((DWORD)dwExceptionCode);
	}
#endif

	struct __POSITION
	{
	};
	typedef __POSITION* POSITION;

//=========================== ftlallolc  =====================================================

/* 
This is	more than a	little unsatisfying. /Wp64 warns when we convert a size_t to an	int
because	it knows such a	conversion won't port. 
But, when we have overloaded templates,	there may well exist both conversions and we need 
to fool	the	warning	into not firing	on 32 bit builds
*/
#if !defined(_FTL_W64)
#if !defined(__midl) &&	(defined(_X86_)	|| defined(_M_IX86))
#define	_FTL_W64 __w64
#else
#define	_FTL_W64
#endif
#endif

/* Can't use ::std::numeric_limits<T> here because we don't want to introduce a new	
   dependency of this code on SCL
*/

template<typename T>
class FtlLimits;

template<>
class FtlLimits<int _FTL_W64>
{
public:
	static const int _Min=INT_MIN;
	static const int _Max=INT_MAX;
};

template<>
class FtlLimits<unsigned int _FTL_W64>
{
public:
	static const unsigned int _Min=0;
	static const unsigned int _Max=UINT_MAX;
};

template<>
class FtlLimits<long _FTL_W64>
{
public:
	static const long _Min=LONG_MIN;
	static const long _Max=LONG_MAX;
};

template<>
class FtlLimits<unsigned long _FTL_W64>
{
public:
	static const unsigned long _Min=0;
	static const unsigned long _Max=ULONG_MAX;
};

template<>
class FtlLimits<long long>
{
public:
	static const long long _Min=LLONG_MIN;
	static const long long _Max=LLONG_MAX;
};

template<>
class FtlLimits<unsigned long long>
{
public:
	static const unsigned long long _Min=0;
	static const unsigned long long _Max=ULLONG_MAX;
};

/* generic version */
template<typename T>
inline HRESULT FtlAdd(T* ptResult, T tLeft, T tRight)
{
	if(::ftl::FtlLimits<T>::_Max-tLeft < tRight)
	{
		return E_INVALIDARG;
	}
	*ptResult= tLeft + tRight;
	return S_OK;
}

/* generic but compariatively slow version */
template<typename T>
inline HRESULT FtlMultiply(T* ptResult,	T tLeft, T tRight)
{
	/* avoid divide 0 */
	if(tLeft==0)
	{
		*ptResult=0;
		return S_OK;
	}
	if(::ftl::FtlLimits<T>::_Max/tLeft < tRight)
	{
		return E_INVALIDARG;
	}
	*ptResult= tLeft * tRight;
	return S_OK;
}

/* fast	version	for	32 bit integers	*/
template<>
inline HRESULT FtlMultiply(int _FTL_W64	*piResult, int _FTL_W64	iLeft, int _FTL_W64 iRight)
{
    INT64 i64Result=static_cast<INT64>(iLeft) * static_cast<INT64>(iRight);
	if(i64Result>INT_MAX || i64Result < INT_MIN)
	{
		return E_INVALIDARG;
	}
	*piResult=static_cast<int _FTL_W64>(i64Result);
	return S_OK;
}

template<>
inline HRESULT FtlMultiply(unsigned int	_FTL_W64 *piResult, unsigned int _FTL_W64 iLeft, unsigned int _FTL_W64 iRight)
{
    UINT64 i64Result=static_cast<UINT64>(iLeft) * static_cast<UINT64>(iRight);
	if(i64Result>UINT_MAX)
	{
		return E_INVALIDARG;
	}
	*piResult=static_cast<unsigned int _FTL_W64>(i64Result);
	return S_OK;
}

template<>
inline HRESULT FtlMultiply(long	_FTL_W64 *piResult, long _FTL_W64 iLeft, long _FTL_W64 iRight)
{
    INT64 i64Result=static_cast<INT64>(iLeft) * static_cast<INT64>(iRight);
	if(i64Result>LONG_MAX || i64Result < LONG_MIN)
	{
		return E_INVALIDARG;
	}
	*piResult=static_cast<long _FTL_W64>(i64Result);
	return S_OK;
}

template<>
inline HRESULT FtlMultiply(unsigned long _FTL_W64 *piResult, unsigned long _FTL_W64 iLeft, unsigned long _FTL_W64 iRight)
{
    UINT64 i64Result=static_cast<UINT64>(iLeft) * static_cast<UINT64>(iRight);
	if(i64Result>ULONG_MAX)
	{
		return E_INVALIDARG;
	}
	*piResult=static_cast<unsigned long _FTL_W64>(i64Result);
	return S_OK;
}

//================== FtlPlex ================================================================

	struct Plex     // warning variable length structure
	{
		Plex* pNext;
#if (_AFX_PACKING >= 8)
		DWORD dwReserved[1];    // align on 8 byte boundary
#endif
		// BYTE data[maxNum*elementSize];

		void* data() { return this+1; }

		static Plex* Create(Plex*& head, size_t nMax, size_t cbElement);
		// like 'calloc' but no zero fill
		// may throw memory exceptions

		void FreeDataChain();       // free this one and links
	};

	inline Plex* Plex::Create( Plex*& pHead, size_t nMax, size_t nElementSize )
	{
		Plex* pPlex;

		FTLASSERT( nMax > 0 );
		FTLASSERT( nElementSize > 0 );

		size_t nBytes=0;
		if( 	FAILED(::ftl::FtlMultiply(&nBytes, nMax, nElementSize)) ||
			FAILED(::ftl::FtlAdd(&nBytes, nBytes, sizeof(Plex))) )
		{
			return NULL;
		}
		pPlex = static_cast< Plex* >( malloc( nBytes ) );
		if( pPlex == NULL )
		{
			return( NULL );
		}

		pPlex->pNext = pHead;
		pHead = pPlex;

		return( pPlex );
	}

	inline void Plex::FreeDataChain()
	{
		Plex* pPlex;

		pPlex = this;
		while( pPlex != NULL )
		{
			Plex* pNext;

			pNext = pPlex->pNext;
			free( pPlex );
			pPlex = pNext;
		}
	}
//=========================== element traits ====================================================
	template< typename T >
	class ElementTraitsBase
	{
	public:
		typedef const T& INARGTYPE;
		typedef T& OUTARGTYPE;

		static void CopyElements( T* pDest, const T* pSrc, size_t nElements )
		{
			for( size_t iElement = 0; iElement < nElements; iElement++ )
			{
				pDest[iElement] = pSrc[iElement];
			}
		}

		static void RelocateElements( T* pDest, T* pSrc, size_t nElements )
		{
			// A simple memmove works for nearly all types.
			// You'll have to override this for types that have pointers to their
			// own members.
            FtlChecked::memcpy( pDest, nElements*sizeof( T ), pSrc, nElements*sizeof( T ));
		}
	};

	template< typename T >
	class DefaultHashTraits
	{
	public:
		static ULONG Hash( const T& element ) throw()
		{
			return( ULONG( uintptr_t( element ) ) );
		}
	};

	template< typename T >
	class DefaultCompareTraits
	{
	public:
		static bool CompareElements( const T& element1, const T& element2 )
		{
			return( (element1 == element2) != 0 );  // != 0 to handle overloads of operator== that return BOOL instead of bool
		}

		static int CompareElementsOrdered( const T& element1, const T& element2 )
		{
			if( element1 < element2 )
			{
				return( -1 );
			}
			else if( element1 == element2 )
			{
				return( 0 );
			}
			else
			{
				FTLASSERT( element1 > element2 );
				return( 1 );
			}
		}
	};

	template< typename T >
	class DefaultElementTraits :
		public ElementTraitsBase< T >,
		public DefaultHashTraits< T >,
		public DefaultCompareTraits< T >
	{
	};

	template< typename T >
	class ElementTraits :
		public DefaultElementTraits< T >
	{
	};

	template < typename T >
	class DefaultCharTraits
	{
	};

	template <>
	class DefaultCharTraits<char>
	{
	public:
		static char CharToUpper(char x)
		{
			return (char)toupper(x);
		}

		static char CharToLower(char x)
		{
			return (char)tolower(x);
		}
	};

	template <>
	class DefaultCharTraits<wchar_t>
	{
	public:
		static wchar_t CharToUpper(wchar_t x)
		{
			return (wchar_t)towupper(x);
		}

		static wchar_t CharToLower(wchar_t x)
		{
			return (wchar_t)towlower(x);
		}
	};

    template<typename CHAR>
    class ElementTraits<std::basic_string<CHAR> >:
        public ElementTraitsBase< std::basic_string<CHAR> >,
        public DefaultHashTraits< std::basic_string<CHAR> >,
        public DefaultCompareTraits< std::basic_string<CHAR> >
    {
    public:
        typedef std::basic_string<CHAR> STRING_T;

//    template<>
//    class ElementTraits<std::string>:
//        public ElementTraitsBase< std::string >,
//        public DefaultHashTraits< std::string >,
//        public DefaultCompareTraits< std::string >
//    {
//    public:
//        typedef std::string STRING_T;

        typedef typename STRING_T::value_type         XCHAR;
        typedef XCHAR       *PXCHAR;
        typedef const XCHAR *PCXCHAR;

        typedef const STRING_T  &INARGTYPE;
        typedef STRING_T        &OUTARGTYPE;

        typedef DefaultCharTraits<XCHAR> CharTraits;

        static ULONG Hash(INARGTYPE str) {
            ULONG           nHash = 0;
            for (int i=0; str[i] != '\0'; ++i)
                nHash = (nHash<<5)+nHash+CharTraits::CharToUpper(str[i]);
            return( nHash );
        }
        static bool CompareElements( INARGTYPE str1, INARGTYPE str2 ) throw()
        {
            return( str1.compare( str2 ) == 0 );
        }

        static int CompareElementsOrdered( INARGTYPE str1, INARGTYPE str2 ) throw()
        {
            return( str1.compare( str2 ) );
        }

    };

    template<typename CHAR>
    class ElementTraitsI:
        public ElementTraitsBase< std::basic_string<CHAR> >,
        public DefaultHashTraits< std::basic_string<CHAR> >,
        public DefaultCompareTraits< std::basic_string<CHAR> >
    {
    public:
        typedef std::basic_string<CHAR> STRING_T;
        typedef typename STRING_T::value_type         XCHAR;
        typedef XCHAR       *PXCHAR;
        typedef const XCHAR *PCXCHAR;

        typedef const STRING_T  &INARGTYPE;
        typedef STRING_T        &OUTARGTYPE;

        typedef DefaultCharTraits<XCHAR> CharTraits;

        static ULONG Hash(INARGTYPE str) {
            ULONG           nHash = 0;
            for (int i=0; i<str.size(); ++i)
                nHash = (nHash<<5)+nHash+CharTraits::CharToUpper(str[i]);
            return( nHash );
        }
        static bool CompareElements( INARGTYPE str1, INARGTYPE str2 ) throw()
        {
            for (int i=0; i<str1.size() && i<str2.size(); ++i)
                if( CharTraits::CharToUpper(str1[i]) != CharTraits::CharToUpper(str2[i]) ) return false;
            return true;
        }

        static int CompareElementsOrdered( INARGTYPE str1, INARGTYPE str2 ) throw()
        {
            int s = 0;
            for (int i=0; i<str1.size() && i<str2.size(); ++i) {
                s = CharTraits::CharToUpper(str1[i]) - CharTraits::CharToUpper(str2[i]);
                if(s==0) continue;
                return s;
            }
            return s;
        }

    };


    /*
	template< typename T, class CharTraits = DefaultCharTraits<T::XCHAR> >
	class StringElementTraitsI :
		public ElementTraitsBase< T >
	{
	public:
		typedef typename T::PCXSTR INARGTYPE;
		typedef T& OUTARGTYPE;

		static ULONG Hash( INARGTYPE str ) 
		{
			ULONG nHash = 0;

			const T::XCHAR* pch = str;

			FTLENSURE( pch != NULL );

			while( *pch != 0 )
			{
				nHash = (nHash<<5)+nHash+CharTraits::CharToUpper(*pch);
				pch++;
			}

			return( nHash );
		}

		static bool CompareElements( INARGTYPE str1, INARGTYPE str2 ) throw()
		{
			return( T::StrTraits::StringCompareIgnore( str1, str2 ) == 0 );
		}

		static int CompareElementsOrdered( INARGTYPE str1, INARGTYPE str2 ) throw()
		{
			return( T::StrTraits::StringCompareIgnore( str1, str2 ) );
		}
	};

	template< typename T >
	class StringRefElementTraits :
		public ElementTraitsBase< T >
	{
	public:
		static ULONG Hash( INARGTYPE str )
		{
			ULONG nHash = 0;

			const T::XCHAR* pch = str;

			FTLENSURE( pch != NULL );

			while( *pch != 0 )
			{
				nHash = (nHash<<5)+nHash+(*pch);
				pch++;
			}

			return( nHash );
		}

		static bool CompareElements( INARGTYPE element1, INARGTYPE element2 ) throw()
		{
			return( element1 == element2 );
		}

		static int CompareElementsOrdered( INARGTYPE str1, INARGTYPE str2 ) throw()
		{
			return( str1.Compare( str2 ) );
		}
	};
*/
	template< typename T >
	class PrimitiveElementTraits :
		public DefaultElementTraits< T >
	{
	public:
		typedef T INARGTYPE;
		typedef T& OUTARGTYPE;
	};

#define _DECLARE_PRIMITIVE_TRAITS( T ) \
	template<> \
	class ElementTraits< T > : \
	public PrimitiveElementTraits< T > \
	{ \
	};

	_DECLARE_PRIMITIVE_TRAITS( unsigned char )
		_DECLARE_PRIMITIVE_TRAITS( unsigned short )
		_DECLARE_PRIMITIVE_TRAITS( unsigned int )
		_DECLARE_PRIMITIVE_TRAITS( unsigned long )
        _DECLARE_PRIMITIVE_TRAITS( UINT64 )
		_DECLARE_PRIMITIVE_TRAITS( signed char )
		_DECLARE_PRIMITIVE_TRAITS( char )
		_DECLARE_PRIMITIVE_TRAITS( short )
		_DECLARE_PRIMITIVE_TRAITS( int )
		_DECLARE_PRIMITIVE_TRAITS( long )
        _DECLARE_PRIMITIVE_TRAITS( INT64 )
		_DECLARE_PRIMITIVE_TRAITS( float )
		_DECLARE_PRIMITIVE_TRAITS( double )
		_DECLARE_PRIMITIVE_TRAITS( bool )
#ifdef _NATIVE_WCHAR_T_DEFINED
		_DECLARE_PRIMITIVE_TRAITS( wchar_t )
#endif
		_DECLARE_PRIMITIVE_TRAITS( void* )

//=========================== array ========================================================
template< typename E, class ETraits = ElementTraits< E > >
	class Array
	{
	public:
		typedef typename ETraits::INARGTYPE INARGTYPE;
		typedef typename ETraits::OUTARGTYPE OUTARGTYPE;

	public:
		Array() throw();

		size_t GetCount() const throw();
		bool IsEmpty() const throw();
		bool SetCount( size_t nNewSize, int nGrowBy = -1 );

		void FreeExtra() throw();
		void RemoveAll() throw();

		const E& GetAt( size_t iElement ) const;
		void SetAt( size_t iElement, INARGTYPE element );
		E& GetAt( size_t iElement );

		const E* GetData() const throw();
		E* GetData() throw();

		void SetAtGrow( size_t iElement, INARGTYPE element );
		// Add an empty element to the end of the array
		size_t Add();
		// Add an element to the end of the array
		size_t Add( INARGTYPE element );
		size_t Append( const Array< E, ETraits >& aSrc );
		void Copy( const Array< E, ETraits >& aSrc );

		const E& operator[]( size_t iElement ) const;
		E& operator[]( size_t iElement );

		void InsertAt( size_t iElement, INARGTYPE element, size_t nCount = 1 );
		void InsertArrayAt( size_t iStart, const Array< E, ETraits >* paNew );
		void RemoveAt( size_t iElement, size_t nCount = 1 );

#ifdef _DEBUG
		void AssertValid() const;
#endif  // _DEBUG

	private:
		bool GrowBuffer( size_t nNewSize );

		// Implementation
	private:
		E* _pData;
		size_t _nSize;
		size_t _nMaxSize;
		int _nGrowBy;

	private:
		static void CallConstructors( E* pElements, size_t nElements );
		static void CallDestructors( E* pElements, size_t nElements ) throw();

	public:
		~Array() throw();

	private:
		// Private to prevent use
		Array( const Array& ) throw();
		Array& operator=( const Array& ) throw();
	}; // class array
	template< typename E, class ETraits >
	inline size_t Array< E, ETraits >::GetCount() const throw()
	{
		return( _nSize );
	}

	template< typename E, class ETraits >
	inline bool Array< E, ETraits >::IsEmpty() const throw()
	{
		return( _nSize == 0 );
	}

	template< typename E, class ETraits >
	inline void Array< E, ETraits >::RemoveAll() throw()
	{
		SetCount( 0, -1 );
	}

	template< typename E, class ETraits >
	inline const E& Array< E, ETraits >::GetAt( size_t iElement ) const
	{
		FTLASSERT( iElement < _nSize );
		if(iElement >= _nSize)
			FtlThrow(E_INVALIDARG);

		return( _pData[iElement] );
	}

	template< typename E, class ETraits >
	inline void Array< E, ETraits >::SetAt( size_t iElement, INARGTYPE element )
	{
		FTLASSERT( iElement < _nSize );
		if(iElement >= _nSize)
			FtlThrow(E_INVALIDARG);

		_pData[iElement] = element;
	}

	template< typename E, class ETraits >
	inline E& Array< E, ETraits >::GetAt( size_t iElement )
	{
		FTLASSERT( iElement < _nSize );
		if(iElement >= _nSize)
			FtlThrow(E_INVALIDARG);

		return( _pData[iElement] );
	}

	template< typename E, class ETraits >
	inline const E* Array< E, ETraits >::GetData() const throw()
	{
		return( _pData );
	}

	template< typename E, class ETraits >
	inline E* Array< E, ETraits >::GetData() throw()
	{
		return( _pData );
	}

	template< typename E, class ETraits >
	inline size_t Array< E, ETraits >::Add()
	{
		size_t iElement;

		iElement = _nSize;
		bool bSuccess=SetCount( _nSize+1 );
		if( !bSuccess )
		{
			FtlThrow( E_OUTOFMEMORY );
		}

		return( iElement );
	}

#pragma push_macro("new")
#undef new

	template< typename E, class ETraits >
	inline size_t Array< E, ETraits >::Add( INARGTYPE element )
	{
		size_t iElement;

		iElement = _nSize;
		if( iElement >= _nMaxSize )
		{
			bool bSuccess = GrowBuffer( iElement+1 );
			if( !bSuccess )
			{
				FtlThrow( E_OUTOFMEMORY );
			}
		}
		::new( _pData+iElement ) E( element );
		_nSize++;

		return( iElement );
	}

#pragma pop_macro("new")

	template< typename E, class ETraits >
	inline const E& Array< E, ETraits >::operator[]( size_t iElement ) const
	{
		FTLASSERT( iElement < _nSize );
		if(iElement >= _nSize)
			FtlThrow(E_INVALIDARG);

		return( _pData[iElement] );
	}

	template< typename E, class ETraits >
	inline E& Array< E, ETraits >::operator[]( size_t iElement ) 
	{
		FTLASSERT( iElement < _nSize );
		if(iElement >= _nSize)
			FtlThrow(E_INVALIDARG);

		return( _pData[iElement] );
	}

	template< typename E, class ETraits >
	Array< E, ETraits >::Array()  throw():
	_pData( NULL ),
		_nSize( 0 ),
		_nMaxSize( 0 ),
		_nGrowBy( 0 )
	{
	}

	template< typename E, class ETraits >
	Array< E, ETraits >::~Array() throw()
	{
		if( _pData != NULL )
		{
			CallDestructors( _pData, _nSize );
			free( _pData );
		}
	}

	template< typename E, class ETraits >
	bool Array< E, ETraits >::GrowBuffer( size_t nNewSize )
	{
		if( nNewSize > _nMaxSize )
		{
			if( _pData == NULL )
			{
				size_t nAllocSize =  size_t( _nGrowBy ) > nNewSize ? size_t( _nGrowBy ) : nNewSize ;
				_pData = static_cast< E* >( calloc( nAllocSize,sizeof( E ) ) );
				if( _pData == NULL )
				{
					return( false );
				}
				_nMaxSize = nAllocSize;
			}
			else
			{
				// otherwise, grow array
				size_t nGrowBy = _nGrowBy;
				if( nGrowBy == 0 )
				{
					// heuristically determine growth when nGrowBy == 0
					//  (this avoids heap fragmentation in many situations)
					nGrowBy = _nSize/8;
					nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
				}
				size_t nNewMax;
				if( nNewSize < (_nMaxSize+nGrowBy) )
					nNewMax = _nMaxSize+nGrowBy;  // granularity
				else
					nNewMax = nNewSize;  // no slush

				FTLASSERT( nNewMax >= _nMaxSize );  // no wrap around
#ifdef SIZE_T_MAX
				FTLASSERT( nNewMax <= SIZE_T_MAX/sizeof( E ) ); // no overflow
#endif
				E* pNewData = static_cast< E* >( calloc( nNewMax,sizeof( E ) ) );
				if( pNewData == NULL )
				{
					return false;
				}

				// copy new data from old
				ETraits::RelocateElements( pNewData, _pData, _nSize );

				// get rid of old stuff (note: no destructors called)
				free( _pData );
				_pData = pNewData;
				_nMaxSize = nNewMax;
			}
		}

		return true;
	}	

	template< typename E, class ETraits >
	bool Array< E, ETraits >::SetCount( size_t nNewSize, int nGrowBy )
	{
		FTLASSERT_VALID(this);

		if( nGrowBy != -1 )
		{
			_nGrowBy = nGrowBy;  // set new size
		}

		if( nNewSize == 0 )
		{
			// shrink to nothing
			if( _pData != NULL )
			{
				CallDestructors( _pData, _nSize );
				free( _pData );
				_pData = NULL;
			}
			_nSize = 0;
			_nMaxSize = 0;
		}
		else if( nNewSize <= _nMaxSize )
		{
			// it fits
			if( nNewSize > _nSize )
			{
				// initialize the new elements
				CallConstructors( _pData+_nSize, nNewSize-_nSize );
			}
			else if( _nSize > nNewSize )
			{
				// destroy the old elements
				CallDestructors( _pData+nNewSize, _nSize-nNewSize );
			}
			_nSize = nNewSize;
		}
		else
		{
			bool bSuccess;

			bSuccess = GrowBuffer( nNewSize );
			if( !bSuccess )
			{
				return( false );
			}

			// construct new elements
			FTLASSERT( nNewSize > _nSize );
			CallConstructors( _pData+_nSize, nNewSize-_nSize );

			_nSize = nNewSize;
		}

		return true;
	}

	template< typename E, class ETraits >
	size_t Array< E, ETraits >::Append( const Array< E, ETraits >& aSrc )
	{
		FTLASSERT_VALID(this);
		FTLASSERT( this != &aSrc );   // cannot append to itself

		size_t nOldSize = _nSize;
		bool bSuccess=SetCount( _nSize+aSrc._nSize );
		if( !bSuccess )
		{
			FtlThrow( E_OUTOFMEMORY );
		}

		ETraits::CopyElements( _pData+nOldSize, aSrc._pData, aSrc._nSize );

		return( nOldSize );
	}

	template< typename E, class ETraits >
	void Array< E, ETraits >::Copy( const Array< E, ETraits >& aSrc )
	{
		FTLASSERT_VALID(this);
		FTLASSERT( this != &aSrc );   // cannot append to itself

		bool bSuccess=SetCount( aSrc._nSize );
		if( !bSuccess )
		{
			FtlThrow( E_OUTOFMEMORY );
		}

		ETraits::CopyElements( _pData, aSrc._pData, aSrc._nSize );
	}

	template< typename E, class ETraits >
	void Array< E, ETraits >::FreeExtra() throw()
	{
		FTLASSERT_VALID(this);

		if( _nSize != _nMaxSize )
		{
			// shrink to desired size
#ifdef SIZE_T_MAX
			FTLASSUME( _nSize <= (SIZE_T_MAX/sizeof( E )) ); // no overflow
#endif
			E* pNewData = NULL;
			if( _nSize != 0 )
			{
				pNewData = (E*)calloc( _nSize,sizeof( E ) );
				if( pNewData == NULL )
				{
					return;
				}

				// copy new data from old
				ETraits::RelocateElements( pNewData, _pData, _nSize );
			}

			// get rid of old stuff (note: no destructors called)
			free( _pData );
			_pData = pNewData;
			_nMaxSize = _nSize;
		}
	}

	template< typename E, class ETraits >
	void Array< E, ETraits >::SetAtGrow( size_t iElement, INARGTYPE element )
	{
		FTLASSERT_VALID(this);
		size_t nOldSize;

		nOldSize = _nSize;
		if( iElement >= _nSize )
		{
			bool bSuccess=SetCount( iElement+1, -1 );
			if( !bSuccess )
			{
				FtlThrow( E_OUTOFMEMORY );
			}
		}

		_FTLTRY
		{
			_pData[iElement] = element;
		}
		_FTLCATCHALL()
		{
			if( _nSize != nOldSize )
			{
				SetCount( nOldSize, -1 );
			}
			_FTLRETHROW;
		}
	}

	template< typename E, class ETraits >
	void Array< E, ETraits >::InsertAt( size_t iElement, INARGTYPE element, size_t nElements /*=1*/)
	{
		FTLASSERT_VALID(this);
		FTLASSERT( nElements > 0 );     // zero size not allowed

		if( iElement >= _nSize )
		{
			// adding after the end of the array
			bool bSuccess=SetCount( iElement+nElements, -1 );   // grow so nIndex is valid
			if( !bSuccess )
			{
				FtlThrow( E_OUTOFMEMORY );
			}
		}
		else
		{
			// inserting in the middle of the array
			size_t nOldSize = _nSize;
			bool bSuccess=SetCount( _nSize+nElements, -1 );  // grow it to new size
			if( !bSuccess )
			{
				FtlThrow( E_OUTOFMEMORY );
			}
			// destroy intial data before copying over it
			CallDestructors( _pData+nOldSize, nElements );
			// shift old data up to fill gap
			ETraits::RelocateElements( _pData+(iElement+nElements), _pData+iElement,
				nOldSize-iElement );

			_FTLTRY
			{
				// re-init slots we copied from
				CallConstructors( _pData+iElement, nElements );
			}
			_FTLCATCHALL()
			{
				ETraits::RelocateElements( _pData+iElement, _pData+(iElement+nElements),
					nOldSize-iElement );
				SetCount( nOldSize, -1 );
				_FTLRETHROW;
			}
		}

		// insert new value in the gap
		FTLASSERT( (iElement+nElements) <= _nSize );
		for( size_t iNewElement = iElement; iNewElement < (iElement+nElements); iNewElement++ )
		{
			_pData[iNewElement] = element;
		}
	}

	template< typename E, class ETraits >
	void Array< E, ETraits >::RemoveAt( size_t iElement, size_t nElements )
	{
		FTLASSERT_VALID(this);
		FTLASSERT( (iElement+nElements) <= _nSize );

		size_t newCount = iElement+nElements;
		if ((newCount < iElement) || (newCount < nElements) || (newCount > _nSize))
			FtlThrow(E_INVALIDARG);		

		// just remove a range
		size_t nMoveCount = _nSize-(newCount);
		CallDestructors( _pData+iElement, nElements );
		if( nMoveCount > 0 )
		{
			ETraits::RelocateElements( _pData+iElement, _pData+(newCount),
				nMoveCount );
		}
		_nSize -= nElements;
	}

	template< typename E, class ETraits >
	void Array< E, ETraits >::InsertArrayAt( size_t iStartElement, 
		const Array< E, ETraits >* paNew )
	{
		FTLASSERT_VALID( this );
		FTLENSURE( paNew != NULL );
		FTLASSERT_VALID( paNew );

		if( paNew->GetCount() > 0 )
		{
			InsertAt( iStartElement, paNew->GetAt( 0 ), paNew->GetCount() );
			for( size_t iElement = 0; iElement < paNew->GetCount(); iElement++ )
			{
				SetAt( iStartElement+iElement, paNew->GetAt( iElement ) );
			}
		}
	}

#ifdef _DEBUG
	template< typename E, class ETraits >
	void Array< E, ETraits >::AssertValid() const
	{
		if( _pData == NULL )
		{
			FTLASSUME( _nSize == 0 );
			FTLASSUME( _nMaxSize == 0 );
		}
		else
		{
			FTLASSUME( _nSize <= _nMaxSize );
			FTLASSERT( FtlIsValidAddress( _pData, _nMaxSize * sizeof( E ) ) );
		}
	}
#endif

#pragma push_macro("new")
#undef new

	template< typename E, class ETraits >
	void Array< E, ETraits >::CallConstructors( E* pElements, size_t nElements )
	{
		size_t iElement = 0;

		_FTLTRY
		{
			for( iElement = 0; iElement < nElements; iElement++ )
			{
				::new( pElements+iElement ) E;
			}
		}
		_FTLCATCHALL()
		{
			while( iElement > 0 )
			{
				iElement--;
				pElements[iElement].~E();
			}

			_FTLRETHROW;
		}
	}

#pragma pop_macro("new")

	template< typename E, class ETraits >
	void Array< E, ETraits >::CallDestructors( E* pElements, size_t nElements ) throw()
	{
		(void)pElements;

		for( size_t iElement = 0; iElement < nElements; iElement++ )
		{
			pElements[iElement].~E();
		}
	}
//=========================== list ========================================================
	template< typename E, class ETraits = ElementTraits< E > >
	class List
	{
	public:
		//typedef typename ETraits::INARGTYPE INARGTYPE;
        typedef const E& INARGTYPE;

	private:
		class Node :
			public __POSITION
		{
		public:
            Node()
			{
			}
			Node( INARGTYPE element) :
			_element( element )
			{
			}
			~Node() throw()
			{
			}

		public:
			Node* _pNext;
			Node* _pPrev;
			E _element;

            inline Node* NextNode( )
            {
                return (_pNext );
            }
            inline Node* PrevNode()
            {
                return ( _pPrev );
            }
		private:
			Node( const Node& ) throw();
		};

    public:

        //---   iterator operations ----
        template<typename Node_T>
        class iterator_t
        {
            Node_T      *node;

        public:
            iterator_t(Node_T* p=NULL):node(p){}

            E* operator->() {
                return &node->_element;
            }
            E& operator*() {
                return node->_element;
            }
            iterator_t& operator++() {
                if( node == NULL) return *this;
                Node* pNext = node->NextNode();
                node = (Node_T*) pNext ;
                return *this;
            }
            iterator_t operator++(int) {
                iterator_t tmp(*this);
                operator++();
                return tmp;
            }
            iterator_t& operator--() {
                if( node == NULL) return *this;
                Node* pNext = node->PrevNode();
                node = (Node_T*) pNext ;
                return *this;
            }
            iterator_t operator--(int) {
                iterator_t tmp(*this);
                operator--();
                return tmp;
            }
            bool operator==(const iterator_t& it) {
                return node == it.node;
            }
            bool operator!=(const iterator_t& it) {
                return !operator==(it);
            }
        };
        typedef iterator_t<Node> iterator;
        typedef iterator_t<const Node> const_iterator;

        inline void push_back(INARGTYPE v) {
            AddTail(v);
        }
        inline void push_front(INARGTYPE v) {
            AddHead(v);
        }
        inline void insert(iterator it, INARGTYPE v) {
            InsertBefore(it.node, v);
        }
        iterator begin(){
            return iterator(_pHead);
        }
        iterator end() {
            return iterator();
        }
        inline iterator erase(INARGTYPE key) {
            Node *p = Find(key, NULL);
            if(p==NULL) return iterator();
            Node *next = p->NextNode();
            RemoveAt(p);
            return iterator(next, this);
        }
        template <typename iterator_T>
        inline iterator erase(iterator_T it) {
            iterator_T itNext = it;
            itNext++;
            RemoveAt(it->node);
            return itNext;
        }
        const_iterator cbegin(){
            return const_iterator(_pHead, this);
        }
        const_iterator cend(){
            return const_iterator(NULL, this);
        }
        iterator find(INARGTYPE key){
            return iterator(Find(key, NULL), this);
        }
        const_iterator find(INARGTYPE key) const {
            return const_iterator(Find(key, NULL), this);
        }
        size_t size()const{
            return GetCount();
        }

        void clear() {
            RemoveAll();
        }
        bool empty()const{
            return IsEmpty();
        }
	public:
		List( UINT nBlockSize = 10 ) throw();

		size_t GetCount() const throw();
		bool IsEmpty() const throw();

		E& GetHead();
		const E& GetHead() const;
		E& GetTail();
		const E& GetTail() const;

		E RemoveHead();
		E RemoveTail();
		void RemoveHeadNoReturn() throw();
		void RemoveTailNoReturn() throw();

		POSITION AddHead();
		POSITION AddHead( INARGTYPE element );
		void AddHeadList( const List< E, ETraits >* plNew );
		POSITION AddTail();
		POSITION AddTail( INARGTYPE element );
		void AddTailList( const List< E, ETraits >* plNew );

		void RemoveAll() throw();

		POSITION GetHeadPosition() const throw();
		POSITION GetTailPosition() const throw();
		E& GetNext( POSITION& pos );
		const E& GetNext( POSITION& pos ) const;
		E& GetPrev( POSITION& pos );
		const E& GetPrev( POSITION& pos ) const throw();

		E& GetAt( POSITION pos );
		const E& GetAt( POSITION pos ) const;
		void SetAt( POSITION pos, INARGTYPE element );
		void RemoveAt( POSITION pos ) throw();

		POSITION InsertBefore( POSITION pos, INARGTYPE element );
		POSITION InsertAfter( POSITION pos, INARGTYPE element );

		POSITION Find( INARGTYPE element, POSITION posStartAfter = NULL ) const throw();
		POSITION FindIndex( size_t iElement ) const throw();

		void MoveToHead( POSITION pos );
		void MoveToTail( POSITION pos );
		void SwapElements( POSITION pos1, POSITION pos2 ) throw();

#ifdef _DEBUG
		void AssertValid() const;
#endif  // _DEBUG

		// Implementation
	private:
		Node* _pHead;
		Node* _pTail;
		size_t _nElements;
		Plex* _pBlocks;
		Node* _pFree;
		UINT _nBlockSize;

	private:
		void GetFreeNode();
		Node* NewNode( Node* pPrev, Node* pNext );
		Node* NewNode( INARGTYPE element, Node* pPrev, Node* pNext );
		void FreeNode( Node* pNode ) throw();

	public:
		~List() throw();

	private:
		// Private to prevent use
		List( const List& ) throw();
		List& operator=( const List& ) throw();
	}; // class list
	template< typename E, class ETraits >
	inline size_t List< E, ETraits >::GetCount() const throw()
	{
		return( _nElements );
	}

	template< typename E, class ETraits >
	inline bool List< E, ETraits >::IsEmpty() const throw()
	{
		return( _nElements == 0 );
	}

	template< typename E, class ETraits >
	inline E& List< E, ETraits >::GetHead()
	{
		FTLENSURE( _pHead != NULL );
		return( _pHead->_element );
	}

	template< typename E, class ETraits >
	inline const E& List< E, ETraits >::GetHead() const
	{
		FTLENSURE( _pHead != NULL );
		return( _pHead->_element );
	}

	template< typename E, class ETraits >
	inline E& List< E, ETraits >::GetTail()
	{
		FTLENSURE( _pTail != NULL );
		return( _pTail->_element );
	}

	template< typename E, class ETraits >
	inline const E& List< E, ETraits >::GetTail() const
	{
		FTLENSURE( _pTail != NULL );
		return( _pTail->_element );
	}

	template< typename E, class ETraits >
	inline POSITION List< E, ETraits >::GetHeadPosition() const throw()
	{
		return( POSITION( _pHead ) );
	}

	template< typename E, class ETraits >
	inline POSITION List< E, ETraits >::GetTailPosition() const throw()
	{
		return( POSITION( _pTail ) );
	}

	template< typename E, class ETraits >
	inline E& List< E, ETraits >::GetNext( POSITION& pos )
	{
		Node* pNode;

		FTLENSURE( pos != NULL );
		pNode = (Node*)pos;
		pos = POSITION( pNode->_pNext );

		return( pNode->_element );
	}

	template< typename E, class ETraits >
	inline const E& List< E, ETraits >::GetNext( POSITION& pos ) const
	{
		Node* pNode;

		FTLENSURE( pos != NULL );
		pNode = (Node*)pos;
		pos = POSITION( pNode->_pNext );

		return( pNode->_element );
	}

	template< typename E, class ETraits >
	inline E& List< E, ETraits >::GetPrev( POSITION& pos )
	{
		Node* pNode;

		FTLENSURE( pos != NULL );
		pNode = (Node*)pos;
		pos = POSITION( pNode->_pPrev );

		return( pNode->_element );
	}

	template< typename E, class ETraits >
	inline const E& List< E, ETraits >::GetPrev( POSITION& pos ) const throw()
	{
		Node* pNode;

		FTLASSERT( pos != NULL );
		pNode = (Node*)pos;
		pos = POSITION( pNode->_pPrev );

		return( pNode->_element );
	}

	template< typename E, class ETraits >
	inline E& List< E, ETraits >::GetAt( POSITION pos )
	{
		FTLENSURE( pos != NULL );
		Node* pNode = (Node*)pos;
		return( pNode->_element );
	}

	template< typename E, class ETraits >
	inline const E& List< E, ETraits >::GetAt( POSITION pos ) const 
	{
		FTLENSURE( pos != NULL );
		Node* pNode = (Node*)pos;
		return( pNode->_element );
	}

	template< typename E, class ETraits >
	inline void List< E, ETraits >::SetAt( POSITION pos, INARGTYPE element )
	{
		FTLENSURE( pos != NULL );
		Node* pNode = (Node*)pos;
		pNode->_element = element;
	}

	template< typename E, class ETraits >
	List< E, ETraits >::List( UINT nBlockSize ) throw() :
	_nElements( 0 ),
		_pHead( NULL ),
		_pTail( NULL ),
		_nBlockSize( nBlockSize ),
		_pBlocks( NULL ),
		_pFree( NULL )
	{
		FTLASSERT( nBlockSize > 0 );
	}

	template< typename E, class ETraits >
    void List< E, ETraits >::RemoveAll() throw()
	{
		while( _nElements > 0 )
		{
			Node* pKill = _pHead;
			FTLENSURE( pKill != NULL );

			_pHead = _pHead->_pNext;
			FreeNode( pKill );
		}

		FTLASSUME( _nElements == 0 );
		_pHead = NULL;
		_pTail = NULL;
		_pFree = NULL;

		if( _pBlocks != NULL )
		{
			_pBlocks->FreeDataChain();
			_pBlocks = NULL;
		}
	}

	template< typename E, class ETraits >
	List< E, ETraits >::~List() throw()
	{
		RemoveAll();
		FTLASSUME( _nElements == 0 );
	}

#pragma push_macro("new")
#undef new

	template< typename E, class ETraits >
	void List< E, ETraits >::GetFreeNode()
	{
		if( _pFree == NULL )
		{
			Plex* pPlex;
			Node* pNode;

			pPlex = Plex::Create( _pBlocks, _nBlockSize, sizeof( Node ) );
			if( pPlex == NULL )
			{
				FtlThrow( E_OUTOFMEMORY );
			}
			pNode = (Node*)pPlex->data();
			pNode += _nBlockSize-1;
			for( int iBlock = _nBlockSize-1; iBlock >= 0; iBlock-- )
			{
				pNode->_pNext = _pFree;
				_pFree = pNode;
				pNode--;
			}
		}
		FTLASSUME( _pFree != NULL );
	}

	template< typename E, class ETraits >
	typename List< E, ETraits >::Node* List< E, ETraits >::NewNode( Node* pPrev, Node* pNext )
	{
		GetFreeNode();

		Node* pNewNode = _pFree;
		Node* pNextFree = _pFree->_pNext;

		::new( pNewNode ) Node;

		_pFree = pNextFree;
		pNewNode->_pPrev = pPrev;
		pNewNode->_pNext = pNext;
		_nElements++;
		FTLASSUME( _nElements > 0 );

		return( pNewNode );
	}

	template< typename E, class ETraits >
	typename List< E, ETraits >::Node* List< E, ETraits >::NewNode( INARGTYPE element, Node* pPrev, 
		Node* pNext )
	{
		GetFreeNode();

		Node* pNewNode = _pFree;
		Node* pNextFree = _pFree->_pNext;

		::new( pNewNode ) Node( element );

		_pFree = pNextFree;
		pNewNode->_pPrev = pPrev;
		pNewNode->_pNext = pNext;
		_nElements++;
		FTLASSUME( _nElements > 0 );

		return( pNewNode );
	}

#pragma pop_macro("new")

	template< typename E, class ETraits >
	void List< E, ETraits >::FreeNode( Node* pNode ) throw()
	{
		pNode->~Node();
		pNode->_pNext = _pFree;
		_pFree = pNode;
		FTLASSUME( _nElements > 0 );
		_nElements--;
		if( _nElements == 0 )
		{
			RemoveAll();
		}
	}

	template< typename E, class ETraits >
	POSITION List< E, ETraits >::AddHead()
	{
		Node* pNode = NewNode( NULL, _pHead );
		if( _pHead != NULL )
		{
			_pHead->_pPrev = pNode;
		}
		else
		{
			_pTail = pNode;
		}
		_pHead = pNode;

		return( POSITION( pNode ) );
	}

	template< typename E, class ETraits >
	POSITION List< E, ETraits >::AddHead( INARGTYPE element )
	{
		Node* pNode;

		pNode = NewNode( element, NULL, _pHead );

		if( _pHead != NULL )
		{
			_pHead->_pPrev = pNode;
		}
		else
		{
			_pTail = pNode;
		}
		_pHead = pNode;

		return( POSITION( pNode ) );
	}

	template< typename E, class ETraits >
	POSITION List< E, ETraits >::AddTail()
	{
		Node* pNode = NewNode( _pTail, NULL );
		if( _pTail != NULL )
		{
			_pTail->_pNext = pNode;
		}
		else
		{
			_pHead = pNode;
		}
		_pTail = pNode;

		return( POSITION( pNode ) );
	}

	template< typename E, class ETraits >
	POSITION List< E, ETraits >::AddTail( INARGTYPE element )
	{
		Node* pNode;

		pNode = NewNode( element, _pTail, NULL );

		if( _pTail != NULL )
		{
			_pTail->_pNext = pNode;
		}
		else
		{
			_pHead = pNode;
		}
		_pTail = pNode;

		return( POSITION( pNode ) );
	}

	template< typename E, class ETraits >
	void List< E, ETraits >::AddHeadList( const List< E, ETraits >* plNew )
	{
		FTLENSURE( plNew != NULL );

		POSITION pos = plNew->GetTailPosition();
		while( pos != NULL )
		{
			INARGTYPE element = plNew->GetPrev( pos );
			AddHead( element );
		}
	}

	template< typename E, class ETraits >
	void List< E, ETraits >::AddTailList( const List< E, ETraits >* plNew )
	{
		FTLENSURE( plNew != NULL );

		POSITION pos = plNew->GetHeadPosition();
		while( pos != NULL )
		{
			INARGTYPE element = plNew->GetNext( pos );
			AddTail( element );
		}
	}

	template< typename E, class ETraits >
	E List< E, ETraits >::RemoveHead()
	{
		FTLENSURE( _pHead != NULL );

		Node* pNode = _pHead;
		E element( pNode->_element );

		_pHead = pNode->_pNext;
		if( _pHead != NULL )
		{
			_pHead->_pPrev = NULL;
		}
		else
		{
			_pTail = NULL;
		}
		FreeNode( pNode );

		return( element );
	}

	template< typename E, class ETraits >
    void List< E, ETraits >::RemoveHeadNoReturn() throw()
	{
		FTLENSURE( _pHead != NULL );

		Node* pNode = _pHead;

		_pHead = pNode->_pNext;
		if( _pHead != NULL )
		{
			_pHead->_pPrev = NULL;
		}
		else
		{
			_pTail = NULL;
		}
		FreeNode( pNode );
	}

	template< typename E, class ETraits >
	E List< E, ETraits >::RemoveTail()
	{
		FTLENSURE( _pTail != NULL );

		Node* pNode = _pTail;

		E element( pNode->_element );

		_pTail = pNode->_pPrev;
		if( _pTail != NULL )
		{
			_pTail->_pNext = NULL;
		}
		else
		{
			_pHead = NULL;
		}
		FreeNode( pNode );

		return( element );
	}

	template< typename E, class ETraits >
    void List< E, ETraits >::RemoveTailNoReturn() throw()
	{
		FTLENSURE( _pTail != NULL );

		Node* pNode = _pTail;

		_pTail = pNode->_pPrev;
		if( _pTail != NULL )
		{
			_pTail->_pNext = NULL;
		}
		else
		{
			_pHead = NULL;
		}
		FreeNode( pNode );
	}

	template< typename E, class ETraits >
	POSITION List< E, ETraits >::InsertBefore( POSITION pos, INARGTYPE element )
	{
		FTLASSERT_VALID(this);

		if( pos == NULL )
			return AddHead( element ); // insert before nothing -> head of the list

		// Insert it before position
		Node* pOldNode = (Node*)pos;
		Node* pNewNode = NewNode( element, pOldNode->_pPrev, pOldNode );

		if( pOldNode->_pPrev != NULL )
		{
			FTLASSERT(FtlIsValidAddress(pOldNode->_pPrev, sizeof(Node)));
			pOldNode->_pPrev->_pNext = pNewNode;
		}
		else
		{
			FTLASSERT( pOldNode == _pHead );
			_pHead = pNewNode;
		}
		pOldNode->_pPrev = pNewNode;

		return( POSITION( pNewNode ) );
	}

	template< typename E, class ETraits >
	POSITION List< E, ETraits >::InsertAfter( POSITION pos, INARGTYPE element )
	{
		FTLASSERT_VALID(this);

		if( pos == NULL )
			return AddTail( element ); // insert after nothing -> tail of the list

		// Insert it after position
		Node* pOldNode = (Node*)pos;
		Node* pNewNode = NewNode( element, pOldNode, pOldNode->_pNext );

		if( pOldNode->_pNext != NULL )
		{
			FTLASSERT(FtlIsValidAddress(pOldNode->_pNext, sizeof(Node)));
			pOldNode->_pNext->_pPrev = pNewNode;
		}
		else
		{
			FTLASSERT( pOldNode == _pTail );
			_pTail = pNewNode;
		}
		pOldNode->_pNext = pNewNode;

		return( POSITION( pNewNode ) );
	}

	template< typename E, class ETraits >
    void List< E, ETraits >::RemoveAt( POSITION pos ) throw()
	{
		FTLASSERT_VALID(this);
		FTLENSURE( pos != NULL );

		Node* pOldNode = (Node*)pos;

		// remove pOldNode from list
		if( pOldNode == _pHead )
		{
			_pHead = pOldNode->_pNext;
		}
		else
		{
			FTLASSERT( FtlIsValidAddress( pOldNode->_pPrev, sizeof(Node) ));
			pOldNode->_pPrev->_pNext = pOldNode->_pNext;
		}
		if( pOldNode == _pTail )
		{
			_pTail = pOldNode->_pPrev;
		}
		else
		{
			FTLASSERT( FtlIsValidAddress( pOldNode->_pNext, sizeof(Node) ));
			pOldNode->_pNext->_pPrev = pOldNode->_pPrev;
		}
		FreeNode( pOldNode );
	}

	template< typename E, class ETraits >
	POSITION List< E, ETraits >::FindIndex( size_t iElement ) const throw()
	{
		FTLASSERT_VALID(this);

		if( iElement >= _nElements )
			return NULL;  // went too far

		if(_pHead == NULL)
			return NULL;

		Node* pNode = _pHead;
		for( size_t iSearch = 0; iSearch < iElement; iSearch++ )
		{
			pNode = pNode->_pNext;
		}

		return( POSITION( pNode ) );
	}

	template< typename E, class ETraits >
	void List< E, ETraits >::MoveToHead( POSITION pos )
	{
		FTLENSURE( pos != NULL );

		Node* pNode = static_cast< Node* >( pos );

		if( pNode == _pHead )
		{
			// Already at the head
			return;
		}

		if( pNode->_pNext == NULL )
		{
			FTLASSERT( pNode == _pTail );
			_pTail = pNode->_pPrev;
		}
		else
		{
			pNode->_pNext->_pPrev = pNode->_pPrev;
		}

		FTLASSERT( pNode->_pPrev != NULL );  // This node can't be the head, since we already checked that case
		pNode->_pPrev->_pNext = pNode->_pNext;

		_pHead->_pPrev = pNode;
		pNode->_pNext = _pHead;
		pNode->_pPrev = NULL;
		_pHead = pNode;
	}

	template< typename E, class ETraits >
	void List< E, ETraits >::MoveToTail( POSITION pos )
	{
		FTLENSURE( pos != NULL );
		Node* pNode = static_cast< Node* >( pos );

		if( pNode == _pTail )
		{
			// Already at the tail
			return;
		}

		if( pNode->_pPrev == NULL )
		{
			FTLENSURE( pNode == _pHead );
			_pHead = pNode->_pNext;
		}
		else
		{
			pNode->_pPrev->_pNext = pNode->_pNext;
		}

		pNode->_pNext->_pPrev = pNode->_pPrev;

		_pTail->_pNext = pNode;
		pNode->_pPrev = _pTail;
		pNode->_pNext = NULL;
		_pTail = pNode;
	}

	template< typename E, class ETraits >
	void List< E, ETraits >::SwapElements( POSITION pos1, POSITION pos2 ) throw()
	{
		FTLASSERT( pos1 != NULL );
		FTLASSERT( pos2 != NULL );

		if( pos1 == pos2 )
		{
			// Nothing to do
			return;
		}

		Node* pNode1 = static_cast< Node* >( pos1 );
		Node* pNode2 = static_cast< Node* >( pos2 );
		if( pNode2->_pNext == pNode1 )
		{
			// Swap pNode2 and pNode1 so that the next case works
			Node* pNodeTemp = pNode1;
			pNode1 = pNode2;
			pNode2 = pNodeTemp;
		}
		if( pNode1->_pNext == pNode2 )
		{
			// Node1 and Node2 are adjacent
			pNode2->_pPrev = pNode1->_pPrev;
			if( pNode1->_pPrev != NULL )
			{
				pNode1->_pPrev->_pNext = pNode2;
			}
			else
			{
				FTLASSUME( _pHead == pNode1 );
				_pHead = pNode2;
			}
			pNode1->_pNext = pNode2->_pNext;
			if( pNode2->_pNext != NULL )
			{
				pNode2->_pNext->_pPrev = pNode1;
			}
			else
			{
				FTLASSUME( _pTail == pNode2 );
				_pTail = pNode1;
			}
			pNode2->_pNext = pNode1;
			pNode1->_pPrev = pNode2;
		}
		else
		{
			// The two nodes are not adjacent
			Node* pNodeTemp;

			pNodeTemp = pNode1->_pPrev;
			pNode1->_pPrev = pNode2->_pPrev;
			pNode2->_pPrev = pNodeTemp;

			pNodeTemp = pNode1->_pNext;
			pNode1->_pNext = pNode2->_pNext;
			pNode2->_pNext = pNodeTemp;

			if( pNode1->_pNext != NULL )
			{
				pNode1->_pNext->_pPrev = pNode1;
			}
			else
			{
				FTLASSUME( _pTail == pNode2 );
				_pTail = pNode1;
			}
			if( pNode1->_pPrev != NULL )
			{
				pNode1->_pPrev->_pNext = pNode1;
			}
			else
			{
				FTLASSUME( _pHead == pNode2 );
				_pHead = pNode1;
			}
			if( pNode2->_pNext != NULL )
			{
				pNode2->_pNext->_pPrev = pNode2;
			}
			else
			{
				FTLASSUME( _pTail == pNode1 );
				_pTail = pNode2;
			}
			if( pNode2->_pPrev != NULL )
			{
				pNode2->_pPrev->_pNext = pNode2;
			}
			else
			{
				FTLASSUME( _pHead == pNode1 );
				_pHead = pNode2;
			}
		}
	}

	template< typename E, class ETraits >
	POSITION List< E, ETraits >::Find( INARGTYPE element, POSITION posStartAfter ) const throw()
	{
		FTLASSERT_VALID(this);

		Node* pNode = (Node*)posStartAfter;
		if( pNode == NULL )
		{
			pNode = _pHead;  // start at head
		}
		else
		{
			FTLASSERT(FtlIsValidAddress(pNode, sizeof(Node)));
			pNode = pNode->_pNext;  // start after the one specified
		}

		for( ; pNode != NULL; pNode = pNode->_pNext )
		{
			if( ETraits::CompareElements( pNode->_element, element ) )
				return( POSITION( pNode ) );
		}

		return( NULL );
	}

#ifdef _DEBUG
	template< typename E, class ETraits >
	void List< E, ETraits >::AssertValid() const
	{
		if( IsEmpty() )
		{
			// empty list
			FTLASSUME(_pHead == NULL);
			FTLASSUME(_pTail == NULL);
		}
		else
		{
			// non-empty list
			FTLASSERT(FtlIsValidAddress(_pHead, sizeof(Node)));
			FTLASSERT(FtlIsValidAddress(_pTail, sizeof(Node)));
		}
	}
#endif


//=========================== map ========================================================
template< typename K, typename V, class KTraits = ElementTraits< K >, class VTraits = ElementTraits< V > >
class Map
{
public:
    typedef const K& KINARGTYPE;
    typedef K&       KOUTARGTYPE;
    typedef const V& VINARGTYPE;
    typedef V&       VOUTARGTYPE;
	//typedef typename KTraits::INARGTYPE KINARGTYPE;
	//typedef typename KTraits::OUTARGTYPE KOUTARGTYPE;
	//typedef typename VTraits::INARGTYPE VINARGTYPE;
	//typedef typename VTraits::OUTARGTYPE VOUTARGTYPE;

	class Pair :
		public __POSITION
	{
	protected:
		Pair( KINARGTYPE key ) :
			 _key( key )
                 , first(key), second(_value)
			 {
			 }

	public:
        Pair( KINARGTYPE key, VINARGTYPE v ) :
          _key(key), _value(v)
              , first(key), second(_value) {}

		const K _key;
		V _value;
        const K   first;
        V&        second;
	};

    //---   iterator operations ----
    template<typename Pair_t>
    class iterator_t
    {
        Pair_t     *pair;
        Map        *pMap;
    public:
        iterator_t(Pair_t* p, Map* m):pair(p), pMap(m){}

        Pair_t* operator->() {
            return pair;
        }
        Pair_t& operator*() {
            return *pair;
        }
        iterator_t& operator++() {
            FTLASSUME( _ppBins != NULL );
            FTLASSERT( pos != NULL );
            if( pair == NULL) return *this;

            Node* pNode = static_cast< Node* >( pair );
            Node* pNext = pMap->FindNextNode( pNode );

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
    typedef iterator_t<Pair> iterator;
    typedef iterator_t<const Pair> const_iterator;

    inline void insert(const Pair& p) {
        SetAt(p.first, p.second);
    }
    iterator begin(){
        return iterator(static_cast<Pair*>(GetStartPosition()), this);
    }
    iterator end() {
        return iterator(NULL, this);
    }
    inline iterator erase(KINARGTYPE key) {
        Node *p = Lookup(key);
        if(p==NULL) return iterator(NULL, this);
        Node *next = FindNextNode(p);
        RemoveNode(p, NULL);
        return iterator(next, this);
    }
    template <typename iterator_T>
    inline iterator erase(iterator_T it) {
        iterator_T itNext = it;
        itNext++;
        RemoveAtPos(it->first);
        return itNext;
    }
    const_iterator cbegin(){
        return const_iterator(static_cast<const Pair*>(GetStartPosition()), this);
    }
    const_iterator cend(){
        return const_iterator(NULL, this);
    }
    iterator find(KINARGTYPE key){
        return iterator(Lookup(key), this);
    }
    const_iterator find(KINARGTYPE key) const {
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

private:
	class Node :
		public Pair
	{
	public:
		Node( KINARGTYPE key, UINT nHash ) :
		  Pair( key ),
			  _nHash( nHash )
		  {
		  }

	public:
		UINT GetHash() const throw()
		{
			return( _nHash );
		}

	public:
		Node* _pNext;
		UINT _nHash;
	};

public:
	Map( UINT nBins = 17, float fOptimalLoad = 0.75f, 
		float fLoThreshold = 0.25f, float fHiThreshold = 2.25f, UINT nBlockSize = 10 ) throw();

	size_t GetCount() const throw();
	bool IsEmpty() const throw();

	bool Lookup( KINARGTYPE key, VOUTARGTYPE value ) const;
	const Pair* Lookup( KINARGTYPE key ) const throw();
	Pair* Lookup( KINARGTYPE key ) throw();
    V& operator[]( KINARGTYPE key ) ;

	POSITION SetAt( KINARGTYPE key, VINARGTYPE value );
	void SetValueAt( POSITION pos, VINARGTYPE value );

	bool RemoveKey( KINARGTYPE key ) throw();
	void RemoveAll();
	void RemoveAtPos( POSITION pos ) throw();

	POSITION GetStartPosition() const throw();
	void GetNextAssoc( POSITION& pos, KOUTARGTYPE key, VOUTARGTYPE value ) const;
	const Pair* GetNext( POSITION& pos ) const throw();
	Pair* GetNext( POSITION& pos ) throw();
	const K& GetNextKey( POSITION& pos ) const;
	const V& GetNextValue( POSITION& pos ) const;
	V& GetNextValue( POSITION& pos );
	void GetAt( POSITION pos, KOUTARGTYPE key, VOUTARGTYPE value ) const;
	Pair* GetAt( POSITION pos ) throw();
	const Pair* GetAt( POSITION pos ) const throw();
	const K& GetKeyAt( POSITION pos ) const;
	const V& GetValueAt( POSITION pos ) const;
	V& GetValueAt( POSITION pos );

	UINT GetHashTableSize() const throw();
	bool InitHashTable( UINT nBins, bool bAllocNow = true );
	void EnableAutoRehash() throw();
	void DisableAutoRehash() throw();
	void Rehash( UINT nBins = 0 );
	void SetOptimalLoad( float fOptimalLoad, float fLoThreshold, float fHiThreshold, 
		bool bRehashNow = false );

#ifdef _DEBUG
	void AssertValid() const;
#endif  // _DEBUG

	// Implementation
private:
	Node** _ppBins;
	size_t _nElements;
	UINT _nBins;
	float _fOptimalLoad;
	float _fLoThreshold;
	float _fHiThreshold;
	size_t _nHiRehashThreshold;
	size_t _nLoRehashThreshold;
	ULONG _nLockCount;
	UINT _nBlockSize;
	Plex* _pBlocks;
	Node* _pFree;

private:
	bool IsLocked() const throw();
	UINT PickSize( size_t nElements ) const throw();
	Node* NewNode( KINARGTYPE key, UINT iBin, UINT nHash );
	void FreeNode( Node* pNode );
	void FreePlexes() throw();
	Node* GetNode( KINARGTYPE key, UINT& iBin, UINT& nHash, Node*& pPrev ) const throw();
    Node* CreateNode( KINARGTYPE key, UINT iBin, UINT nHash ) ;
	void RemoveNode( Node* pNode, Node* pPrev ) throw();
	Node* FindNextNode( Node* pNode ) const throw();
	void UpdateRehashThresholds() throw();

public:
	~Map() throw();

private:
	// Private to prevent use
	Map( const Map& ) throw();
	Map& operator=( const Map& ) throw();
};

template< typename K, typename V, class KTraits, class VTraits >
inline size_t Map< K, V, KTraits, VTraits >::GetCount() const throw()
{
	return( _nElements );
}

template< typename K, typename V, class KTraits, class VTraits >
inline bool Map< K, V, KTraits, VTraits >::IsEmpty() const throw()
{
	return( _nElements == 0 );
}

template< typename K, typename V, class KTraits, class VTraits >
inline V& Map< K, V, KTraits, VTraits >::operator[]( KINARGTYPE key )
{
	Node* pNode;
	UINT iBin;
	UINT nHash;
	Node* pPrev;

	pNode = GetNode( key, iBin, nHash, pPrev );
	if( pNode == NULL )
	{
		pNode = CreateNode( key, iBin, nHash );
	}

	return( pNode->_value );
}

template< typename K, typename V, class KTraits, class VTraits >
inline UINT Map< K, V, KTraits, VTraits >::GetHashTableSize() const throw()
{
	return( _nBins );
}

template< typename K, typename V, class KTraits, class VTraits >
inline void Map< K, V, KTraits, VTraits >::GetAt( POSITION pos, KOUTARGTYPE key, VOUTARGTYPE value ) const
{
	FTLENSURE( pos != NULL );

	Node* pNode = static_cast< Node* >( pos );

	key = pNode->_key;
	value = pNode->_value;
}

template< typename K, typename V, class KTraits, class VTraits >
inline typename Map< K, V, KTraits, VTraits >::Pair* Map< K, V, KTraits, VTraits >::GetAt( POSITION pos ) throw()
{
	FTLASSERT( pos != NULL );

	return( static_cast< Pair* >( pos ) );
}

template< typename K, typename V, class KTraits, class VTraits >
inline const typename Map< K, V, KTraits, VTraits >::Pair* Map< K, V, KTraits, VTraits >::GetAt( POSITION pos ) const throw()
{
	FTLASSERT( pos != NULL );

	return( static_cast< const Pair* >( pos ) );
}

template< typename K, typename V, class KTraits, class VTraits >
inline const K& Map< K, V, KTraits, VTraits >::GetKeyAt( POSITION pos ) const
{
	FTLENSURE( pos != NULL );

	Node* pNode = (Node*)pos;

	return( pNode->_key );
}

template< typename K, typename V, class KTraits, class VTraits >
inline const V& Map< K, V, KTraits, VTraits >::GetValueAt( POSITION pos ) const
{
	FTLENSURE( pos != NULL );

	Node* pNode = (Node*)pos;

	return( pNode->_value );
}

template< typename K, typename V, class KTraits, class VTraits >
inline V& Map< K, V, KTraits, VTraits >::GetValueAt( POSITION pos )
{
	FTLENSURE( pos != NULL );

	Node* pNode = (Node*)pos;

	return( pNode->_value );
}

template< typename K, typename V, class KTraits, class VTraits >
inline void Map< K, V, KTraits, VTraits >::DisableAutoRehash() throw()
{
	_nLockCount++;
}

template< typename K, typename V, class KTraits, class VTraits >
inline void Map< K, V, KTraits, VTraits >::EnableAutoRehash() throw()
{
	FTLASSUME( _nLockCount > 0 );
	_nLockCount--;
}

template< typename K, typename V, class KTraits, class VTraits >
inline bool Map< K, V, KTraits, VTraits >::IsLocked() const throw()
{
	return( _nLockCount != 0 );
}

template< typename K, typename V, class KTraits, class VTraits >
UINT Map< K, V, KTraits, VTraits >::PickSize( size_t nElements ) const throw()
{
	// List of primes such that s_anPrimes[i] is the smallest prime greater than 2^(5+i/3)
	static const UINT s_anPrimes[] =
	{
		17, 23, 29, 37, 41, 53, 67, 83, 103, 131, 163, 211, 257, 331, 409, 521, 647, 821, 
		1031, 1291, 1627, 2053, 2591, 3251, 4099, 5167, 6521, 8209, 10331, 
		13007, 16411, 20663, 26017, 32771, 41299, 52021, 65537, 82571, 104033, 
		131101, 165161, 208067, 262147, 330287, 416147, 524309, 660563, 
		832291, 1048583, 1321139, 1664543, 2097169, 2642257, 3329023, 4194319, 
		5284493, 6658049, 8388617, 10568993, 13316089, UINT_MAX
	};

	size_t nBins = (size_t)(nElements/_fOptimalLoad);
	UINT nBinsEstimate = UINT(  UINT_MAX < nBins ? UINT_MAX : nBins );

	// Find the smallest prime greater than our estimate
	int iPrime = 0;
	while( nBinsEstimate > s_anPrimes[iPrime] )
	{
		iPrime++;
	}

	if( s_anPrimes[iPrime] == UINT_MAX )
	{
		return( nBinsEstimate );
	}
	else
	{
		return( s_anPrimes[iPrime] );
	}
}

template< typename K, typename V, class KTraits, class VTraits >
typename Map< K, V, KTraits, VTraits >::Node* Map< K, V, KTraits, VTraits >::CreateNode( 
    KINARGTYPE key, UINT iBin, UINT nHash )
{
	Node* pNode;

	if( _ppBins == NULL )
	{
		bool bSuccess;

		bSuccess = InitHashTable( _nBins );
		if( !bSuccess )
		{
			FtlThrow( E_OUTOFMEMORY );
		}
	}

	pNode = NewNode( key, iBin, nHash );

	return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION Map< K, V, KTraits, VTraits >::GetStartPosition() const throw()
{
	if( IsEmpty() )
	{
		return( NULL );
	}

	for( UINT iBin = 0; iBin < _nBins; iBin++ )
	{
		if( _ppBins[iBin] != NULL )
		{
			return( POSITION( _ppBins[iBin] ) );
		}
	}
	FTLASSERT( false );

	return( NULL );
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION Map< K, V, KTraits, VTraits >::SetAt( KINARGTYPE key, VINARGTYPE value )
{
	Node* pNode;
	UINT iBin;
	UINT nHash;
	Node* pPrev;

	pNode = GetNode( key, iBin, nHash, pPrev );
	if( pNode == NULL )
	{
		pNode = CreateNode( key, iBin, nHash );
		_FTLTRY
		{
			pNode->_value = value;
		}
		_FTLCATCHALL()
		{
			RemoveAtPos( POSITION( pNode ) );
			_FTLRETHROW;
		}
	}
	else
	{
		pNode->_value = value;
	}

	return( POSITION( pNode ) );
}

template< typename K, typename V, class KTraits, class VTraits >
void Map< K, V, KTraits, VTraits >::SetValueAt( POSITION pos, VINARGTYPE value )
{
	FTLASSERT( pos != NULL );

	Node* pNode = static_cast< Node* >( pos );

	pNode->_value = value;
}

template< typename K, typename V, class KTraits, class VTraits >
Map< K, V, KTraits, VTraits >::Map( UINT nBins, float fOptimalLoad, 
										   float fLoThreshold, float fHiThreshold, UINT nBlockSize ) throw() :
_ppBins( NULL ),
_nBins( nBins ),
_nElements( 0 ),
_nLockCount( 0 ),  // Start unlocked
_fOptimalLoad( fOptimalLoad ),
_fLoThreshold( fLoThreshold ),
_fHiThreshold( fHiThreshold ),
_nHiRehashThreshold( UINT_MAX ),
_nLoRehashThreshold( 0 ),
_pBlocks( NULL ),
_pFree( NULL ),
_nBlockSize( nBlockSize )
{
	FTLASSERT( nBins > 0 );
	FTLASSERT( nBlockSize > 0 );

	SetOptimalLoad( fOptimalLoad, fLoThreshold, fHiThreshold, false );
}

template< typename K, typename V, class KTraits, class VTraits >
void Map< K, V, KTraits, VTraits >::SetOptimalLoad( float fOptimalLoad, float fLoThreshold,
													   float fHiThreshold, bool bRehashNow )
{
	FTLASSERT( fOptimalLoad > 0 );
	FTLASSERT( (fLoThreshold >= 0) && (fLoThreshold < fOptimalLoad) );
	FTLASSERT( fHiThreshold > fOptimalLoad );

	_fOptimalLoad = fOptimalLoad;
	_fLoThreshold = fLoThreshold;
	_fHiThreshold = fHiThreshold;

	UpdateRehashThresholds();

	if( bRehashNow && ((_nElements > _nHiRehashThreshold) || 
		(_nElements < _nLoRehashThreshold)) )
	{
		Rehash( PickSize( _nElements ) );
	}
}

template< typename K, typename V, class KTraits, class VTraits >
void Map< K, V, KTraits, VTraits >::UpdateRehashThresholds() throw()
{
	_nHiRehashThreshold = size_t( _fHiThreshold*_nBins );
	_nLoRehashThreshold = size_t( _fLoThreshold*_nBins );
	if( _nLoRehashThreshold < 17 )
	{
		_nLoRehashThreshold = 0;
	}
}

template< typename K, typename V, class KTraits, class VTraits >
bool Map< K, V, KTraits, VTraits >::InitHashTable( UINT nBins, bool bAllocNow )
{
	FTLASSUME( _nElements == 0 );
	FTLASSERT( nBins > 0 );

	if( _ppBins != NULL )
	{
		delete[] _ppBins;
		_ppBins = NULL;
	}

	if( bAllocNow )
	{
		FTLTRY( _ppBins = new Node*[nBins] );
		if( _ppBins == NULL )
		{
			return false;
		}
		memset( _ppBins, 0, sizeof( Node* )*nBins );
	}
	_nBins = nBins;

	UpdateRehashThresholds();

	return true;
}

template< typename K, typename V, class KTraits, class VTraits >
void Map< K, V, KTraits, VTraits >::RemoveAll()
{
	DisableAutoRehash();
	if( _ppBins != NULL )
	{
		for( UINT iBin = 0; iBin < _nBins; iBin++ )
		{
			Node* pNext;

			pNext = _ppBins[iBin];
			while( pNext != NULL )
			{
				Node* pKill;

				pKill = pNext;
				pNext = pNext->_pNext;
				FreeNode( pKill );
			}
		}
	}

	delete[] _ppBins;
	_ppBins = NULL;
	_nElements = 0;

	if( !IsLocked() )
	{
		InitHashTable( PickSize( _nElements ), false );
	}

	FreePlexes();
	EnableAutoRehash();
}

template< typename K, typename V, class KTraits, class VTraits >
Map< K, V, KTraits, VTraits >::~Map() throw()
{
	_FTLTRY
	{
		RemoveAll();
	}
	_FTLCATCHALL()
	{
		FTLASSERT(false);		
	}
}

#pragma push_macro("new")
#undef new

template< typename K, typename V, class KTraits, class VTraits >
typename Map< K, V, KTraits, VTraits >::Node* Map< K, V, KTraits, VTraits >::NewNode( 
	KINARGTYPE key, UINT iBin, UINT nHash )
{
	Node* pNewNode;

	if( _pFree == NULL )
	{
		Plex* pPlex;
		Node* pNode;

		pPlex = Plex::Create( _pBlocks, _nBlockSize, sizeof( Node ) );
		if( pPlex == NULL )
		{
			FtlThrow( E_OUTOFMEMORY );
		}
		pNode = (Node*)pPlex->data();
		pNode += _nBlockSize-1;
		for( int iBlock = _nBlockSize-1; iBlock >= 0; iBlock-- )
		{
			pNode->_pNext = _pFree;
			_pFree = pNode;
			pNode--;
		}
	}
	FTLENSURE(_pFree != NULL );
	pNewNode = _pFree;
	_pFree = pNewNode->_pNext;

	_FTLTRY
	{
		::new( pNewNode ) Node( key, nHash );
	}
	_FTLCATCHALL()
	{
		pNewNode->_pNext = _pFree;
		_pFree = pNewNode;

		_FTLRETHROW;
	}
	_nElements++;

	pNewNode->_pNext = _ppBins[iBin];
	_ppBins[iBin] = pNewNode;

	if( (_nElements > _nHiRehashThreshold) && !IsLocked() )
	{
		Rehash( PickSize( _nElements ) );
	}

	return( pNewNode );
}

#pragma pop_macro("new")

template< typename K, typename V, class KTraits, class VTraits >
void Map< K, V, KTraits, VTraits >::FreeNode( Node* pNode )
{
	FTLENSURE( pNode != NULL );

	pNode->~Node();
	pNode->_pNext = _pFree;
	_pFree = pNode;

	FTLASSUME( _nElements > 0 );
	_nElements--;

	if( (_nElements < _nLoRehashThreshold) && !IsLocked() )
	{
		Rehash( PickSize( _nElements ) );
	}

	if( _nElements == 0 )
	{
		FreePlexes();
	}
}

template< typename K, typename V, class KTraits, class VTraits >
void Map< K, V, KTraits, VTraits >::FreePlexes() throw()
{
	_pFree = NULL;
	if( _pBlocks != NULL )
	{
		_pBlocks->FreeDataChain();
		_pBlocks = NULL;
	}
}

template< typename K, typename V, class KTraits, class VTraits >
typename Map< K, V, KTraits, VTraits >::Node* Map< K, V, KTraits, VTraits >::GetNode(
	KINARGTYPE key, UINT& iBin, UINT& nHash, Node*& pPrev ) const throw()
{
	Node* pFollow;

	nHash = KTraits::Hash( key );
	iBin = nHash%_nBins;

	if( _ppBins == NULL )
	{
		return( NULL );
	}

	pFollow = NULL;
	pPrev = NULL;
	for( Node* pNode = _ppBins[iBin]; pNode != NULL; pNode = pNode->_pNext )
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

template< typename K, typename V, class KTraits, class VTraits >
bool Map< K, V, KTraits, VTraits >::Lookup( KINARGTYPE key, VOUTARGTYPE value ) const
{
	UINT iBin;
	UINT nHash;
	Node* pNode;
	Node* pPrev;

	pNode = GetNode( key, iBin, nHash, pPrev );
	if( pNode == NULL )
	{
		return( false );
	}

	value = pNode->_value;

	return( true );
}

template< typename K, typename V, class KTraits, class VTraits >
const typename Map< K, V, KTraits, VTraits >::Pair* Map< K, V, KTraits, VTraits >::Lookup( KINARGTYPE key ) const throw()
{
	UINT iBin;
	UINT nHash;
	Node* pNode;
	Node* pPrev;

	pNode = GetNode( key, iBin, nHash, pPrev );

	return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
typename Map< K, V, KTraits, VTraits >::Pair* Map< K, V, KTraits, VTraits >::Lookup( KINARGTYPE key ) throw()
{
	UINT iBin;
	UINT nHash;
	Node* pNode;
	Node* pPrev;

	pNode = GetNode( key, iBin, nHash, pPrev );

	return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
bool Map< K, V, KTraits, VTraits >::RemoveKey( KINARGTYPE key ) throw()
{
	Node* pNode;
	UINT iBin;
	UINT nHash;
	Node* pPrev;

	pPrev = NULL;
	pNode = GetNode( key, iBin, nHash, pPrev );
	if( pNode == NULL )
	{
		return( false );
	}

	RemoveNode( pNode, pPrev );

	return( true );
}

template< typename K, typename V, class KTraits, class VTraits >
void Map< K, V, KTraits, VTraits >::RemoveNode( Node* pNode, Node* pPrev ) throw()
{
	FTLENSURE( pNode != NULL );

	UINT iBin = pNode->GetHash() % _nBins;

	if( pPrev == NULL )
	{
		FTLASSUME( _ppBins[iBin] == pNode );
		_ppBins[iBin] = pNode->_pNext;
	}
	else
	{
		FTLASSERT( pPrev->_pNext == pNode );
		pPrev->_pNext = pNode->_pNext;
	}
	FreeNode( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
void Map< K, V, KTraits, VTraits >::RemoveAtPos( POSITION pos ) throw()
{
	FTLENSURE( pos != NULL );

	Node* pNode = static_cast< Node* >( pos );
	Node* pPrev = NULL;
	UINT iBin = pNode->GetHash() % _nBins;

	FTLASSUME( _ppBins[iBin] != NULL );
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
			FTLASSERT( pPrev != NULL );
		}
	}
	RemoveNode( pNode, pPrev );
}

template< typename K, typename V, class KTraits, class VTraits >
void Map< K, V, KTraits, VTraits >::Rehash( UINT nBins )
{
	Node** ppBins = NULL;

	if( nBins == 0 )
	{
		nBins = PickSize( _nElements );
	}

	if( nBins == _nBins )
	{
		return;
	}

//	FTLTRACE(atlTraceMap, 2, _T("Rehash: %u bins\n"), nBins );

	if( _ppBins == NULL )
	{
		// Just set the new number of bins
		InitHashTable( nBins, false );
		return;
	}

	FTLTRY(ppBins = new Node*[nBins]);
	if (ppBins == NULL)
	{
		FtlThrow( E_OUTOFMEMORY );
	}

	memset( ppBins, 0, nBins*sizeof( Node* ) );

	// Nothing gets copied.  We just rewire the old nodes
	// into the new bins.
	for( UINT iSrcBin = 0; iSrcBin < _nBins; iSrcBin++ )
	{
		Node* pNode;

		pNode = _ppBins[iSrcBin];
		while( pNode != NULL )
		{
			Node* pNext;
			UINT iDestBin;

			pNext = pNode->_pNext;  // Save so we don't trash it
			iDestBin = pNode->GetHash()%nBins;
			pNode->_pNext = ppBins[iDestBin];
			ppBins[iDestBin] = pNode;

			pNode = pNext;
		}
	}

	delete[] _ppBins;
	_ppBins = ppBins;
	_nBins = nBins;

	UpdateRehashThresholds();
}

template< typename K, typename V, class KTraits, class VTraits >
void Map< K, V, KTraits, VTraits >::GetNextAssoc( POSITION& pos, KOUTARGTYPE key,
													 VOUTARGTYPE value ) const
{
	Node* pNode;
	Node* pNext;

	FTLASSUME( _ppBins != NULL );
	FTLENSURE( pos != NULL );

	pNode = (Node*)pos;
	pNext = FindNextNode( pNode );

	pos = POSITION( pNext );
	key = pNode->_key;
	value = pNode->_value;
}

template< typename K, typename V, class KTraits, class VTraits >
const typename Map< K, V, KTraits, VTraits >::Pair* Map< K, V, KTraits, VTraits >::GetNext( POSITION& pos ) const throw()
{
	Node* pNode;
	Node* pNext;

	FTLASSUME( _ppBins != NULL );
	FTLASSERT( pos != NULL );

	pNode = (Node*)pos;
	pNext = FindNextNode( pNode );

	pos = POSITION( pNext );

	return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
typename Map< K, V, KTraits, VTraits >::Pair* Map< K, V, KTraits, VTraits >::GetNext( 
	POSITION& pos ) throw()
{
	FTLASSUME( _ppBins != NULL );
	FTLASSERT( pos != NULL );

	Node* pNode = static_cast< Node* >( pos );
	Node* pNext = FindNextNode( pNode );

	pos = POSITION( pNext );

	return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
const K& Map< K, V, KTraits, VTraits >::GetNextKey( POSITION& pos ) const
{
	Node* pNode;
	Node* pNext;

	FTLASSUME( _ppBins != NULL );
	FTLENSURE( pos != NULL );

	pNode = (Node*)pos;
	pNext = FindNextNode( pNode );

	pos = POSITION( pNext );

	return( pNode->_key );
}

template< typename K, typename V, class KTraits, class VTraits >
const V& Map< K, V, KTraits, VTraits >::GetNextValue( POSITION& pos ) const
{
	Node* pNode;
	Node* pNext;

	FTLASSUME( _ppBins != NULL );
	FTLENSURE( pos != NULL );

	pNode = (Node*)pos;
	pNext = FindNextNode( pNode );

	pos = POSITION( pNext );

	return( pNode->_value );
}

template< typename K, typename V, class KTraits, class VTraits >
V& Map< K, V, KTraits, VTraits >::GetNextValue( POSITION& pos )
{
	Node* pNode;
	Node* pNext;

	FTLASSUME( _ppBins != NULL );
	FTLENSURE( pos != NULL );

	pNode = (Node*)pos;
	pNext = FindNextNode( pNode );

	pos = POSITION( pNext );

	return( pNode->_value );
}

template< typename K, typename V, class KTraits, class VTraits >
typename Map< K, V, KTraits, VTraits >::Node* Map< K, V, KTraits, VTraits >::FindNextNode( Node* pNode ) const throw()
{
	Node* pNext;

	if(pNode == NULL)
	{
		FTLASSERT(FALSE);
		return NULL;
	}

	if( pNode->_pNext != NULL )
	{
		pNext = pNode->_pNext;
	}
	else
	{
		UINT iBin;

		pNext = NULL;
		iBin = (pNode->GetHash()%_nBins)+1;
		while( (pNext == NULL) && (iBin < _nBins) )
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

#ifdef _DEBUG
template< typename K, typename V, class KTraits, class VTraits >
void Map< K, V, KTraits, VTraits >::AssertValid() const
{
	FTLASSUME( _nBins > 0 );
	// non-empty map should have hash table
	FTLASSERT( IsEmpty() || (_ppBins != NULL) );
}
#endif
////========= RBMap ======================================================
#pragma push_macro("new")
#undef new

//
// The red-black tree code is based on the the descriptions in
// "Introduction to Algorithms", by Cormen, Leiserson, and Rivest
//
template< typename K, typename V, class KTraits = ElementTraits< K >, class VTraits = ElementTraits< V > >
class RBTree
{
public:
	typedef typename KTraits::INARGTYPE KINARGTYPE;
	typedef typename KTraits::OUTARGTYPE KOUTARGTYPE;
	typedef typename VTraits::INARGTYPE VINARGTYPE;
	typedef typename VTraits::OUTARGTYPE VOUTARGTYPE;

public:
	class Pair : 
		public __POSITION
	{
	protected:

		Pair( KINARGTYPE key, VINARGTYPE value ) : 
			 _key( key ),
				 _value( value )
			 {
			 }
			 ~Pair() throw()
			 {
			 }

	public:
		const K _key;
		V _value;
	};

private:

	class Node : 
		public Pair
	{
	public:
		enum RB_COLOR
		{
			RB_RED, 
			RB_BLACK
		};

	public:
		RB_COLOR _eColor;
		Node* _pLeft;
		Node* _pRight;
		Node* _pParent;

		Node( KINARGTYPE key, VINARGTYPE value ) : 
		Pair( key, value ),
			_pParent( NULL ),
			_eColor( RB_BLACK )
		{
		}
		~Node() throw()
		{
		}
	};

private:
	Node* _pRoot;
	size_t _nCount;
	Node* _pFree;
	Plex* _pBlocks;
	size_t _nBlockSize;

	// sentinel node
	Node *_pNil;

	// methods
	bool IsNil(Node *p) const throw();
	void SetNil(Node **p) throw();

	Node* NewNode( KINARGTYPE key, VINARGTYPE value ) ;
	void FreeNode(Node* pNode) throw();
	void RemovePostOrder(Node* pNode) throw();
	Node* LeftRotate(Node* pNode) throw();
	Node* RightRotate(Node* pNode) throw();
	void SwapNode(Node* pDest, Node* pSrc) throw();
	Node* InsertImpl( KINARGTYPE key, VINARGTYPE value ) ;
	void RBDeleteFixup(Node* pNode) throw();
	bool RBDelete(Node* pZ) throw();

#ifdef _DEBUG

	// internal debugging code to verify red-black properties of tree:
	// 1) Every node is either red or black
	// 2) Every leaf (NIL) is black
	// 3) If a node is red, both its children are black
	// 4) Every simple path from a node to a descendant leaf node contains 
	//    the same number of black nodes
private:
	void VerifyIntegrity(const Node *pNode, int nCurrBlackDepth, int &nBlackDepth) const throw();

public:
	void VerifyIntegrity() const throw();

#endif // _DEBUG

protected:
	Node* Minimum(Node* pNode) const throw();
	Node* Maximum(Node* pNode) const throw();
	Node* Predecessor( Node* pNode ) const throw();
	Node* Successor(Node* pNode) const throw();
	Node* RBInsert( KINARGTYPE key, VINARGTYPE value ) ;
	Node* Find(KINARGTYPE key) const throw();
	Node* FindPrefix( KINARGTYPE key ) const throw();

protected:
	explicit RBTree( size_t nBlockSize = 10 ) throw();  // protected to prevent instantiation

public:
	~RBTree() throw();

	void RemoveAll() throw();
	void RemoveAt(POSITION pos) throw();

	size_t GetCount() const throw();
	bool IsEmpty() const throw();

	POSITION FindFirstKeyAfter( KINARGTYPE key ) const throw();

	POSITION GetHeadPosition() const throw();
	POSITION GetTailPosition() const throw();
	void GetNextAssoc( POSITION& pos, KOUTARGTYPE key, VOUTARGTYPE value ) const;
	const Pair* GetNext(POSITION& pos) const throw();
	Pair* GetNext(POSITION& pos) throw();
	const Pair* GetPrev(POSITION& pos) const throw();
	Pair* GetPrev(POSITION& pos) throw();
	const K& GetNextKey(POSITION& pos) const throw();
	const V& GetNextValue(POSITION& pos) const throw();
	V& GetNextValue(POSITION& pos) throw();

	Pair* GetAt( POSITION pos ) throw();
	const Pair* GetAt( POSITION pos ) const throw();
	void GetAt(POSITION pos, KOUTARGTYPE key, VOUTARGTYPE value) const;
	const K& GetKeyAt(POSITION pos) const;
	const V& GetValueAt(POSITION pos) const;
	V& GetValueAt(POSITION pos);
	void SetValueAt(POSITION pos, VINARGTYPE value);

private:
	// Private to prevent use
	RBTree( const RBTree& ) throw();
	RBTree& operator=( const RBTree& ) throw();
};

template< typename K, typename V, class KTraits, class VTraits >
inline bool RBTree< K, V, KTraits, VTraits >::IsNil(Node *p) const throw()
{
	return ( p == _pNil );
}

template< typename K, typename V, class KTraits, class VTraits >
inline void RBTree< K, V, KTraits, VTraits >::SetNil(Node **p) throw()
{
	FTLENSURE( p != NULL );
	*p = _pNil;
}

template< typename K, typename V, class KTraits, class VTraits >
RBTree< K, V, KTraits, VTraits >::RBTree( size_t nBlockSize ) throw() :
_pRoot( NULL ),
_nCount( 0 ),
_nBlockSize( nBlockSize ),
_pFree( NULL ),
_pBlocks( NULL ),
_pNil( NULL )
{
	FTLASSERT( nBlockSize > 0 );
}

template< typename K, typename V, class KTraits, class VTraits >
RBTree< K, V, KTraits, VTraits >::~RBTree() throw()
{
	RemoveAll();
	if (_pNil != NULL)
	{
		free(_pNil);
	}
}

template< typename K, typename V, class KTraits, class VTraits >
void RBTree< K, V, KTraits, VTraits >::RemoveAll() throw()
{
	if (!IsNil(_pRoot))
		RemovePostOrder(_pRoot);
	_nCount = 0;
	_pBlocks->FreeDataChain();
	_pBlocks = NULL;
	_pFree = NULL;
	_pRoot = _pNil;
}

template< typename K, typename V, class KTraits, class VTraits >
size_t RBTree< K, V, KTraits, VTraits >::GetCount() const throw()
{
	return _nCount;
}

template< typename K, typename V, class KTraits, class VTraits >
bool RBTree< K, V, KTraits, VTraits >::IsEmpty() const throw()
{
	return( _nCount == 0 );
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION RBTree< K, V, KTraits, VTraits >::FindFirstKeyAfter( KINARGTYPE key ) const throw()
{
	return( FindPrefix( key ) );
}

template< typename K, typename V, class KTraits, class VTraits >
void RBTree< K, V, KTraits, VTraits >::RemoveAt(POSITION pos) throw()
{
	FTLASSERT(pos != NULL);
	RBDelete(static_cast<Node*>(pos));
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION RBTree< K, V, KTraits, VTraits >::GetHeadPosition() const throw()
{
	return( Minimum( _pRoot ) );
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION RBTree< K, V, KTraits, VTraits >::GetTailPosition() const throw()
{
	return( Maximum( _pRoot ) );
}

template< typename K, typename V, class KTraits, class VTraits >
void RBTree< K, V, KTraits, VTraits >::GetNextAssoc( POSITION& pos, KOUTARGTYPE key, VOUTARGTYPE value ) const
{
	FTLASSERT(pos != NULL);
	Node* pNode = static_cast< Node* >(pos);

	key = pNode->_key;
	value = pNode->_value;

	pos = Successor(pNode);
}

template< typename K, typename V, class KTraits, class VTraits >
const typename RBTree< K, V, KTraits, VTraits >::Pair* RBTree< K, V, KTraits, VTraits >::GetNext(POSITION& pos) const throw()
{
	FTLASSERT(pos != NULL);
	Node* pNode = static_cast< Node* >(pos);
	pos = Successor(pNode);
	return pNode;
}

template< typename K, typename V, class KTraits, class VTraits >
typename RBTree< K, V, KTraits, VTraits >::Pair* RBTree< K, V, KTraits, VTraits >::GetNext(POSITION& pos) throw()
{
	FTLASSERT(pos != NULL);
	Node* pNode = static_cast< Node* >(pos);
	pos = Successor(pNode);
	return pNode;
}

template< typename K, typename V, class KTraits, class VTraits >
const typename RBTree< K, V, KTraits, VTraits >::Pair* RBTree< K, V, KTraits, VTraits >::GetPrev(POSITION& pos) const throw()
{
	FTLASSERT(pos != NULL);
	Node* pNode = static_cast< Node* >(pos);
	pos = Predecessor(pNode);

	return pNode;
}

template< typename K, typename V, class KTraits, class VTraits >
typename RBTree< K, V, KTraits, VTraits >::Pair* RBTree< K, V, KTraits, VTraits >::GetPrev(POSITION& pos) throw()
{
	FTLASSERT(pos != NULL);
	Node* pNode = static_cast< Node* >(pos);
	pos = Predecessor(pNode);

	return pNode;
}

template< typename K, typename V, class KTraits, class VTraits >
const K& RBTree< K, V, KTraits, VTraits >::GetNextKey(POSITION& pos) const throw()
{
	FTLASSERT(pos != NULL);
	Node* pNode = static_cast<Node*>(pos);
	pos = Successor(pNode);

	return pNode->_key;
}

template< typename K, typename V, class KTraits, class VTraits >
const V& RBTree< K, V, KTraits, VTraits >::GetNextValue(POSITION& pos) const throw()
{
	FTLASSERT(pos != NULL);
	Node* pNode = static_cast<Node*>(pos);
	pos = Successor(pNode);

	return pNode->_value;
}

template< typename K, typename V, class KTraits, class VTraits >
V& RBTree< K, V, KTraits, VTraits >::GetNextValue(POSITION& pos) throw()
{
	FTLASSERT(pos != NULL);
	Node* pNode = static_cast<Node*>(pos);
	pos = Successor(pNode);

	return pNode->_value;
}

template< typename K, typename V, class KTraits, class VTraits >
typename RBTree< K, V, KTraits, VTraits >::Pair* RBTree< K, V, KTraits, VTraits >::GetAt( POSITION pos ) throw()
{
	FTLASSERT( pos != NULL );

	return( static_cast< Pair* >( pos ) );
}

template< typename K, typename V, class KTraits, class VTraits >
const typename RBTree< K, V, KTraits, VTraits >::Pair* RBTree< K, V, KTraits, VTraits >::GetAt( POSITION pos ) const throw()
{
	FTLASSERT( pos != NULL );

	return( static_cast< const Pair* >( pos ) );
}

template< typename K, typename V, class KTraits, class VTraits >
void RBTree< K, V, KTraits, VTraits >::GetAt(POSITION pos, KOUTARGTYPE key, VOUTARGTYPE value) const
{
	FTLENSURE( pos != NULL );
	key = static_cast<Node*>(pos)->_key;
	value = static_cast<Node*>(pos)->_value;
}

template< typename K, typename V, class KTraits, class VTraits >
const K& RBTree< K, V, KTraits, VTraits >::GetKeyAt(POSITION pos) const
{
	FTLENSURE( pos != NULL );
	return static_cast<Node*>(pos)->_key;
}

template< typename K, typename V, class KTraits, class VTraits >
const V& RBTree< K, V, KTraits, VTraits >::GetValueAt(POSITION pos) const
{
	FTLENSURE( pos != NULL );
	return static_cast<Node*>(pos)->_value;
}

template< typename K, typename V, class KTraits, class VTraits >
V& RBTree< K, V, KTraits, VTraits >::GetValueAt(POSITION pos)
{
	FTLENSURE( pos != NULL );
	return static_cast<Node*>(pos)->_value;
}

template< typename K, typename V, class KTraits, class VTraits >
void RBTree< K, V, KTraits, VTraits >::SetValueAt(POSITION pos, VINARGTYPE value)
{
	FTLENSURE( pos != NULL );
	static_cast<Node*>(pos)->_value = value;
}

template< typename K, typename V, class KTraits, class VTraits >
typename RBTree< K, V, KTraits, VTraits >::Node* RBTree< K, V, KTraits, VTraits >::NewNode( KINARGTYPE key, VINARGTYPE value ) 
{
	if( _pFree == NULL )
	{
		if (_pNil == NULL)
		{
			_pNil = reinterpret_cast<Node *>(malloc(sizeof( Node )));
			if (_pNil == NULL)
			{
				FtlThrow( E_OUTOFMEMORY );
			}
			memset(_pNil, 0x00, sizeof(Node));
			_pNil->_eColor = Node::RB_BLACK;
			_pNil->_pParent = _pNil->_pLeft = _pNil->_pRight = _pNil;
			_pRoot = _pNil;
		}

		Plex* pPlex = Plex::Create( _pBlocks, _nBlockSize, sizeof( Node ) );
		if( pPlex == NULL )
		{
			FtlThrow( E_OUTOFMEMORY );
		}
		Node* pNode = static_cast< Node* >( pPlex->data() );
		pNode += _nBlockSize-1;
		for( INT_PTR iBlock = _nBlockSize-1; iBlock >= 0; iBlock-- )
		{
			pNode->_pLeft = _pFree;
			_pFree = pNode;
			pNode--;
		}
	}
	FTLASSUME( _pFree != NULL );

	Node* pNewNode = _pFree;
	::new( pNewNode ) Node( key, value );

	_pFree = _pFree->_pLeft;
	pNewNode->_eColor = Node::RB_RED;
	SetNil(&pNewNode->_pLeft);
	SetNil(&pNewNode->_pRight);
	SetNil(&pNewNode->_pParent);

	_nCount++;
	FTLASSUME( _nCount > 0 );

	return( pNewNode );
}

template< typename K, typename V, class KTraits, class VTraits >
void RBTree< K, V, KTraits, VTraits >::FreeNode(Node* pNode) throw()
{
	FTLENSURE( pNode != NULL );
	pNode->~Node();
	pNode->_pLeft = _pFree;
	_pFree = pNode;
	FTLASSUME( _nCount > 0 );
	_nCount--;
}

template< typename K, typename V, class KTraits, class VTraits >
void RBTree< K, V, KTraits, VTraits >::RemovePostOrder(Node* pNode) throw()
{
	if (IsNil(pNode))
		return;
	RemovePostOrder(pNode->_pLeft);
	RemovePostOrder(pNode->_pRight);
	FreeNode( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
typename RBTree< K, V, KTraits, VTraits >::Node* RBTree< K, V, KTraits, VTraits >::LeftRotate(Node* pNode) throw()
{
	FTLASSERT(pNode != NULL);
	if(pNode == NULL)
		return NULL;

	Node* pRight = pNode->_pRight;
	pNode->_pRight = pRight->_pLeft;
	if (!IsNil(pRight->_pLeft))
		pRight->_pLeft->_pParent = pNode;

	pRight->_pParent = pNode->_pParent;
	if (IsNil(pNode->_pParent))
		_pRoot = pRight;
	else if (pNode == pNode->_pParent->_pLeft)
		pNode->_pParent->_pLeft = pRight;
	else 
		pNode->_pParent->_pRight = pRight;

	pRight->_pLeft = pNode;
	pNode->_pParent = pRight;
	return pNode;

}

template< typename K, typename V, class KTraits, class VTraits >
typename RBTree< K, V, KTraits, VTraits >::Node* RBTree< K, V, KTraits, VTraits >::RightRotate(Node* pNode) throw()
{
	FTLASSERT(pNode != NULL);
	if(pNode == NULL)
		return NULL;

	Node* pLeft = pNode->_pLeft;
	pNode->_pLeft = pLeft->_pRight;
	if (!IsNil(pLeft->_pRight))
		pLeft->_pRight->_pParent = pNode;

	pLeft->_pParent = pNode->_pParent;
	if (IsNil(pNode->_pParent))
		_pRoot = pLeft;
	else if (pNode == pNode->_pParent->_pRight)
		pNode->_pParent->_pRight = pLeft;
	else
		pNode->_pParent->_pLeft = pLeft;

	pLeft->_pRight = pNode;
	pNode->_pParent = pLeft;
	return pNode;
}

template< typename K, typename V, class KTraits, class VTraits >
typename RBTree< K, V, KTraits, VTraits >::Node* RBTree< K, V, KTraits, VTraits >::Find(KINARGTYPE key) const throw()
{
	Node* pKey = NULL;
	Node* pNode = _pRoot;
	while( !IsNil(pNode) && (pKey == NULL) )
	{
		int nCompare = KTraits::CompareElementsOrdered( key, pNode->_key );
		if( nCompare == 0 )
		{
			pKey = pNode;
		}
		else
		{
			if( nCompare < 0 )
			{
				pNode = pNode->_pLeft;
			}
			else
			{
				pNode = pNode->_pRight;
			}
		}
	}

	if( pKey == NULL )
	{
		return( NULL );
	}

#pragma warning(push)
#pragma warning(disable:4127)

	while( true )
	{
		Node* pPrev = Predecessor( pKey );
		if( (pPrev != NULL) && KTraits::CompareElements( key, pPrev->_key ) )
		{
			pKey = pPrev;
		}
		else
		{
			return( pKey );
		}
	}

#pragma warning(pop)
}

template< typename K, typename V, class KTraits, class VTraits >
typename RBTree< K, V, KTraits, VTraits >::Node* RBTree< K, V, KTraits, VTraits >::FindPrefix( KINARGTYPE key ) const throw()
{
	// First, attempt to find a node that matches the key exactly
	Node* pParent = NULL;
	Node* pKey = NULL;
	Node* pNode = _pRoot;
	while( !IsNil(pNode) && (pKey == NULL) )
	{
		pParent = pNode;
		int nCompare = KTraits::CompareElementsOrdered( key, pNode->_key );
		if( nCompare == 0 )
		{
			pKey = pNode;
		}
		else if( nCompare < 0 )
		{
			pNode = pNode->_pLeft;
		}
		else
		{
			pNode = pNode->_pRight;
		}
	}

	if( pKey != NULL )
	{
		// We found a node with the exact key, so find the first node after 
		// this one with a different key 
		while( true )
		{
			Node* pNext = Successor( pKey );
			if ((pNext != NULL) && KTraits::CompareElements( key, pNext->_key ))
			{
				pKey = pNext;
			}
			else
			{
				return pNext;
			}
		}
	}
	else if (pParent != NULL)
	{
		// No node matched the key exactly, so pick the first node with 
		// a key greater than the given key
		int nCompare = KTraits::CompareElementsOrdered( key, pParent->_key );
		if( nCompare < 0 )
		{
			pKey = pParent;
		}
		else
		{
			FTLASSERT( nCompare > 0 );
			pKey = Successor( pParent );
		}
	}

	return( pKey );
}

template< typename K, typename V, class KTraits, class VTraits >
void RBTree< K, V, KTraits, VTraits >::SwapNode(Node* pDest, Node* pSrc) throw()
{
	FTLENSURE( pDest != NULL );
	FTLENSURE( pSrc != NULL );

	pDest->_pParent = pSrc->_pParent;
	if (pSrc->_pParent->_pLeft == pSrc)
	{
		pSrc->_pParent->_pLeft = pDest;
	}
	else
	{
		pSrc->_pParent->_pRight = pDest;
	}

	pDest->_pRight = pSrc->_pRight;
	pDest->_pLeft = pSrc->_pLeft;
	pDest->_eColor = pSrc->_eColor;
	pDest->_pRight->_pParent = pDest;
	pDest->_pLeft->_pParent = pDest;

	if (_pRoot == pSrc)
	{
		_pRoot = pDest;
	}
}

template< typename K, typename V, class KTraits, class VTraits >
typename RBTree< K, V, KTraits, VTraits >::Node* RBTree< K, V, KTraits, VTraits >::InsertImpl( KINARGTYPE key, VINARGTYPE value ) 
{
	Node* pNew = NewNode( key, value );

	Node* pY = NULL;
	Node* pX = _pRoot;

	while (!IsNil(pX))
	{
		pY = pX;
		if( KTraits::CompareElementsOrdered( key, pX->_key ) <= 0 )
			pX = pX->_pLeft;
		else
			pX = pX->_pRight;
	}

	pNew->_pParent = pY;
	if (pY == NULL)
	{
		_pRoot = pNew;
	}
	else if( KTraits::CompareElementsOrdered( key, pY->_key ) <= 0 )
		pY->_pLeft = pNew;
	else
		pY->_pRight = pNew;

	return pNew;
}

template< typename K, typename V, class KTraits, class VTraits >


void RBTree< K, V, KTraits, VTraits >::RBDeleteFixup(Node* pNode) throw()
{
	FTLENSURE( pNode != NULL );

	Node* pX = pNode;
	Node* pW = NULL;

	while (( pX != _pRoot ) && ( pX->_eColor == Node::RB_BLACK ))
	{
		if (pX == pX->_pParent->_pLeft)
		{
			pW = pX->_pParent->_pRight;
			if (pW->_eColor == Node::RB_RED)
			{
				pW->_eColor = Node::RB_BLACK;
				pW->_pParent->_eColor = Node::RB_RED;
				LeftRotate(pX->_pParent);
				pW = pX->_pParent->_pRight;
			}
			if (pW->_pLeft->_eColor == Node::RB_BLACK && pW->_pRight->_eColor == Node::RB_BLACK)
			{
				pW->_eColor = Node::RB_RED;
				pX = pX->_pParent;
			}
			else
			{
				if (pW->_pRight->_eColor == Node::RB_BLACK)
				{
					pW->_pLeft->_eColor = Node::RB_BLACK;
					pW->_eColor = Node::RB_RED;
					RightRotate(pW);
					pW = pX->_pParent->_pRight;
				}
				pW->_eColor = pX->_pParent->_eColor;
				pX->_pParent->_eColor = Node::RB_BLACK;
				pW->_pRight->_eColor = Node::RB_BLACK;
				LeftRotate(pX->_pParent);
				pX = _pRoot;
			}
		}
		else
		{
			pW = pX->_pParent->_pLeft;
			if (pW->_eColor == Node::RB_RED)
			{
				pW->_eColor = Node::RB_BLACK;
				pW->_pParent->_eColor = Node::RB_RED;
				RightRotate(pX->_pParent);
				pW = pX->_pParent->_pLeft;
			}
			if (pW->_pRight->_eColor == Node::RB_BLACK && pW->_pLeft->_eColor == Node::RB_BLACK)
			{
				pW->_eColor = Node::RB_RED;
				pX = pX->_pParent;
			}
			else
			{
				if (pW->_pLeft->_eColor == Node::RB_BLACK)
				{
					pW->_pRight->_eColor = Node::RB_BLACK;
					pW->_eColor = Node::RB_RED;
					LeftRotate(pW);
					pW = pX->_pParent->_pLeft;
				}
				pW->_eColor = pX->_pParent->_eColor;
				pX->_pParent->_eColor = Node::RB_BLACK;
				pW->_pLeft->_eColor = Node::RB_BLACK;
				RightRotate(pX->_pParent);
				pX = _pRoot;
			}
		}
	}

	pX->_eColor = Node::RB_BLACK;
}


template< typename K, typename V, class KTraits, class VTraits >
bool RBTree< K, V, KTraits, VTraits >::RBDelete(Node* pZ) throw()
{
	if (pZ == NULL)
		return false;

	Node* pY = NULL;
	Node* pX = NULL;
	if (IsNil(pZ->_pLeft) || IsNil(pZ->_pRight))
		pY = pZ;
	else
		pY = Successor(pZ);

	if (!IsNil(pY->_pLeft))
		pX = pY->_pLeft;
	else
		pX = pY->_pRight;

	pX->_pParent = pY->_pParent;

	if (IsNil(pY->_pParent))
		_pRoot = pX;
	else if (pY == pY->_pParent->_pLeft)
		pY->_pParent->_pLeft = pX;
	else
		pY->_pParent->_pRight = pX;

	if (pY->_eColor == Node::RB_BLACK)
		RBDeleteFixup(pX);

	if (pY != pZ)
		SwapNode(pY, pZ);

	if (_pRoot != NULL)
		SetNil(&_pRoot->_pParent);

	FreeNode( pZ );

	return true;
}

template< typename K, typename V, class KTraits, class VTraits >
typename RBTree< K, V, KTraits, VTraits >::Node* RBTree< K, V, KTraits, VTraits >::Minimum(Node* pNode) const throw()
{
	if (pNode == NULL || IsNil(pNode))
	{
		return NULL;
	}

	Node* pMin = pNode;
	while (!IsNil(pMin->_pLeft))
	{
		pMin = pMin->_pLeft;
	}

	return pMin;
}

template< typename K, typename V, class KTraits, class VTraits >
typename RBTree< K, V, KTraits, VTraits >::Node* RBTree< K, V, KTraits, VTraits >::Maximum(Node* pNode) const throw()
{
	if (pNode == NULL || IsNil(pNode))
	{
		return NULL;
	}

	Node* pMax = pNode;
	while (!IsNil(pMax->_pRight))
	{
		pMax = pMax->_pRight;
	}

	return pMax;
}

template< typename K, typename V, class KTraits, class VTraits >
typename RBTree< K, V, KTraits, VTraits >::Node* RBTree< K, V, KTraits, VTraits >::Predecessor( Node* pNode ) const throw()
{
	if( pNode == NULL )
	{
		return( NULL );
	}
	if( !IsNil(pNode->_pLeft) )
	{
		return( Maximum( pNode->_pLeft ) );
	}

	Node* pParent = pNode->_pParent;
	Node* pLeft = pNode;
	while( !IsNil(pParent) && (pLeft == pParent->_pLeft) )
	{
		pLeft = pParent;
		pParent = pParent->_pParent;
	}

	if (IsNil(pParent))
	{
		pParent = NULL;
	}
	return( pParent );
}

template< typename K, typename V, class KTraits, class VTraits >
typename RBTree< K, V, KTraits, VTraits >::Node* RBTree< K, V, KTraits, VTraits >::Successor(Node* pNode) const throw()
{
	if ( pNode == NULL )
	{
		return NULL;
	}
	if ( !IsNil(pNode->_pRight) )
	{
		return Minimum(pNode->_pRight);
	}

	Node* pParent = pNode->_pParent;
	Node* pRight = pNode;
	while ( !IsNil(pParent) && (pRight == pParent->_pRight) )
	{
		pRight = pParent;
		pParent = pParent->_pParent;
	}

	if (IsNil(pParent))
	{
		pParent = NULL;
	}
	return pParent;
}

template< typename K, typename V, class KTraits, class VTraits >
typename RBTree< K, V, KTraits, VTraits >::Node* RBTree< K, V, KTraits, VTraits >::RBInsert( KINARGTYPE key, VINARGTYPE value ) 
{
	Node* pNewNode = InsertImpl( key, value );

	Node* pX = pNewNode;
	pX->_eColor = Node::RB_RED;
	Node* pY = NULL;
	while (pX != _pRoot && pX->_pParent->_eColor == Node::RB_RED)
	{
		if (pX->_pParent == pX->_pParent->_pParent->_pLeft)
		{
			pY = pX->_pParent->_pParent->_pRight;
			if (pY != NULL && pY->_eColor == Node::RB_RED)
			{
				pX->_pParent->_eColor = Node::RB_BLACK;
				pY->_eColor = Node::RB_BLACK;
				pX->_pParent->_pParent->_eColor = Node::RB_RED;
				pX = pX->_pParent->_pParent;
			}
			else
			{
				if (pX == pX->_pParent->_pRight)
				{
					pX = pX->_pParent;
					LeftRotate(pX);
				}
				pX->_pParent->_eColor = Node::RB_BLACK;
				pX->_pParent->_pParent->_eColor = Node::RB_RED;
				RightRotate(pX->_pParent->_pParent);
			}
		}
		else
		{
			pY = pX->_pParent->_pParent->_pLeft;
			if (pY != NULL && pY->_eColor == Node::RB_RED)
			{
				pX->_pParent->_eColor = Node::RB_BLACK;
				pY->_eColor = Node::RB_BLACK;
				pX->_pParent->_pParent->_eColor = Node::RB_RED;
				pX = pX->_pParent->_pParent;
			}
			else
			{
				if (pX == pX->_pParent->_pLeft)
				{
					pX = pX->_pParent;
					RightRotate(pX);
				}
				pX->_pParent->_eColor = Node::RB_BLACK;
				pX->_pParent->_pParent->_eColor = Node::RB_RED;
				LeftRotate(pX->_pParent->_pParent);
			}
		}
	}

	_pRoot->_eColor = Node::RB_BLACK;
	SetNil(&_pRoot->_pParent);

	return( pNewNode );
}

#ifdef _DEBUG

template< typename K, typename V, class KTraits, class VTraits >
void RBTree< K, V, KTraits, VTraits >::VerifyIntegrity(const Node *pNode, int nCurrBlackDepth, int &nBlackDepth) const throw()
{
	bool bCheckForBlack = false;
	bool bLeaf = true;

	if (pNode->_eColor == Node::RB_RED) 
		bCheckForBlack = true;
	else
		nCurrBlackDepth++;

	FTLASSERT(pNode->_pLeft != NULL);
	if (!IsNil(pNode->_pLeft))
	{
		bLeaf = false;
		if (bCheckForBlack)
		{
			FTLASSERT(pNode->_pLeft->_eColor == Node::RB_BLACK);
		}

		VerifyIntegrity(pNode->_pLeft, nCurrBlackDepth, nBlackDepth);
	}

	FTLASSERT(pNode->_pRight != NULL);
	if (!IsNil(pNode->_pRight))
	{
		bLeaf = false;
		if (bCheckForBlack)
		{
			FTLASSERT(pNode->_pRight->_eColor == Node::RB_BLACK);
		}

		VerifyIntegrity(pNode->_pRight, nCurrBlackDepth, nBlackDepth);
	}

	FTLASSERT( pNode->_pParent != NULL );
	FTLASSERT( ( IsNil(pNode->_pParent) ) ||
		( pNode->_pParent->_pLeft == pNode ) ||
		( pNode->_pParent->_pRight == pNode ) );

	if (bLeaf) 
	{
		if (nBlackDepth == 0)
		{
			nBlackDepth = nCurrBlackDepth;
		}
		else 
		{
			FTLASSERT(nBlackDepth == nCurrBlackDepth);
		}
	}
}

template< typename K, typename V, class KTraits, class VTraits >
void RBTree< K, V, KTraits, VTraits >::VerifyIntegrity() const throw()
{
	if ((_pRoot == NULL) || (IsNil(_pRoot)))
		return;

	FTLASSUME(_pRoot->_eColor == Node::RB_BLACK);
	int nBlackDepth = 0;
	VerifyIntegrity(_pRoot, 0, nBlackDepth);
}

#endif // _DEBUG

template< typename K, typename V, class KTraits = ElementTraits< K >, class VTraits = ElementTraits< V > >
class RBMap :
	public RBTree< K, V, KTraits, VTraits >
{
public:
    typedef RBTree< K, V, KTraits, VTraits > RBTREE;
    typedef typename RBTREE::KINARGTYPE KINARGTYPE;
    typedef typename RBTREE::KOUTARGTYPE KOUTARGTYPE;
    typedef typename RBTREE::VINARGTYPE VINARGTYPE;
    typedef typename RBTREE::VOUTARGTYPE VOUTARGTYPE;
    typedef typename RBTREE::Pair Pair;

	explicit RBMap( size_t nBlockSize = 10 ) throw();
	~RBMap() throw();

	bool Lookup( KINARGTYPE key, VOUTARGTYPE value ) const ;
	const Pair* Lookup( KINARGTYPE key ) const throw();
	Pair* Lookup( KINARGTYPE key ) throw();
	POSITION SetAt( KINARGTYPE key, VINARGTYPE value ) ;
	bool RemoveKey( KINARGTYPE key ) throw();
};

template< typename K, typename V, class KTraits, class VTraits >
RBMap< K, V, KTraits, VTraits >::RBMap( size_t nBlockSize ) throw() :
RBTree< K, V, KTraits, VTraits >( nBlockSize )
{
}

template< typename K, typename V, class KTraits, class VTraits >
RBMap< K, V, KTraits, VTraits >::~RBMap() throw()
{
}

template< typename K, typename V, class KTraits, class VTraits >
const typename RBMap< K, V, KTraits, VTraits >::Pair* RBMap< K, V, KTraits, VTraits >::Lookup( KINARGTYPE key ) const throw()
{
	return Find(key);
}

template< typename K, typename V, class KTraits, class VTraits >
typename RBMap< K, V, KTraits, VTraits >::Pair* RBMap< K, V, KTraits, VTraits >::Lookup( KINARGTYPE key ) throw()
{
	return Find(key);
}

template< typename K, typename V, class KTraits, class VTraits >
bool RBMap< K, V, KTraits, VTraits >::Lookup( KINARGTYPE key, VOUTARGTYPE value ) const 
{
	const Pair* pLookup = Find( key );
	if( pLookup == NULL )
		return false;

	value = pLookup->_value;
	return true;
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION RBMap< K, V, KTraits, VTraits >::SetAt( KINARGTYPE key, VINARGTYPE value ) 
{
	Pair* pNode = Find( key );
	if( pNode == NULL )
	{
		return( RBInsert( key, value ) );
	}
	else
	{
		pNode->_value = value;

		return( pNode );
	}
}

template< typename K, typename V, class KTraits, class VTraits >
bool RBMap< K, V, KTraits, VTraits >::RemoveKey( KINARGTYPE key ) throw()
{
	POSITION pos = Lookup( key );
	if( pos != NULL )
	{
        RBTREE::RemoveAt( pos );

		return( true );
	}
	else
	{
		return( false );
	}
}

template< typename K, typename V, class KTraits = ElementTraits< K >, class VTraits = ElementTraits< V > >
class RBMultiMap :
	public RBTree< K, V, KTraits, VTraits >
{
public:
    typedef RBTree< K, V, KTraits, VTraits > RBTREE;
    typedef typename RBTREE::KINARGTYPE KINARGTYPE;
    typedef typename RBTREE::KOUTARGTYPE KOUTARGTYPE;
    typedef typename RBTREE::VINARGTYPE VINARGTYPE;
    typedef typename RBTREE::VOUTARGTYPE VOUTARGTYPE;
    typedef typename RBTREE::Pair Pair;

    explicit RBMultiMap( size_t nBlockSize = 10 ) throw();
	~RBMultiMap() throw();

	POSITION Insert( KINARGTYPE key, VINARGTYPE value ) ;
	size_t RemoveKey( KINARGTYPE key ) throw();

	POSITION FindFirstWithKey( KINARGTYPE key ) const throw();
	const Pair* GetNextWithKey( POSITION& pos, KINARGTYPE key ) const throw();
	Pair* GetNextWithKey( POSITION& pos, KINARGTYPE key ) throw();
	const V& GetNextValueWithKey( POSITION& pos, KINARGTYPE key ) const throw();
	V& GetNextValueWithKey( POSITION& pos, KINARGTYPE key ) throw();
};

template< typename K, typename V, class KTraits, class VTraits >
RBMultiMap< K, V, KTraits, VTraits >::RBMultiMap( size_t nBlockSize ) throw() :
RBTree< K, V, KTraits, VTraits >( nBlockSize )
{
}

template< typename K, typename V, class KTraits, class VTraits >
RBMultiMap< K, V, KTraits, VTraits >::~RBMultiMap() throw()
{
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION RBMultiMap< K, V, KTraits, VTraits >::Insert( KINARGTYPE key, VINARGTYPE value )
{
	return( RBInsert( key, value ) );
}

template< typename K, typename V, class KTraits, class VTraits >
size_t RBMultiMap< K, V, KTraits, VTraits >::RemoveKey( KINARGTYPE key ) throw()
{
	size_t nElementsDeleted = 0;

	POSITION pos = FindFirstWithKey( key );
	while( pos != NULL )
	{
		POSITION posDelete = pos;
		GetNextWithKey( pos, key );
        RBTREE::RemoveAt( posDelete );
		nElementsDeleted++;
	}

	return( nElementsDeleted );
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION RBMultiMap< K, V, KTraits, VTraits >::FindFirstWithKey( KINARGTYPE key ) const throw()
{
	return( Find( key ) );
}

template< typename K, typename V, class KTraits, class VTraits >
const typename RBMultiMap< K, V, KTraits, VTraits >::Pair* RBMultiMap< K, V, KTraits, VTraits >::GetNextWithKey( POSITION& pos, KINARGTYPE key ) const throw()
{
	FTLASSERT( pos != NULL );

    const Pair* pNode = RBTREE::GetNext( pos );
	if( (pos == NULL) || !KTraits::CompareElements( static_cast< Pair* >( pos )->_key, key ) )
	{
		pos = NULL;
	}

	return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
typename RBMultiMap< K, V, KTraits, VTraits >::Pair* RBMultiMap< K, V, KTraits, VTraits >::GetNextWithKey( POSITION& pos, KINARGTYPE key ) throw()
{
	FTLASSERT( pos != NULL );

    Pair* pNode = RBTREE::GetNext( pos );
	if( (pos == NULL) || !KTraits::CompareElements( static_cast< Pair* >( pos )->_key, key ) )
	{
		pos = NULL;
	}

	return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
const V& RBMultiMap< K, V, KTraits, VTraits >::GetNextValueWithKey( POSITION& pos, KINARGTYPE key ) const throw()
{
	const Pair* pPair = GetNextWithKey( pos, key );

	return( pPair->_value );
}

template< typename K, typename V, class KTraits, class VTraits >
V& RBMultiMap< K, V, KTraits, VTraits >::GetNextValueWithKey( POSITION& pos, KINARGTYPE key ) throw()
{
	Pair* pPair = GetNextWithKey( pos, key );

	return( pPair->_value );
}

#pragma pop_macro("new")


} // namespace atl

#endif // FTLCOLLECTIONS_HPP
