# include "common_header.h"

# include "codex_srv_impl.h"
# include "codex_archive_impl.h"
# include "codex_cpl_impl.h"

volatile LONG g_nRefCounter = 0;

class CCodexSrvClassFactory :
  IMPLEMENTS( IUnknown ),
  IMPLEMENTS( IClassFactory ),
  public CRefCounted
{

public:

  virtual HRESULT
	IUnknown_QueryInterface(
	  __in  const IID &  i_poReqIfaceId,
	  __out       void** o_ppoIface )
	{
		if( ! VERIFY( o_ppoIface ) )
		{
			return E_INVALIDARG;
		}

		* o_ppoIface = 0;

		if( __uuidof( IClassFactory ) == i_poReqIfaceId )
		{
			* o_ppoIface
        = static_cast< IClassFactory* >(
            static_cast< IClassFactory_wrap* >( this ) );
		}
		else if( __uuidof( IUnknown ) == i_poReqIfaceId )
		{
			* o_ppoIface
        = static_cast< IUnknown* >(
            static_cast< IUnknown_wrap* >( this ) );
		}
		else
		{
			return E_NOINTERFACE;
		}

    IUnknown_AddRef();

		return S_OK;
	}

  virtual ULONG
  IUnknown_AddRef( void )
  {
    return CRefCounted_AddRef();
  }

  virtual ULONG
  IUnknown_Release( void )
  {
    //::MessageBoxW( 0, L"Class factory release", L"CodexSrv", 0 );
    return CRefCounted_Release();
  }

  virtual HRESULT
  IClassFactory_CreateInstance(
    __in        IUnknown* i_poIfaceOuter,
    __in  const IID &     i_poReqIfaceId,
    __out       void**    o_ppoIface )
  {
    //::MessageBoxW( 0, L"Create Instance called", L"CodexSrv", 0 );

	  if( ! VERIFY( o_ppoIface ) )
	  {
		  return E_INVALIDARG;
	  }

	  if( i_poIfaceOuter )
	  {
		  return CLASS_E_NOAGGREGATION;
	  }

	  * o_ppoIface = 0;

	  CodexSrv* poCodexSrv = new CodexSrv();
	  if( ! VERIFY( poCodexSrv ) )
	  {
		  return E_OUTOFMEMORY;
	  }

    IUnknown* poIUnknown
      = static_cast< IUnknown* >(
          static_cast< IUnknown_wrap* >( poCodexSrv ) );

	  HRESULT hRes = poIUnknown->QueryInterface(
      i_poReqIfaceId, o_ppoIface );

	  //VERIFY( S_OK == hRes );

	  poIUnknown->Release();

	  return hRes;
  }
        
  virtual HRESULT
  IClassFactory_LockServer( BOOL i_fnLock )
  {
    if( i_fnLock )
    {
      ::InterlockedIncrement( & g_nRefCounter );
    }
    else
    {
      //::MessageBoxW( 0, L"Unlock called", L"CodexSrv", 0 );

      if( ! ::InterlockedDecrement( & g_nRefCounter ) )
      {
        ::PostQuitMessage( 0 );
      }
    }

	  return S_OK;
  }
};

CCodexSrvClassFactory* g_poCodexSrvClassFactory = 0;

extern BOOL RegUnregOrRun( CHAR* i_pczCmdLine, BOOL* o_pfnRun );

template< class T, size_t N >
struct ArrayAnyToChar_StubBB
{
  static T* NN() { return (T*)N; }
};

// template< class T, T* N >
// char ( & ArrayAnyToChar_StubB( ( & )N ) )[ 5 ];

template< class T, size_t N >
ArrayAnyToChar_StubBB< T, N > ( & ArrayAnyToChar_StubB( T(&)[ N ] ))[ N ];
/*
int ( & someFn( void ) )[ 5 ]
{
  int arr[ 5 ];
  return arr;
}*/

template< size_t N >
void foo( int ( & arr )[ N ] )
{
}

struct SIntegralCTag
{
  static const int value = 0;
};

template< int N >
struct SInt
{
  static const int value = N;

  typedef SInt type;

  typedef int value_type;
  typedef SIntegralCTag tag;

  typedef SInt< static_cast< int >( value + 1 ) > next;
  typedef SInt< static_cast< int >( value - 1 ) > prior;

  operator int() const
  {
    return static_cast< int >( this->value );
  } 
};

template< int N >
int const SInt< N >::value;

template< bool C >
struct SBool
{
    static const bool value = C;
    typedef SIntegralCTag tag;
    typedef SBool type;
    typedef bool value_type;

    operator bool( void ) const
    {
      return this->value;
    }
};

typedef SBool< true > STrue;
typedef SBool< false > SFalse;

template< typename T, T N >
struct SIntegralC;

template < class T, T val >
struct SIntegralConstant :
  public SIntegralC< T, val >
{
  typedef SIntegralConstant< T, val > type;
};

template<>
struct SIntegralConstant< bool, true > :
  public STrue
{
  typedef SIntegralConstant< bool, true > type;
};

template<>
struct SIntegralConstant< bool, false > :
  public SFalse 
{
  typedef SIntegralConstant< bool, false > type;
};

typedef SIntegralConstant< bool, true > STruetype;
typedef SIntegralConstant< bool, false > SFalsetype;


// Function Ptr Testers

typedef char YesType;

struct NoType
{
   char padding[ 8 ];
};

// Note it is acceptible to use ellipsis here,
// since the argument will always be a pointer type of some sort:
NoType __cdecl IsFuncPtrTester( ... );

template < class R >
YesType IsFuncPtrTester( R (*)( void ) );

#ifndef IS_FUNC_TEST_NO_CHECK_ELLIPSIS
template <class R >
YesType IsFuncPtrTester(R (*)( ...));
#endif

#ifdef IS_FUNC_TEST_MS_SIGS
  template <class R >
  YesType IsFuncPtrTester(R (__stdcall*)());
  template <class R >
  YesType IsFuncPtrTester(R (__stdcall*)( ...));

  template <class R >
  YesType IsFuncPtrTester(R (__fastcall*)());
  template <class R >
  YesType IsFuncPtrTester(R (__fastcall*)( ...));

  template <class R >
  YesType IsFuncPtrTester(R (__cdecl*)());
  template <class R >
  YesType IsFuncPtrTester(R (__cdecl*)( ...));
#endif

template <class R , class T0 >
YesType IsFuncPtrTester(R (*)( T0));

#ifndef IS_FUNC_TEST_NO_CHECK_ELLIPSIS
  template <class R , class T0 >
  YesType IsFuncPtrTester(R (*)( T0 ...));
#endif

#ifdef IS_FUNC_TEST_MS_SIGS
  template <class R , class T0 >
  YesType IsFuncPtrTester(R (__stdcall*)( T0));
  template <class R , class T0 >
  YesType IsFuncPtrTester(R (__stdcall*)( T0 ...));

  template <class R , class T0 >
  YesType IsFuncPtrTester(R (__fastcall*)( T0));
  template <class R , class T0 >
  YesType IsFuncPtrTester(R (__fastcall*)( T0 ...));

  template <class R , class T0 >
  YesType IsFuncPtrTester(R (__cdecl*)( T0));
  template <class R , class T0 >
  YesType IsFuncPtrTester(R (__cdecl*)( T0 ...));

#endif

template <class R , class T0 , class T1 >
YesType IsFuncPtrTester(R (*)( T0 , T1));

#ifndef IS_FUNC_TEST_NO_CHECK_ELLIPSIS
template <class R , class T0 , class T1 >
YesType IsFuncPtrTester(R (*)( T0 , T1 ...));
#endif

#ifdef IS_FUNC_TEST_MS_SIGS
template <class R , class T0 , class T1 >
YesType IsFuncPtrTester(R (__stdcall*)( T0 , T1));
template <class R , class T0 , class T1 >
YesType IsFuncPtrTester(R (__stdcall*)( T0 , T1 ...));

template <class R , class T0 , class T1 >
YesType IsFuncPtrTester(R (__fastcall*)( T0 , T1));
template <class R , class T0 , class T1 >
YesType IsFuncPtrTester(R (__fastcall*)( T0 , T1 ...));

template <class R , class T0 , class T1 >
YesType IsFuncPtrTester(R (__cdecl*)( T0 , T1));
template <class R , class T0 , class T1 >
YesType IsFuncPtrTester(R (__cdecl*)( T0 , T1 ...));
#endif

template <class R , class T0 , class T1 , class T2 >
YesType IsFuncPtrTester(R (*)( T0 , T1 , T2));

#ifndef IS_FUNC_TEST_NO_CHECK_ELLIPSIS
template <class R , class T0 , class T1 , class T2 >
YesType IsFuncPtrTester(R (*)( T0 , T1 , T2 ...));
#endif

#ifdef IS_FUNC_TEST_MS_SIGS
template <class R , class T0 , class T1 , class T2 >
YesType IsFuncPtrTester(R (__stdcall*)( T0 , T1 , T2));
template <class R , class T0 , class T1 , class T2 >
YesType IsFuncPtrTester(R (__stdcall*)( T0 , T1 , T2 ...));

template <class R , class T0 , class T1 , class T2 >
YesType IsFuncPtrTester(R (__fastcall*)( T0 , T1 , T2));
template <class R , class T0 , class T1 , class T2 >
YesType IsFuncPtrTester(R (__fastcall*)( T0 , T1 , T2 ...));

template <class R , class T0 , class T1 , class T2 >
YesType IsFuncPtrTester(R (__cdecl*)( T0 , T1 , T2));
template <class R , class T0 , class T1 , class T2 >
YesType IsFuncPtrTester(R (__cdecl*)( T0 , T1 , T2 ...));
#endif

template <class R , class T0 , class T1 , class T2 , class T3 >
YesType IsFuncPtrTester(R (*)( T0 , T1 , T2 , T3));

#ifndef IS_FUNC_TEST_NO_CHECK_ELLIPSIS
template <class R , class T0 , class T1 , class T2 , class T3 >
YesType IsFuncPtrTester(R (*)( T0 , T1 , T2 , T3 ...));
#endif

#ifdef IS_FUNC_TEST_MS_SIGS
template <class R , class T0 , class T1 , class T2 , class T3 >
YesType IsFuncPtrTester(R (__stdcall*)( T0 , T1 , T2 , T3));
template <class R , class T0 , class T1 , class T2 , class T3 >
YesType IsFuncPtrTester(R (__stdcall*)( T0 , T1 , T2 , T3 ...));

template <class R , class T0 , class T1 , class T2 , class T3 >
YesType IsFuncPtrTester(R (__fastcall*)( T0 , T1 , T2 , T3));
template <class R , class T0 , class T1 , class T2 , class T3 >
YesType IsFuncPtrTester(R (__fastcall*)( T0 , T1 , T2 , T3 ...));

template <class R , class T0 , class T1 , class T2 , class T3 >
YesType IsFuncPtrTester(R (__cdecl*)( T0 , T1 , T2 , T3));
template <class R , class T0 , class T1 , class T2 , class T3 >
YesType IsFuncPtrTester(R (__cdecl*)( T0 , T1 , T2 , T3 ...));
#endif

template <class R , class T0 , class T1 , class T2 , class T3 , class T4 >
YesType IsFuncPtrTester(R (*)( T0 , T1 , T2 , T3 , T4));

#ifndef IS_FUNC_TEST_NO_CHECK_ELLIPSIS
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 >
YesType IsFuncPtrTester(R (*)( T0 , T1 , T2 , T3 , T4 ...));
#endif

#ifdef IS_FUNC_TEST_MS_SIGS
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 >
YesType IsFuncPtrTester(R (__stdcall*)( T0 , T1 , T2 , T3 , T4));
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 >
YesType IsFuncPtrTester(R (__stdcall*)( T0 , T1 , T2 , T3 , T4 ...));

template <class R , class T0 , class T1 , class T2 , class T3 , class T4 >
YesType IsFuncPtrTester(R (__fastcall*)( T0 , T1 , T2 , T3 , T4));
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 >
YesType IsFuncPtrTester(R (__fastcall*)( T0 , T1 , T2 , T3 , T4 ...));

template <class R , class T0 , class T1 , class T2 , class T3 , class T4 >
YesType IsFuncPtrTester(R (__cdecl*)( T0 , T1 , T2 , T3 , T4));
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 >
YesType IsFuncPtrTester(R (__cdecl*)( T0 , T1 , T2 , T3 , T4 ...));
#endif

template< class R, class T0, class T1, class T2, class T3, class T4, class T5 >
YesType IsFuncPtrTester( R (*)( T0 , T1 , T2 , T3 , T4 , T5 ) );

#ifndef IS_FUNC_TEST_NO_CHECK_ELLIPSIS
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 >
YesType IsFuncPtrTester(R (*)( T0 , T1 , T2 , T3 , T4 , T5 ...));
#endif

#ifdef IS_FUNC_TEST_MS_SIGS
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 >
YesType IsFuncPtrTester(R (__stdcall*)( T0 , T1 , T2 , T3 , T4 , T5));
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 >
YesType IsFuncPtrTester(R (__stdcall*)( T0 , T1 , T2 , T3 , T4 , T5 ...));

template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 >
YesType IsFuncPtrTester(R (__fastcall*)( T0 , T1 , T2 , T3 , T4 , T5));
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 >
YesType IsFuncPtrTester(R (__fastcall*)( T0 , T1 , T2 , T3 , T4 , T5 ...));

template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 >
YesType IsFuncPtrTester(R (__cdecl*)( T0 , T1 , T2 , T3 , T4 , T5));
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 >
YesType IsFuncPtrTester(R (__cdecl*)( T0 , T1 , T2 , T3 , T4 , T5 ...));
#endif

// SIsFunc

template < typename T >
struct SIsFuncImpl
{
#pragma warning( push )
#pragma warning( disable: 6334 )
  static T* t;

  static const bool
    value = sizeof( IsFuncPtrTester( t ) ) == sizeof( YesType );
#pragma warning( pop )
};

template < typename T >
struct SIsFuncImpl< T& > :
  public SFalsetype
{
};

template< typename T >
struct SIsFunc :
  SIntegralConstant< bool, SIsFuncImpl< T >::value >
{
};

// Counter

template< int N >
struct STheCounter;

template< typename T, int N = 5 >
struct SEncodeCounter
{
  __if_exists( STheCounter< N + 256 > )
  {
    static const unsigned count = SEncodeCounter< T, N + 257 >::count;
  }

  __if_not_exists( STheCounter< N + 256 > )
  {
    __if_exists( STheCounter< N + 64 > )
    {
      static const unsigned count = SEncodeCounter< T, N + 65 >::count;
    }
    __if_not_exists( STheCounter< N + 64 > )
    {
      __if_exists( STheCounter< N + 16 > )
      {
        static const unsigned count = SEncodeCounter< T, N + 17 >::count;
      }
      __if_not_exists( STheCounter< N + 16 > )
      {
        __if_exists( STheCounter< N + 4 > )
        {
          static const unsigned count = SEncodeCounter< T, N + 5 >::count;
        }
        __if_not_exists( STheCounter< N + 4 > )
        {
          __if_exists( STheCounter< N > )
          {
              static const unsigned
                count = SEncodeCounter< T, N + 1 >::count;
          }
          __if_not_exists( STheCounter< N > )
          {
              static const unsigned count = N;
              typedef STheCounter< N > type;
          }
        }
      }
    }
  }
};


// Extract type

struct SExtractTypeDefParam
{
};

template< typename ID, typename T = SExtractTypeDefParam >
struct SExtractType;

template< typename ID >
struct SExtractType< ID, SExtractTypeDefParam >
{
  template< bool >
  struct SId2TypeImpl;

  typedef SId2TypeImpl< true > SId2Type;
};

template< typename ID, typename T >
struct SExtractType :
  SExtractType< ID, SExtractTypeDefParam >
{
  template<>
  struct SId2TypeImpl< true >  //VC8.0 specific bugfeature
  {
    typedef T type;
  };

  template< bool >
  struct SId2TypeImpl;

  typedef SId2TypeImpl< true > SId2Type;
};


// Register type

template< typename T, typename ID >
struct SRegisterType : SExtractType< ID, T >
{
};


//Tie it all together
template< typename T >
struct SEncodeType
{
  //Get the next available compile time constants index
  static const unsigned value = SEncodeCounter< T >::count;

  //Instantiate the template
  typedef typename SRegisterType< T, SInt< value > >::SId2Type type;

  //Set the next compile time constants index
  static const unsigned next = value + 1;
};

// SSizer

template< class T >
struct SSizer
{
  typedef char( * type )[ SEncodeType< T >::value ];
};


// EncodeStart( T const & )

template < bool B, class T = void >
struct SDisableIfC
{
  typedef T type;
};

template < class T >
struct SDisableIfC< true, T >
{
};

template < class Cond, class T = void > 
struct SDisableIf :
  public SDisableIfC< Cond::value, T >
{
};

template< typename T >
typename SDisableIf< typename SIsFunc< T >::type,
                     typename SSizer< T >::type
                   >::type
EncodeStart( T const & );


// EncodeStart( T & )

template < bool B, class T = void >
struct SEnableIfC
{
  typedef T type;
};

template < class T >
struct SEnableIfC< false, T >
{
};

template < class Cond, class T = void >
struct SEnableIf :
  public SEnableIfC< Cond::value, T >
{
};

template< typename T >
typename SEnableIf< typename SIsFunc< T >::type,
                    typename SSizer< T >::type
                   >::type
EncodeStart( T & );


// TypeID Wrapper

template< int ID >
struct STypeIdWrapper
{
  typedef typename SExtractType< SInt< ID > >::SId2Type SId2Type;
  
  typedef typename SId2Type::type Type;
};

// (Workaround for ETI-bug for VC6 and VC7)
template<>
struct STypeIdWrapper< 1 >
{
  typedef STypeIdWrapper< 1 > Type;
};

// (Workaround for ETI-bug for VC7.1)
template<>
struct STypeIdWrapper< 4 >
{
  typedef STypeIdWrapper< 4 > Type;
};

// Main Macro

#define VAR2TYPE( expr ) \
  STypeIdWrapper< sizeof( * EncodeStart( expr ) ) >::Type




struct NIL { };

template< typename TFun >
struct FuncParser;

template<>
struct FuncParser< void WINAPI(void) >
{
	static const int ParamsCount = 0;
	static const int StackSize = 0;
};

template< typename TRet >
struct FuncParser< typename TRet WINAPI(void) >
{
	static const int ParamsCount = 0;
	struct SSizeTest
	{
		TRet ret;
	};
	static const int StackSize = sizeof( SSizeTest );
};

template< typename TRet, typename TArg0 >
struct FuncParser< typename TRet WINAPI( typename TArg0 ) >
{
	static const int ParamsCount = 1;
	struct SSizeTest
	{
		TRet ret;
		TArg0 arg0;
	};
	static const int StackSize = sizeof( SSizeTest );
};

template< typename TRet, typename TArg0, typename TArg1 >
struct FuncParser< typename TRet WINAPI( typename TArg0, typename TArg1 ) >
{
	static const int ParamsCount = 2;
	struct SSizeTest
	{
		TRet ret;
		TArg0 arg0;
		TArg1 arg1;
	};
	static const int StackSize = sizeof( SSizeTest );
};

template< typename TRet, typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5 >
struct FuncParser< typename TRet WINAPI( typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5 ) >
{
	static const int ParamsCount = 6;
	struct SSizeTest
	{
		TRet ret;
		TArg0 arg0;
		TArg1 arg1;
		TArg2 arg2;
		TArg3 arg3;
		TArg4 arg4;
		TArg5 arg5;
	};
	static const int StackSize = sizeof( SSizeTest );
};

template< typename TRet, typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6 >
struct FuncParser< typename TRet WINAPI( typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6 ) >
{
	static const int ParamsCount = 7;
	struct SSizeTest
	{
		TRet ret;
		TArg0 arg0;
		TArg1 arg1;
		TArg2 arg2;
		TArg3 arg3;
		TArg4 arg4;
		TArg5 arg5;
		TArg6 arg6;
	};
	static const int StackSize = sizeof( SSizeTest );
};


int WINAPI foo( int a )
{
  return 5;
}

VAR2TYPE( foo ) bar;

int WINAPI bar( int a )
{
  return 7;
}

INT WINAPI WinMain(
  __in HINSTANCE i_hInst,
  __in_opt HINSTANCE,
  __in_opt CHAR* i_pczCmdLine,
  __in int )
{
  //void* abc[ 5 ] = { 0 };

  //void** n = ArrayAnyToChar_StubB( abc )[ 0 ].NN();

  int abc[ 5 ] = { 0 };
  // foo( abc );

  // someFn()[ 0 ] = 0;

  VAR2TYPE( abc ) def = { 0 };

  size_t argscnt = FuncParser< VAR2TYPE( foo ) >::ParamsCount;
  size_t stacksize = FuncParser< VAR2TYPE( foo ) >::StackSize;

  VAR2TYPE( foo )* fooPtr = foo;
  fooPtr( 5 );

  // STypeIdWrapper< sizeof( * EncodeStart( expr ) ) >::Type;

  UNREFERENCED_PARAMETER( i_hInst );

  BOOL fnStartServer = FALSE;
  if( ! VERIFY( RegUnregOrRun( i_pczCmdLine, & fnStartServer ) ) )
  {
    return E_UNEXPECTED;
  }

  if( ! fnStartServer )
  {
    return S_OK;
  }

  //::MessageBoxW( 0, L"Attach to me!", L"CodexSrv", 0 );

  HRESULT hRes = CoInitializeEx( 0, COINIT_APARTMENTTHREADED );
  if( VERIFY( S_OK == hRes ) )
  {
    g_poCodexSrvClassFactory = new CCodexSrvClassFactory();
    if( VERIFY( g_poCodexSrvClassFactory ) )
    {
      DWORD nClassObjId = 0;
      hRes = ::CoRegisterClassObject(
        __uuidof( CodexSrv ),
        static_cast< IUnknown_wrap* >( g_poCodexSrvClassFactory ),
        CLSCTX_LOCAL_SERVER,
        REGCLS_SINGLEUSE,
        & nClassObjId );

      if( VERIFY( S_OK == hRes ) )
      {
        MSG oMsg = { 0 };
        while( ::GetMessageW( & oMsg, 0, 0, 0 ) )
        {
          ::DispatchMessageW( & oMsg );
        }

        hRes = ::CoRevokeClassObject( nClassObjId );
        VERIFY( S_OK == hRes );
      }
      else
      {
        ::MessageBoxW( 0, L"CoRegisterClassObject failed", L"CodexSrv", 0 );
      }

      g_poCodexSrvClassFactory->IUnknown_Release();
      g_poCodexSrvClassFactory = 0;
    }
    else
    {
      hRes = E_OUTOFMEMORY;
    }

    ::CoUninitialize();
  }
  
  return hRes;
}
