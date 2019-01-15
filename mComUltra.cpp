# include "common_header.h"

# include "codex_srv_cache.h"

# include "archive_folders_impl.h"
# include "af_shell_folder_impl.h"
# include "af_persist_folder_impl.h"
# include "af_enum_id_list_impl.h"
# include "af_extract_icon_impl.h"
# include "af_context_menu_impl.h"
//# include "af_shell_view_impl.h"

HINSTANCE g_hInst = 0;
volatile LONG g_nRefCounter = 0;

//CCodexSrvCache g_oCodexCache;
//CCodexSrvCache* g_poCodexCache = & g_oCodexCache;

class CArchiveFoldersClassFactory :
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

		if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IClassFactory ) ) )
		{
			* o_ppoIface
        = static_cast< IClassFactory* >(
            static_cast< IClassFactory_wrap* >( this ) );

			IUnknown_AddRef();
		}
		else if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IUnknown ) ) )
		{
			* o_ppoIface
        = static_cast< IUnknown* >(
            static_cast< IUnknown_wrap* >( this ) );

			IUnknown_AddRef();
		}
		else
		{
			return E_NOINTERFACE;
		}

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
    return CRefCounted_Release();
  }

  virtual HRESULT
  IClassFactory_CreateInstance(
    __in        IUnknown* i_poIfaceOuter,
    __in  const IID &     i_poReqIfaceId,
    __out       void**    o_ppoIface )
  {

	  if( ! VERIFY( o_ppoIface ) )
	  {
		  return E_INVALIDARG;
	  }

	  if( i_poIfaceOuter )
	  {
		  return CLASS_E_NOAGGREGATION;
	  }

	  * o_ppoIface = 0;

	  CArchiveFolders* poArchiveFolders = new CArchiveFolders();
	  if( ! VERIFY( poArchiveFolders ) )
	  {
		  return E_OUTOFMEMORY;
	  }

    IUnknown* poIUnknown
      = static_cast< IUnknown_wrap* >( poArchiveFolders );

	  HRESULT hRes = poIUnknown->QueryInterface(
      i_poReqIfaceId, o_ppoIface );

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
      ::InterlockedDecrement( & g_nRefCounter );
    }

	  return S_OK;
  }

  CArchiveFoldersClassFactory()
	{
    ::InterlockedIncrement( & g_nRefCounter );
	}

  virtual ~CArchiveFoldersClassFactory()
	{
    ::InterlockedDecrement( & g_nRefCounter );
	}
};



// Returns a pointer to the IClassFactory
// of the requested type 'i_poClsid'.
// 'i_poClsid' should be CLSID of dll.
// Using IClassFactory, the client then will get
// a pointer to any interface, supported by this dll.\
// Real constuction of the object and determining its
// real type can be at that phase (by IID).
STDAPI
DllGetClassObject(
  // Which class of the objects to create?
  // Only one object class presented by this dll.
  __in  const CLSID & i_poReqClsid,
  // IID_IClassFactory
  __in  const IID &   i_poIClassFactoryId,
  __out       void**  o_ppoIClassFactory )
{
	if( ! VERIFY( o_ppoIClassFactory ) )
	{
		return E_POINTER;
	}

  // TODO: in some cases it is not initialized!!!
  //::CoInitializeEx( 0, COINIT_APARTMENTTHREADED );

  //VERIFY( FALSE );

	* o_ppoIClassFactory = 0;

	if( ::IsEqualCLSID( i_poReqClsid, __uuidof( CArchiveFolders ) ) )
	{
		CArchiveFoldersClassFactory* poClassFactory = new CArchiveFoldersClassFactory();
		if( ! VERIFY( poClassFactory ) )
		{
			return E_OUTOFMEMORY;
		}

    IUnknown* poIUnknown
      = static_cast< IUnknown_wrap* >( poClassFactory );

		HRESULT hRes = poIUnknown->QueryInterface(
		  i_poIClassFactoryId, o_ppoIClassFactory );
		//VERIFY( S_OK == hRes );

		poIUnknown->Release();

		return hRes;
	}
	else
	{
		return CLASS_E_CLASSNOTAVAILABLE;
	}
}

// Used to determine whether the DLL can be unloaded by OLE
STDAPI
DllCanUnloadNow( void )
{
  if( ::InterlockedCompareExchange( & g_nRefCounter, 0, 0 ) )
  {
    return S_FALSE;
  }

  //::MessageBoxW( 0, L"DllCanUnloadNow()", L"MagicRAR NSE", 0 );
  return S_OK;
}
/*
class CInfo
{
public:

  CInfo()
  {
    ::OutputDebugStringW( L"\r\n\r\nDll loaded\r\n\r\n" );
    ::MessageBoxW( 0, L"Dll loaded", L"Archive Folders", 0 );
  }

  ~CInfo()
  {
    ::OutputDebugStringW( L"\r\n\r\nDll unloaded\r\n\r\n" );
    ::MessageBoxW( 0, L"Dll unloaded", L"Archive Folders", 0 );
  }
};
*/

//CInfo oInfo;

//HANDLE g_hEvent = 0;

extern "C"
BOOL WINAPI
DllMain( HINSTANCE i_hInst, DWORD i_nReason, void* )
{
  switch( i_nReason )
  {
    break; case DLL_PROCESS_ATTACH:
    {
/*      if( ! ::OpenEventW( EVENT_ALL_ACCESS, FALSE, L"asdfkhgsadfh" ) )
      {
        g_hEvent = ::CreateEventW( 0, TRUE, FALSE, L"asdfkhgsadfh" );
      }
      else
      {
        return FALSE;
      }
*/
      g_hInst = i_hInst;

/*      g_poCodexCache = new CCodexSrvCache();
      if( ! g_poCodexCache )
      {
        return FALSE;
      }*/
    }

    break; case DLL_PROCESS_DETACH:
    {
/*      if( g_hEvent )
      {
        ::CloseHandle( g_hEvent );
        g_hEvent = 0;
      }
*/
/*      if( g_poCodexCache )
      {
        delete g_poCodexCache;
        g_poCodexCache = 0;
      }*/
    }
  }
  return TRUE;
}
