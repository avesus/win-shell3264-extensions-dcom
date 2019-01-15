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

# pragma once

# define _WIN32_DCOM
# include <windows.h>

# include <unknwn.h>
# include <objidl.h>

// we completely access methods with concrete names,
// so ambiguity excluded.
# pragma warning( disable: 4584 )

// Macro for CoClass implementers and interface inheritance
# define IMPLEMENTS( IfaceName ) public IfaceName##_wrap

// Utilitary class for the ref counting support.
// Its methods should be called
// within client class IUnknown_AddRef()/IUnknown_Release()
// implementation methods.
class CRefCounted
{
protected:

  ULONG
  CRefCounted_AddRef( void )
  {
    LONG nNewRefCount = ::InterlockedIncrement( & m_nReferencesCount );

    VERIFY( LONG_MAX != nNewRefCount );

    return (ULONG)nNewRefCount;
  }

  ULONG
  CRefCounted_Release( void )
  {
    VERIFY( m_nReferencesCount );

    LONG nNewRefCount = ::InterlockedDecrement( & m_nReferencesCount );
    if( ! nNewRefCount )
    {
      delete this;
    }

    return (ULONG)nNewRefCount;
  }

  CRefCounted() :
     m_nReferencesCount( 1 )
  {
    VERIFY( TRUE );
  }

  virtual ~CRefCounted()
  {
    VERIFY( ! m_nReferencesCount );
  }

private:

  volatile LONG m_nReferencesCount;
};


# define IFACE_METHOD( RetType, IfaceName, MethodName,                \
  MethodSignature, MethodArguments )                                  \
  private: virtual RetType STDMETHODCALLTYPE                          \
  MethodName MethodSignature                                          \
  {                                                                   \
    return IfaceName##_##MethodName MethodArguments ;                 \
  }                                                                   \
  protected: virtual RetType                                          \
  IfaceName##_##MethodName MethodSignature = 0

class //__declspec( novtable )
IUnknown_wrap :
	public IUnknown
{
  IFACE_METHOD( HRESULT, IUnknown,
    QueryInterface, ( REFIID riid, void** ppvObject ),
      ( riid, ppvObject ) );

  IFACE_METHOD( ULONG, IUnknown,
    AddRef, ( void ), () );

  IFACE_METHOD( ULONG, IUnknown,
    Release, ( void ), () );
};

class //__declspec( novtable )
IClassFactory_wrap :
  public IUnknown_wrap,
  public IClassFactory
{
  IFACE_METHOD( HRESULT,
    IUnknown,
    QueryInterface, ( REFIID riid, void** ppvObject ),
      ( riid, ppvObject ) );

  IFACE_METHOD( ULONG,
    IUnknown,
    AddRef, ( void ), () );

  IFACE_METHOD( ULONG,
    IUnknown,
    Release, ( void ), () );

  IFACE_METHOD( HRESULT,
    IClassFactory,
    CreateInstance,
      ( IUnknown* pUnkOuter, REFIID riid, void **ppvObject ),
      ( pUnkOuter, riid, ppvObject ) );

  IFACE_METHOD( HRESULT,
    IClassFactory,
    LockServer, ( BOOL fLock ), ( fLock ) );
};

class //__declspec( novtable )
IPersist_wrap :
  public IUnknown_wrap,
  public IPersist
{
public:

  IFACE_METHOD( HRESULT,
    IUnknown,
    QueryInterface, ( REFIID riid, void** ppvObject ),
      ( riid, ppvObject ) );

  IFACE_METHOD( ULONG,
    IUnknown,
    AddRef, ( void ), () );

  IFACE_METHOD( ULONG,
    IUnknown,
    Release, ( void ), () );

  IFACE_METHOD( HRESULT,
    IPersist,
    GetClassID, ( __RPC__out CLSID *pClassID ),
      ( pClassID ) );
};

# define AVE_BSTR_CONST( Name, WChar ) \
  WCHAR awz##Name[] = L"\0\0" WChar; \
  * ( (__int32*) awz##Name ) = (ALEN( awz##Name ) - 3) * sizeof( * awz##Name ); \
  BSTR bstr##Name = & awz##Name [ 2 ]

# define AVE_BSTR_LEN( Bstr ) ( *(((UINT32*)(Bstr)) - 1) / sizeof( WCHAR ) )