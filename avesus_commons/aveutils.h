//////////////////////////////////////////////////////////////////////////
//                         Avesus Common Headers
//         Copyright (c) Ivan Borisenko 2009-2010 All Rights reserved.
//
// This file and its contents are protected by International copyright 
// laws. Unauthorized reproduction and/or distribution of all or any 
// portion of the code contained herein is strictly prohibited and will 
// result in severe civil and criminal penalties. Any violations of this 
// copyright will be prosecuted to the fullest extent possible under law.
//                                                                        
// THE SOURCE CODE CONTAINED HEREIN AND IN RELATED FILES IS THE PROPERTY  
// OF IVAN BORISENKO (HEREIN AND AFTER "SOFTWARE DEVELOPER").
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS 
// AND CONDITIONS OUTLINED IN THE SOURCE CODE LICENSE AGREEMENT EXECUTED 
// BETWEEN THE SOFTWARE DEVELOPER AND YOU.
//  
// UNDER NO CIRCUMSTANCES MAY ANY PART OF THE SOURCE CODE BE
// DISTRIBUTED, DISCLOSED OR OTHERWISE MADE AVAILABLE TO ANY THIRD PARTY 
// WITHOUT THE EXPRESS WRITTEN CONSENT OF THE SOFTWARE DEVELOPER.
//
// USER(S) OF THIS SOURCE CODE ACKNOWLEDGES THAT THIS SOURCE CODE
// CONTAINS VALUABLE AND PROPRIETARY TRADE SECRETS OF THE SOFTWARE DEVELOPER.
// USER(S) OF THIS SOURCE CODE AGREES TO MAKE EVERY EFFORT TO INSURE
// ITS CONFIDENTIALITY.
//
// THIS COPYRIGHT NOTICE MAY NOT BE REMOVED FROM THIS FILE.
//////////////////////////////////////////////////////////////////////////

#pragma once

template< class T, size_t N >
char ( & ArrayAnyToChar_Stub( T( & )[ N ] ) )[ N ];

#define ALEN( a ) ( sizeof( ArrayAnyToChar_Stub( a ) ) )


#define UNUSED( arg ) (arg);

# define BYTELEN( a ) (ALEN( a ) * sizeof( * a ))

class CRegistryGuid
{
public:

	CRegistryGuid()
	{
		::memset( m_awzClassGuid, 0, sizeof( m_awzClassGuid ) );
	}

	BOOL Build( const CLSID & i_poClsid )
	{
		ENSURE( ALEN( m_awzClassGuid )
		  ==
		  ::StringFromGUID2(
		    i_poClsid, m_awzClassGuid, ALEN( m_awzClassGuid ) ) );

		ENSURE( m_awzClassGuid[ 0 ] && ! m_awzClassGuid[ 38 ] );

		return TRUE;
	}

	WCHAR* Get( void )
	{
		return m_awzClassGuid;
	}

private:

	WCHAR m_awzClassGuid[ 39 ];
};

class CAveAutoGuard
{
public:

  CAveAutoGuard( CRITICAL_SECTION* i_poCritSect ) :
    m_poCritSect( i_poCritSect )
  {
    ::EnterCriticalSection( m_poCritSect );
  }

  ~CAveAutoGuard()
  {
    ::LeaveCriticalSection( m_poCritSect );
    m_poCritSect = 0;
  }

private:

  CRITICAL_SECTION* m_poCritSect;
};

template< typename T >
class CAutoArray
{
public:

  CAutoArray() :
    m_pArray( 0 ),
    m_nSize( 0 )
  {
  }

  virtual ~CAutoArray()
  {
    Free();
  }

  BOOL Alloc( UINT32 i_nSize )
  {
    Free();

    m_pArray = new T[ i_nSize ];
    if( m_pArray )
    {
      m_nSize = i_nSize;

      ::memset( m_pArray, 0, sizeof( T ) * i_nSize );
      
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }

  T* Ptr( void )
  {
    return m_pArray;
  }

  UINT32 Size( void )
  {
    return m_nSize;
  }

  void Free( void )
  {
    if( m_pArray )
    {
      delete[] m_pArray;
      m_pArray = 0;
      m_nSize = 0;
    }
  }

private:

  T* m_pArray;
  UINT32 m_nSize;
};

//----------------------------------------------------------------------------

class CNull
{
};

//----------------------------------------------------------------------------

template< typename T1 >
struct ExtractTypeFromFunctionSignature
{
  typedef typename T1 TFunc;
};

template< typename T1 >
struct ExtractTypeFromFunctionSignature< T1 (*)( void ) >
{
  typedef typename T1 TRet;

  typedef CNull TArg1;
  typedef CNull TArg2;
  typedef CNull TArg3;
  typedef CNull TArg4;
  typedef CNull TArg5;
  typedef CNull TArg6;
};

# ifndef _WIN64

template< typename T1 >
struct ExtractTypeFromFunctionSignature< T1 (__stdcall*)( void ) >
{
  typedef typename T1 TRet;

  typedef CNull TArg1;
  typedef CNull TArg2;
  typedef CNull TArg3;
  typedef CNull TArg4;
  typedef CNull TArg5;
  typedef CNull TArg6;
};

# endif

template< typename T1, typename T2 >
struct ExtractTypeFromFunctionSignature< T1 (*)( T2 ) >
{
  typedef typename T1 TRet;

  typedef typename T2 TArg1;
  typedef CNull TArg2;
  typedef CNull TArg3;
  typedef CNull TArg4;
  typedef CNull TArg5;
  typedef CNull TArg6;
};

# ifndef _WIN64

template< typename T1, typename T2 >
struct ExtractTypeFromFunctionSignature< T1 (__stdcall*)( T2 ) >
{
  typedef typename T1 TRet;

  typedef typename T2 TArg1;
  typedef CNull TArg2;
  typedef CNull TArg3;
  typedef CNull TArg4;
  typedef CNull TArg5;
  typedef CNull TArg6;
};

# endif

template< typename T1, typename T2, typename T3 >
struct ExtractTypeFromFunctionSignature< T1 (*)( T2, T3 ) >
{
  typedef typename T1 TRet;

  typedef typename T2 TArg1;
  typedef typename T3 TArg2;
  typedef CNull TArg3;
  typedef CNull TArg4;
  typedef CNull TArg5;
  typedef CNull TArg6;
};

# ifndef _WIN64

template< typename T1, typename T2, typename T3 >
struct ExtractTypeFromFunctionSignature< T1 (__stdcall*)( T2, T3 ) >
{
  typedef typename T1 TRet;

  typedef typename T2 TArg1;
  typedef typename T3 TArg2;
  typedef CNull TArg3;
  typedef CNull TArg4;
  typedef CNull TArg5;
  typedef CNull TArg6;
};

# endif

template< typename T1, typename T2, typename T3, typename T4 >
struct ExtractTypeFromFunctionSignature< T1 (*)( T2, T3, T4 ) >
{
  typedef typename T1 TRet;

  typedef typename T2 TArg1;
  typedef typename T3 TArg2;
  typedef typename T4 TArg3;
  typedef CNull TArg4;
  typedef CNull TArg5;
  typedef CNull TArg6;
};

# ifndef _WIN64

template< typename T1, typename T2, typename T3, typename T4 >
struct ExtractTypeFromFunctionSignature< T1 (__stdcall*)( T2, T3, T4 ) >
{
  typedef typename T1 TRet;

  typedef typename T2 TArg1;
  typedef typename T3 TArg2;
  typedef typename T4 TArg3;
  typedef CNull TArg4;
  typedef CNull TArg5;
  typedef CNull TArg6;
};

# endif

template< typename T1, typename T2, typename T3, typename T4, typename T5 >
struct ExtractTypeFromFunctionSignature< T1 (*)( T2, T3, T4, T5 ) >
{
  typedef typename T1 TRet;

  typedef typename T2 TArg1;
  typedef typename T3 TArg2;
  typedef typename T4 TArg3;
  typedef typename T5 TArg4;
  typedef CNull TArg5;
  typedef CNull TArg6;
};

# ifndef _WIN64

template< typename T1, typename T2, typename T3, typename T4, typename T5 >
struct ExtractTypeFromFunctionSignature< T1 (__stdcall*)( T2, T3, T4, T5 ) >
{
  typedef typename T1 TRet;

  typedef typename T2 TArg1;
  typedef typename T3 TArg2;
  typedef typename T4 TArg3;
  typedef typename T5 TArg4;
  typedef CNull TArg5;
  typedef CNull TArg6;
};

# endif

template< typename T1, typename T2, typename T3, typename T4, typename T5,
          typename T6 >
struct ExtractTypeFromFunctionSignature< T1 (*)( T2, T3, T4, T5, T6 ) >
{
  typedef typename T1 TRet;

  typedef typename T2 TArg1;
  typedef typename T3 TArg2;
  typedef typename T4 TArg3;
  typedef typename T5 TArg4;
  typedef typename T6 TArg5;
  typedef CNull TArg6;
};

# ifndef _WIN64

template< typename T1, typename T2, typename T3, typename T4, typename T5,
          typename T6 >
struct ExtractTypeFromFunctionSignature< T1 (__stdcall*)( T2, T3, T4, T5, T6 ) >
{
  typedef typename T1 TRet;

  typedef typename T2 TArg1;
  typedef typename T3 TArg2;
  typedef typename T4 TArg3;
  typedef typename T5 TArg4;
  typedef typename T6 TArg5;
  typedef CNull TArg6;
};

# endif

template< typename T1, typename T2, typename T3, typename T4, typename T5,
          typename T6, typename T7 >
struct ExtractTypeFromFunctionSignature< T1 (*)( T2, T3, T4, T5, T6, T7 ) >
{
  typedef typename T1 TRet;

  typedef typename T2 TArg1;
  typedef typename T3 TArg2;
  typedef typename T4 TArg3;
  typedef typename T5 TArg4;
  typedef typename T6 TArg5;
  typedef typename T7 TArg6;
};

# ifndef _WIN64

template< typename T1, typename T2, typename T3, typename T4, typename T5,
          typename T6, typename T7 >
struct ExtractTypeFromFunctionSignature< T1 (__stdcall*)( T2, T3, T4, T5, T6, T7 ) >
{
  typedef typename T1 TRet;

  typedef typename T2 TArg1;
  typedef typename T3 TArg2;
  typedef typename T4 TArg3;
  typedef typename T5 TArg4;
  typedef typename T6 TArg5;
  typedef typename T7 TArg6;
};

# endif

//----------------------------------------------------------------------------

template< typename T1, typename TC >
struct ExtractTypeFromMethodSignature
{
  typedef typename TC TClass;
  typedef typename T1 TMethod;
  typedef CNull TArg1;
  typedef CNull TArg2;
  typedef CNull TArg3;
  typedef CNull TArg4;
  typedef CNull TArg5;
  typedef CNull TArg6;
};

template< typename T1, typename TC >
struct ExtractTypeFromMethodSignature< T1 (TC::*)( void ), TC >
{
  typedef typename TC TClass;
  typedef typename T1 TRet;
  typedef CNull TArg1;
  typedef CNull TArg2;
  typedef CNull TArg3;
  typedef CNull TArg4;
  typedef CNull TArg5;
  typedef CNull TArg6;
};

template< typename T1, typename T2, typename TC >
struct ExtractTypeFromMethodSignature< T1 (TC::*)( T2 ), TC >
{
  typedef typename TC TClass;
  typedef typename T1 TRet;
  typedef typename T2 TArg1;
  typedef CNull TArg2;
  typedef CNull TArg3;
  typedef CNull TArg4;
  typedef CNull TArg5;
  typedef CNull TArg6;
};

template< typename T1, typename T2, typename T3, typename TC >
struct ExtractTypeFromMethodSignature< T1 (TC::*)( T2, T3 ), TC >
{
  typedef typename TC TClass;
  typedef typename T1 TRet;
  typedef typename T2 TArg1;
  typedef typename T3 TArg2;
  typedef CNull TArg3;
  typedef CNull TArg4;
  typedef CNull TArg5;
  typedef CNull TArg6;
};

template< typename T1, typename T2, typename T3, typename T4, typename TC >
struct ExtractTypeFromMethodSignature< T1 (TC::*)( T2, T3, T4 ), TC >
{
  typedef typename TC TClass;
  typedef typename T1 TRet;
  typedef typename T2 TArg1;
  typedef typename T3 TArg2;
  typedef typename T4 TArg3;
  typedef CNull TArg4;
  typedef CNull TArg5;
  typedef CNull TArg6;
};

template< typename T1, typename T2, typename T3, typename T4, typename T5,
          typename TC >
struct ExtractTypeFromMethodSignature< T1 (TC::*)( T2, T3, T4, T5 ), TC >
{
  typedef typename TC TClass;
  typedef typename T1 TRet;
  typedef typename T2 TArg1;
  typedef typename T3 TArg2;
  typedef typename T4 TArg3;
  typedef typename T5 TArg4;
  typedef CNull TArg5;
  typedef CNull TArg6;
};

template< typename T1, typename T2, typename T3, typename T4, typename T5,
          typename T6, typename TC >
struct ExtractTypeFromMethodSignature< T1 (TC::*)( T2, T3, T4, T5, T6 ), TC >
{
  typedef typename TC TClass;
  typedef typename T1 TRet;
  typedef typename T2 TArg1;
  typedef typename T3 TArg2;
  typedef typename T4 TArg3;
  typedef typename T5 TArg4;
  typedef typename T6 TArg5;
  typedef CNull TArg6;
};

template< typename T1, typename T2, typename T3, typename T4, typename T5,
          typename T6, typename T7, typename TC >
struct ExtractTypeFromMethodSignature< T1 (TC::*)( T2, T3, T4, T5, T6, T7 ), TC >
{
  typedef typename TC TClass;
  typedef typename T1 TRet;
  typedef typename T2 TArg1;
  typedef typename T3 TArg2;
  typedef typename T4 TArg3;
  typedef typename T5 TArg4;
  typedef typename T6 TArg5;
  typedef typename T7 TArg6;
};

//----------------------------------------------------------------------------

template< typename TRet, typename TArg1, typename TArg2, typename TArg3,
          typename TArg4, typename TArg5, typename TArg6, typename TClass >
struct SelectMethodType
{
  typedef TRet (*TFunc)( TArg1, TArg2, TArg3, TArg4, TArg5, TArg6 );
  typedef TRet (TClass::*Type)( TArg1, TArg2, TArg3, TArg4, TArg5, TArg6 );
};

template< typename TRet, typename TArg1, typename TArg2, typename TArg3,
          typename TArg4, typename TArg5, typename TClass >
struct SelectMethodType< TRet, TArg1, TArg2, TArg3, TArg4, TArg5, CNull, TClass >
{
  typedef TRet (*TFunc)( TArg1, TArg2, TArg3, TArg4, TArg5 );
  typedef TRet (TClass::*Type)( TArg1, TArg2, TArg3, TArg4, TArg5 );
};

template< typename TRet, typename TArg1, typename TArg2, typename TArg3,
          typename TArg4, typename TClass >
struct SelectMethodType< TRet, TArg1, TArg2, TArg3, TArg4, CNull, CNull, TClass >
{
  typedef TRet (*TFunc)( TArg1, TArg2, TArg3, TArg4 );
  typedef TRet (TClass::*Type)( TArg1, TArg2, TArg3, TArg4 );
};

template< typename TRet, typename TArg1, typename TArg2, typename TArg3,
          typename TClass >
struct SelectMethodType< TRet, TArg1, TArg2, TArg3, CNull, CNull, CNull, TClass >
{
  typedef TRet (*TFunc)( TArg1, TArg2, TArg3 );
  typedef TRet (TClass::*Type)( TArg1, TArg2, TArg3 );
};

template< typename TRet, typename TArg1, typename TArg2, typename TClass >
struct SelectMethodType< TRet, TArg1, TArg2, CNull, CNull, CNull, CNull, TClass >
{
  typedef TRet (*TFunc)( TArg1, TArg2 );
  typedef TRet (TClass::*Type)( TArg1, TArg2 );
};

template< typename TRet, typename TArg1, typename TClass >
struct SelectMethodType< TRet, TArg1, CNull, CNull, CNull, CNull, CNull, TClass >
{
  typedef TRet (*TFunc)( TArg1 );
  typedef TRet (TClass::*Type)( TArg1 );
};

template< typename TRet, typename TClass >
struct SelectMethodType< TRet, CNull, CNull, CNull, CNull, CNull, CNull, TClass >
{
  typedef TRet (*TFunc)( void );
  typedef TRet (TClass::*Type)( void );
};

//----------------------------------------------------------------------------

template< typename TFunc, typename TClass >
struct TFuncToTMethod
{
  typedef typename ExtractTypeFromFunctionSignature< TFunc >::TRet TRet;
  typedef typename ExtractTypeFromFunctionSignature< TFunc >::TArg1 TArg1;
  typedef typename ExtractTypeFromFunctionSignature< TFunc >::TArg2 TArg2;
  typedef typename ExtractTypeFromFunctionSignature< TFunc >::TArg3 TArg3;
  typedef typename ExtractTypeFromFunctionSignature< TFunc >::TArg4 TArg4;
  typedef typename ExtractTypeFromFunctionSignature< TFunc >::TArg5 TArg5;
  typedef typename ExtractTypeFromFunctionSignature< TFunc >::TArg6 TArg6;

  typedef typename SelectMethodType<
    TRet, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TClass >::Type Type;
};

#define DEFINE_VAR_ARGUMENTED_INTERFACE_SELECTOR( SelectedTemplateBaseName ) \
                                                                             \
  template< typename TRet,  typename TArg1, typename TArg2, typename TArg3,  \
            typename TArg4, typename TArg5, typename TArg6 >                 \
  struct Select##SelectedTemplateBaseName                                    \
  {                                                                          \
    typedef I##SelectedTemplateBaseName##6                                   \
      < TRet, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6 > Type;               \
  };                                                                         \
                                                                             \
  template< typename TRet, typename TArg1, typename TArg2, typename TArg3,   \
            typename TArg4, typename TArg5 >                                 \
  struct Select##SelectedTemplateBaseName                                    \
    < TRet, TArg1, TArg2, TArg3, TArg4, TArg5, CNull >                       \
  {                                                                          \
    typedef I##SelectedTemplateBaseName##5                                   \
      < TRet, TArg1, TArg2, TArg3, TArg4, TArg5 > Type;                      \
  };                                                                         \
                                                                             \
  template< typename TRet, typename TArg1, typename TArg2, typename TArg3,   \
            typename TArg4 >                                                 \
  struct Select##SelectedTemplateBaseName                                    \
    < TRet, TArg1, TArg2, TArg3, TArg4, CNull, CNull >                       \
  {                                                                          \
    typedef I##SelectedTemplateBaseName##4                                   \
      < TRet, TArg1, TArg2, TArg3, TArg4 > Type;                             \
  };                                                                         \
                                                                             \
  template< typename TRet, typename TArg1, typename TArg2, typename TArg3 >  \
  struct Select##SelectedTemplateBaseName                                    \
    < TRet, TArg1, TArg2, TArg3, CNull, CNull, CNull >                       \
  {                                                                          \
    typedef I##SelectedTemplateBaseName##3                                   \
      < TRet, TArg1, TArg2, TArg3 > Type;                                    \
  };                                                                         \
                                                                             \
  template< typename TRet, typename TArg1, typename TArg2 >                  \
  struct Select##SelectedTemplateBaseName                                    \
    < TRet, TArg1, TArg2, CNull, CNull, CNull, CNull >                       \
  {                                                                          \
    typedef I##SelectedTemplateBaseName##2< TRet, TArg1, TArg2 > Type;       \
  };                                                                         \
                                                                             \
  template< typename TRet, typename TArg1 >                                  \
  struct Select##SelectedTemplateBaseName                                    \
    < TRet, TArg1, CNull, CNull, CNull, CNull, CNull >                       \
  {                                                                          \
    typedef I##SelectedTemplateBaseName##1< TRet, TArg1 > Type;              \
  };                                                                         \
                                                                             \
  template< typename TRet >                                                  \
  struct Select##SelectedTemplateBaseName                                    \
    < TRet, CNull, CNull, CNull, CNull, CNull, CNull >                       \
  {                                                                          \
    typedef I##SelectedTemplateBaseName##0< TRet > Type;                     \
  };                                                                         \
                                                                             \
  template< typename TFunc >                                                 \
  struct I##SelectedTemplateBaseName##N                                      \
  {                                                                          \
    typedef typename                                                         \
      ExtractTypeFromFunctionSignature< TFunc > FuncSigParser;               \
                                                                             \
    typedef typename TFunc TFuncType;                                        \
                                                                             \
    typedef typename Select##SelectedTemplateBaseName                        \
    <                                                                        \
     typename FuncSigParser::TRet,                                           \
     typename FuncSigParser::TArg1,                                          \
     typename FuncSigParser::TArg2,                                          \
     typename FuncSigParser::TArg3,                                          \
     typename FuncSigParser::TArg4,                                          \
     typename FuncSigParser::TArg5                                           \
     typename FuncSigParser::TArg6                                           \
    >::Type Type;                                                            \
  }

//----------------------------------------------------------------------------

namespace Private
{
    template< typename T, typename U >
    struct ConversionHelper
    {
        typedef char Small;
        struct Big { char dummy[ 2 ]; };

        static Big   Test( ... );
        static Small Test( U );
        static T     MakeT( void );
    };
}

template< typename T, typename U >
struct Conversion
{
    typedef Private::ConversionHelper< T, U > H;

    enum {
      exists = sizeof( typename H::Small ) == sizeof( H::Test( H::MakeT() ) )
    };
    enum { exists2Way = exists && Conversion< U, T >::exists };
    enum { sameType = FALSE };
};

template< typename T >
struct Conversion< T, T >
{
    enum { exists = 1, exists2Way = 1, sameType = 1 };
};

template < typename T >
struct Conversion< void, T >
{
    enum { exists = 1, exists2Way = 0, sameType = 0 };
};

template < typename T >
struct Conversion< T, void >
{
    enum { exists = 1, exists2Way = 0, sameType = 0 };
};

template <>
struct Conversion< void, void >    
{
    enum { exists = 1, exists2Way = 1, sameType = 1 };
};

#define SUPERSUBCLASS( T, U ) \
    ( ::Conversion< const U*, const T* >::exists && \
    ! ::Conversion< const T*, const void* >::sameType )

template< int >
struct CompileTimeError;

template< >
struct CompileTimeError< TRUE >
{
};

#define STATIC_CHECK( expr, msg ) \
    { ::CompileTimeError< ((expr) != 0) > ERROR_##msg; (void)ERROR_##msg; }


//-------------------------------------------------------------------------

// "convert" ptr to type
template< typename T> T IfPtrPtrToType( T* );
template< typename T> T IfPtrPtrToType( T );

// placement new operator
inline void* operator new( size_t, void* p )
{
  return p;
}

// placement delete operator
inline void operator delete( void*, void* )
{
  return;
}

// argument container
template< typename PT >
class CBox
{

private:

  // argument memory storage
  BYTE abObject[ sizeof( IfPtrPtrToType( (PT) *((PT*)0) ) ) ];

  template< typename T >
  BOOL DoDestroy( T* )
  {
    ((PT)abObject)->~T();
    return TRUE;
  }

  template< typename T >
  BOOL DoDestroy( T )
  {
    return TRUE;
  }

public:

  CBox()
  {
  }

  ~CBox()
  {
    VERIFY( DoDestroy( (PT) *( (PT*)(& abObject) ) ) );
  }

  template< typename T >
  BOOL Assign( T* i_poSrc )
  {
    // calling standard constructor
    T* poDst = new( abObject ) T();

    ENSURE( poDst->CopyFrom( i_poSrc ) );

    return TRUE;
  }

  template< typename T >
  BOOL Get( T** o_pTo )
  {
    ENSURE( o_pTo );

    * o_pTo = (PT)abObject;

    return TRUE;
  }

  template< typename T >
  BOOL Assign( T i_oSrc )
  {
    * ((T*)abObject) = i_oSrc;

    return TRUE;
  }

  template< typename T >
  BOOL Get( T* o_pTo )
  {
    ENSURE( o_pTo );

    * o_pTo = * ((T*)abObject);

    return TRUE;
  }
};

// DLL LOADERS -----------------------------------------------------------

template< typename TFunc >
class CDllFuncPtr
{
  typedef typename ExtractTypeFromFunctionSignature< TFunc >::TRet TRet;
  typedef typename ExtractTypeFromFunctionSignature< TFunc >::TArg1 TArg1;
  typedef typename ExtractTypeFromFunctionSignature< TFunc >::TArg2 TArg2;
  typedef typename ExtractTypeFromFunctionSignature< TFunc >::TArg3 TArg3;
  typedef typename ExtractTypeFromFunctionSignature< TFunc >::TArg4 TArg4;
  typedef typename ExtractTypeFromFunctionSignature< TFunc >::TArg5 TArg5;
  typedef typename ExtractTypeFromFunctionSignature< TFunc >::TArg6 TArg6;

public:

  inline TRet operator()( void )
  {
    return m_xDllFunc();
  }

  inline TRet operator()( TArg1 a1 )
  {
    return m_xDllFunc( a1 );
  }

  inline TRet operator()( TArg1 a1, TArg2 a2  )
  {
    return m_xDllFunc( a1, a2 );
  }

  inline TRet operator()( TArg1 a1, TArg2 a2, TArg3 a3  )
  {
    return m_xDllFunc( a1, a2, a3 );
  }

  inline TRet operator()( TArg1 a1, TArg2 a2, TArg3 a3, TArg4 a4  )
  {
    return m_xDllFunc( a1, a2, a3, a4 );
  }

  inline TRet operator()( TArg1 a1, TArg2 a2, TArg3 a3, TArg4 a4, TArg5 a5  )
  {
    return m_xDllFunc( a1, a2, a3, a4, a5 );
  }

  inline TRet operator()( TArg1 a1, TArg2 a2, TArg3 a3, TArg4 a4, TArg5 a5, TArg6 a6  )
  {
    return m_xDllFunc( a1, a2, a3, a4, a5, a6 );
  }

  BOOL FindPtr( HMODULE i_hDllFrom, const CHAR* i_pczName )
  {
    ENSURE( i_hDllFrom && i_pczName && i_pczName[ 0 ] );

    m_xDllFunc = (TFunc)::GetProcAddress( i_hDllFrom, i_pczName );
    ENSURE( m_xDllFunc );

    return TRUE;
  }

  CDllFuncPtr( void ) :
    m_xDllFunc( 0 )
  {
  }

  ~CDllFuncPtr()
  {
    m_xDllFunc = 0;
  }

private:

  TFunc m_xDllFunc;
};

class CDllWrapper
{
public:

  BOOL Load( const WCHAR* i_pwzDllPath )
  {
    ENSURE( i_pwzDllPath && i_pwzDllPath[ 0 ] );

    if( ! m_hDll )
    {
      HMODULE hDll = ::LoadLibraryW( i_pwzDllPath );
      ENSURE( hDll );

      ENSURE( FillDllFuncPtrs( hDll ) );

      m_hDll = hDll;
      m_nRefCounter = 1;
    }
    else
    {
      ++ m_nRefCounter;
    }
    
    return TRUE;
  }

  BOOL Free( void )
  {
    ENSURE( m_hDll );

    -- m_nRefCounter;

    if( ! m_nRefCounter )
    {
      ENSURE( ::FreeLibrary( m_hDll ) );
      m_hDll = 0;
    }

    return TRUE;
  }

  BOOL IsLoaded( void )
  {
    if( m_hDll )
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }

  CDllWrapper() :
    m_hDll( 0 ),
    m_nRefCounter( 0 )
  {
  }

  virtual ~CDllWrapper()
  {
    if( m_hDll )
    {
      VERIFY( Free() );
    }
  }

private:

  virtual BOOL FillDllFuncPtrs( HMODULE i_hDll ) = 0;

  HMODULE m_hDll;

  UINT32 m_nRefCounter;
};

# define FILL_DLL_FUNC_PTR( FuncName ) ENSURE( FuncName.FindPtr( i_hDll, #FuncName ) )


// Example of declaring common function type methods:
# if 0

// Declare FUNCTION type (not a function pointer!!!)
// This is generic form BOTH for functions and methods!
typedef BOOL FuncAction( const CHAR* i_pcChar, EEvent* o_peEvent );

// Declare FUNCTION pointer type:
typedef FuncAction* FuncActionPtr

// Declare METHOD pointer type
// in the class scope, or in any other place, you can type:
typedef FuncAction TClass::* MethodActionPtr;

// Declare function NAME:
FuncAction MyFunction;

// Declare method name:
FuncAction MyMethod;

// Declare pointer-to member:
FuncAction TClass::* m_xMethodPtr;

// Another way to declare a method pointer:
BOOL (TClass::* m_xMethodPtr2)( CHAR* i_pcChar, EEvent* o_peEvent );

// For implementing such defined methods & funcs,
// it is recommended to have 2 parts:
// one with definition, and another with implementation
// and full-specified signature:
BOOL MyFunction( const CHAR* i_pcChar, EEvent* o_peEvent )
{
  TClass oClass;
  oClass.m_xMethodPtr = & TClass::MyMethod;

  // Call pointer-to-member
  (oClass.*m_xMethodPtr)( i_pcChar, o_peEvent );
  // or
  ( (& oClass)->* (m_xMethodPtr) )( i_pcChar, o_peEvent );
  // and so on...
  

  return TRUE;
}

# endif


template< typename TEventsHandler, typename TEnumStates >
class CAnsiLexicalParser
{

public:

  struct SStateDef
  {
    // ptr to event
    BOOL (TEventsHandler::* m_xEvent)( void );

    TEnumStates m_eNextState;

    // flag for the need of store start of detected string
    BOOL m_fStoreStart;
  };

  CAnsiLexicalParser() :
    m_eState( TEventsHandler::STATE_START ),
    m_pcDetected( 0 ),
    m_pcCurrChar( 0 )
  {
  }

  BOOL Parse( TEventsHandler* i_poEventsHandler,
    SStateDef i_aoStatesTable[ TEventsHandler::STATE_MAX_ID ][ 256 ],
    const CHAR** io_ppcStringToParse, UINT32* io_pnMaxLen )
  {
    ENSURE( io_ppcStringToParse && io_pnMaxLen );
    ENSURE( * io_ppcStringToParse && (* io_ppcStringToParse)[ 0 ] );
    ENSURE( * io_pnMaxLen );

    BOOL fnShouldBreak = FALSE;

    for( m_pcCurrChar = * io_ppcStringToParse;
         m_pcCurrChar < & (* io_ppcStringToParse)[ * io_pnMaxLen ];
         ++ m_pcCurrChar )
    {
      // get current state definition
      SStateDef& pState
        = i_aoStatesTable[ m_eState ][ * ((BYTE*)m_pcCurrChar) ];

      // save string start ptr if need
      if( pState.m_fStoreStart )
      {
        m_pcDetected = m_pcCurrChar;
      }

      // if there is an event handler,
      if( pState.m_xEvent )
      {
        // call event handler
        if( ! ( i_poEventsHandler->* (pState.m_xEvent) )() )
        {
          // break parsing if it returns FALSE;
          fnShouldBreak = TRUE;
        }
      }

      // change state
      m_eState = pState.m_eNextState;

      if( fnShouldBreak
          &&
          TEventsHandler::STATE_START == m_eState )
      {
        ++ m_pcCurrChar;
        break;
      }
    }

    * io_pnMaxLen -= UINT32( m_pcCurrChar - * io_ppcStringToParse );
    * io_ppcStringToParse = m_pcCurrChar;

    if( TEventsHandler::STATE_START != m_eState )
    {
      return i_poEventsHandler->OnIncompleteString();
    }

    return TRUE;
  }

  const CHAR* GetCurrCharPtr( void ) const
  {
    return m_pcCurrChar;
  }

  const CHAR* GetDetectedCharPtr( void ) const
  {
    return m_pcDetected;
  }

  void ResetDetectedCharPtr( void )
  {
    m_pcDetected = 0;
  }

  // ONLY USE FOR A SMART SKIP!!!
  void AdvanceCharPtr( void )
  {
    ++ m_pcCurrChar;
  }

private:

  // detected char ptr
  const CHAR* m_pcDetected;

  // current parsed char ptr
  const CHAR* m_pcCurrChar;

  // current state
  TEnumStates m_eState;
};


class CTimeUtils
{
public:

  static __checkReturn BOOL SetWaitableTimerMsTimeout(
	  __in HANDLE i_hTimer,
	  __in UINT64 i_qTimeoutInMilliseconds,
    __in PTIMERAPCROUTINE i_xApcProc = 0,
    __in void* i_pParamForApc = 0 )
  {
	  // “‡ÈÏ‡ÛÚ, ‚ 100 ÌÒ ËÌÚÂ‚‡Î‡ı.
	  // Œ“–»÷¿“≈À‹ÕŒ≈ ◊»—ÀŒ! (˜ÚÓ·˚ ·˚Î Œ“ÕŒ—»“≈À‹Õ€… Ú‡ÈÏ‡ÛÚ).
	  // 1 ÌÒ = 0.000 000 001 Ò
	  // 1 ÏÒ = 0.001 Ò
	  // 1 ÏÒ = 1 000 000 ÌÒ
	  // 1 ÏÒ = 100 000 ÌÒ * 10
	  // 1 ÏÒ = 10 000 ÌÒ * 100
	  LARGE_INTEGER oRelativeTimeoutIn100nsIntervals;
	  oRelativeTimeoutIn100nsIntervals.QuadPart
		  = -10000LL * i_qTimeoutInMilliseconds;

	  ENSURE( ::SetWaitableTimer( i_hTimer,
		  & oRelativeTimeoutIn100nsIntervals, 0,
      i_xApcProc, i_pParamForApc, FALSE ) );

	  return TRUE;
  }
};


template< class T >
class CAveLinkedList
{
  class CItem : public T
  {
  public:

    CItem() :
      m_poPrev( 0 ),
      m_poNext( 0 )
    {
    }

    virtual ~CItem()
    {
      VERIFY( ! m_poPrev && ! m_poNext );
    }

    CItem* m_poPrev;
    CItem* m_poNext;
  };

public:

  CAveLinkedList() :
    m_poFirst( 0 ),
    m_poLast( 0 )
  {
  }

  virtual ~CAveLinkedList()
  {
    while( m_poFirst )
    {
      CItem* poNext = m_poFirst->m_poNext;
      m_poFirst->m_poPrev = 0;
      m_poFirst->m_poNext = 0;
      delete m_poFirst;
      m_poFirst = poNext;
    }

    m_poLast = 0;
  }

  BOOL IsEmpty( void )
  {
    return ! (m_poFirst && m_poLast) ? TRUE : FALSE;
  }

  // Create unlinked item object
  BOOL Create( T** o_ppoItem )
  {
    ENSURE( o_ppoItem );

    * o_ppoItem = 0;

    CItem* poItem = new CItem();
    ENSURE( poItem );

    * o_ppoItem = poItem;

    return TRUE;
  }

  // Insert new object. Method doesn't check the object uniqueness.
  BOOL Insert( T* i_poObjToInsert )
  {
    CItem* poItem = static_cast< CItem* >( i_poObjToInsert );
    ENSURE( poItem );

    ENSURE( ! poItem->m_poPrev && ! poItem->m_poNext );

    if( ! m_poFirst )
    {
      ENSURE( ! m_poLast );

      m_poFirst = poItem;
    }
    else
    {
      ENSURE( m_poLast );

      m_poLast->m_poNext = poItem;
    }

    poItem->m_poPrev = m_poLast;
    m_poLast = poItem;

    if( m_poFirst && ! m_poLast
        ||
        ! m_poFirst && m_poLast )
    {
      ENSURE( FALSE );
    }

    return TRUE;
  }

  typedef BOOL __stdcall IsEqualsCallback(
    T* i_poCurrCompared, T* i_poComparedWith, BOOL* o_pfnIsEquals );

  BOOL Find( T* i_poCompareWith, IsEqualsCallback* i_xCompareFn,
    T** o_ppoFound )
  {
    ENSURE( i_poCompareWith && o_ppoFound && i_xCompareFn );

    * o_ppoFound = 0;

    BOOL fnEquals = FALSE;

    for( CItem* poItem = m_poFirst;
         poItem;
         poItem = poItem->m_poNext )
    {
      T* poObj = static_cast< T* >( poItem );
      ENSURE( i_xCompareFn( poObj, i_poCompareWith, & fnEquals ) );
      if( fnEquals )
      {
        * o_ppoFound = poObj;
        return TRUE;
      }
    }

    return TRUE;
  }

  // Remove existing object from the list.
  BOOL Remove( T* i_poObjToRemove )
  {
    CItem* poItem = static_cast< CItem* >( i_poObjToRemove );
    ENSURE( poItem );

    if( m_poLast == poItem )
    {
      // if last element in the list removes
      if( m_poLast == m_poFirst )
      {
        m_poFirst = m_poLast = 0;
      }
      else
      {
        m_poLast = m_poLast->m_poPrev;
      }
    }
    else
    {
      if( m_poFirst == poItem )
      {
        m_poFirst = poItem->m_poNext;
      }
    }

    if( poItem->m_poPrev )
    {
      poItem->m_poPrev->m_poNext = poItem->m_poNext;
    }

    if( poItem->m_poNext )
    {
      poItem->m_poNext->m_poPrev = poItem->m_poPrev;
    }

    poItem->m_poPrev = 0;
    poItem->m_poNext = 0;

    if( m_poFirst && ! m_poLast
        ||
        ! m_poFirst && m_poLast )
    {
      ENSURE( FALSE );
    }

    return TRUE;
  }

private:
  
  CItem* m_poFirst;
  CItem* m_poLast;
};

class IApcQueued
{
public:

  virtual BOOL QueueApc( class CApcCall* i_poCall ) = 0;

  virtual ~IApcQueued() { }
};

class CApcCall
{
public:

  CApcCall( IApcQueued* i_poHost ) :
    m_poHost( i_poHost ),
    m_hCompleted( ::CreateEventW( 0, TRUE, FALSE, 0 ) ),
    m_fOk( FALSE )
  {
  }

  virtual ~CApcCall()
  {
  }

  // Calls given method for process the call.
  // Called within destination thread
  void Process( void )
  {
    m_fOk = Invoke();
    VERIFY( ::SetEvent( m_hCompleted ) );
  }

  // Called within callee thread
  BOOL QueueApcAndWaitCompletion( void )
  {
    ENSURE( m_poHost && m_hCompleted );

    ENSURE( m_poHost->QueueApc( this ) );

    ENSURE( WAIT_OBJECT_0
      == ::WaitForSingleObject( m_hCompleted, INFINITE ) );

    return m_fOk;
  }

protected:

  IApcQueued* m_poHost;
  virtual BOOL Invoke( void ) = 0;

private:

  volatile BOOL m_fOk;
  HANDLE m_hCompleted;
};

// Simplified non-OOP version
struct SApcCall
{
  // Called within callee thread
  BOOL Call( HANDLE i_hThread, PAPCFUNC i_xApcProc )
  {
    m_hCompleted = 0;
    m_fOk = FALSE;

    ENSURE( i_hThread && i_xApcProc );

    BOOL fRet = FALSE;

    m_hCompleted = ::CreateEventW( 0, TRUE, FALSE, 0 );
    if( VERIFY( m_hCompleted ) )
    {
      if( VERIFY(
        ::QueueUserAPC( i_xApcProc, i_hThread, (ULONG_PTR) this ) ) )
      {
        if( VERIFY( WAIT_OBJECT_0
              == ::WaitForSingleObject( m_hCompleted, INFINITE ) ) )
        {
          fRet = m_fOk;
        }
      }

      fRet = fRet && VERIFY( ::CloseHandle( m_hCompleted ) );
      m_hCompleted = 0;
    }

    return fRet;
  }

  BOOL m_fOk;
  HANDLE m_hCompleted;
};

class CVersionUtils
{
public:

  static BOOL IsWow64( void )
  {
    BOOL fIsWow64 = FALSE;

    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS xIsWow64Process
      = (LPFN_ISWOW64PROCESS)::GetProcAddress(
        ::GetModuleHandleW( L"kernel32" ), "IsWow64Process" );

    if( xIsWow64Process )
    {
      if( ! xIsWow64Process( ::GetCurrentProcess(), & fIsWow64 ) )
      {
        return FALSE;
      }
    }

    return fIsWow64;
  }
};