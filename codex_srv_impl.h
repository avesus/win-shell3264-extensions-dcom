# include "codex_srv.h"

CodexSrv::CodexSrv() :
  m_hWndParent( 0 ),
  m_fnOpened( FALSE )
{
  ::InterlockedIncrement( & g_nRefCounter );
  //::MessageBoxW( 0, L"Constructor", L"CodexSrv", 0 );
}

CodexSrv::~CodexSrv()
{
  //::MessageBoxW( 0, L"Destructor", L"CodexSrv", 0 );
  if( m_oCodexApiDll.IsLoaded() )
  {
    m_oCodexApiDll.FreePlugIns();
    VERIFY( m_oCodexApiDll.Free() );
  }

  if( ! ::InterlockedDecrement( & g_nRefCounter ) )
  {
    ::PostQuitMessage( 0 );
  }
}

HRESULT
CodexSrv::IUnknown_QueryInterface(
  __in  const IID &  i_poReqIfaceId,
  __out       void** o_ppoIface )
{
	if( ! VERIFY( o_ppoIface ) )
	{
		return E_INVALIDARG;
	}

	* o_ppoIface = 0;

  if( __uuidof( ICodexArchive ) == i_poReqIfaceId )
	{
    if( ! m_oCodexApiDll.IsLoaded() )
    {
      if( ! VERIFY( m_oCodexApiDll.Load( L"mCodexAPI.dll" ) ) )
      {
        return ERROR_FILE_NOT_FOUND;
      }

      if( ! m_oCodexApiDll.LoadPlugIns() )
      { // if no one plugin enabled, free dll
        m_oCodexApiDll.FreePlugIns();
        m_oCodexApiDll.Free();

        return ERROR_BAD_CONFIGURATION;
      }

      /*
      ::MessageBoxW( 0, L"QueryInterface for ICodexArchive: Plugins loaded.", L"CodexSrv", 0 );
      */
    }

    * o_ppoIface
      = static_cast< ICodexArchive* >(
          static_cast< ICodexArchive_wrap* >( this ) );
	}
  else if( __uuidof( ICodexControlPanel ) == i_poReqIfaceId )
	{
    * o_ppoIface
      = static_cast< ICodexControlPanel* >(
          static_cast< ICodexControlPanel_wrap* >( this ) );
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

ULONG
CodexSrv::IUnknown_AddRef( void )
{
  LONG nRefCount = CRefCounted_AddRef();
/*
  WCHAR awzStr[ 1024 ] = { 0 };
  ::wsprintf( awzStr, L"%d AddRef", nRefCount );
  ::MessageBoxW( 0, awzStr, L"CodexSrv", 0 );
*/
  return nRefCount;
}

ULONG
CodexSrv::IUnknown_Release( void )
{
  //::MessageBoxW( 0, L"Release()", L"CodexSrv", 0 );
  LONG nRefCount = CRefCounted_Release();
/*
  WCHAR awzStr[ 1024 ] = { 0 };
  ::wsprintf( awzStr, L"%d Release", nRefCount );
  ::MessageBoxW( 0, awzStr, L"CodexSrv", 0 );
*/
  return nRefCount;
}
