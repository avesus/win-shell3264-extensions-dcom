# include "af_context_menu.h"

# include "archive_folders.h"

# include <strsafe.h>

# include <shlwapi.h>

CArchiveFoldersContextMenu::
CArchiveFoldersContextMenu(
  WCHAR* i_pwzObjectName, WCHAR* i_pwzPath,
  IShellBrowser* i_poBrowser ) :
  m_poShellBrowser( i_poBrowser )
{
  ::wcscpy( m_awzObjectName, i_pwzObjectName );
  ::memset( m_awzObjectPath, 0, ALEN( m_awzObjectPath ) );

  if( i_pwzPath )
  {
    //::MessageBoxW( 0, L"Construct", L"Cmd", 0 );

    ::wcscat( m_awzObjectPath, i_pwzPath );
    ::wcscat( m_awzObjectPath, L"\\" );
    ::wcscat( m_awzObjectPath, m_awzObjectName );
  }
}

CArchiveFoldersContextMenu::
~CArchiveFoldersContextMenu()
{
}

HRESULT
CArchiveFoldersContextMenu::
IUnknown_QueryInterface(
  __in  const IID &  i_poReqIfaceId,
  __out       void** o_ppoIface )
{
	if( ! VERIFY( o_ppoIface ) )
	{
		return E_INVALIDARG;
	}

	* o_ppoIface = 0;

  if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IUnknown ) ) )
	{
		* o_ppoIface
      = static_cast< IUnknown* >(
          static_cast< IUnknown_wrap* >( this ) );

		IUnknown_AddRef();
	}
  else if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IShellExtInit ) ) )
	{
		* o_ppoIface
      = static_cast< IShellExtInit* >(
          static_cast< IShellExtInit_wrap* >( this ) );

		IUnknown_AddRef();
	}
  else if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IContextMenu ) ) )
	{
		* o_ppoIface
      = static_cast< IContextMenu* >(
          static_cast< IContextMenu_wrap* >( this ) );

		IUnknown_AddRef();
	}
  else
	{
		return E_NOINTERFACE;
	}

	return S_OK;
}

ULONG
CArchiveFoldersContextMenu::
IUnknown_AddRef( void )
{
  return CRefCounted_AddRef();
}

ULONG
CArchiveFoldersContextMenu::
IUnknown_Release( void )
{
  return CRefCounted_Release();
}

HRESULT
CArchiveFoldersContextMenu::
IShellExtInit_Initialize( 
  __in_opt  PCIDLIST_ABSOLUTE pidlFolder,
  __in_opt  IDataObject *pdtobj,
  __in_opt  HKEY hkeyProgID )
{
  ::SHGetPathFromIDListW( pidlFolder, m_awzObjectPath );

  //::MessageBoxW( 0, L"Init", L"Cmd", 0 );

  ::wcscat( m_awzObjectPath, L"\\" );
  ::wcscat( m_awzObjectPath, m_awzObjectName );

  return S_OK;
}

# define IDM_OPEN 0

HRESULT
CArchiveFoldersContextMenu::
IContextMenu_QueryContextMenu( 
    __in  HMENU hmenu,
    __in  UINT indexMenu,
    __in  UINT idCmdFirst,
    __in  UINT idCmdLast,
    __in  UINT uFlags )
{
 
//  if( CMF_DEFAULTONLY & uFlags )
//  {
/*    return S_OK;
  }

  if( ! (CMF_DEFAULTONLY & uFlags) )
  {*/
  /*
    MENUITEMINFOW oItem = { 0 };
    oItem.cbSize = sizeof( oItem );
    oItem.fMask = MIIM_TYPE | MIIM_STATE;
    oItem.fType = MFT_STRING;
    oItem.fState = MFS_DEFAULT;
    oItem.dwTypeData = L"open";
    oItem.cch = 4;

    if( ! ::InsertMenuItem( hmenu, indexMenu, TRUE, & oItem ) )
    {
      return E_UNEXPECTED;
    }
*/
    ::InsertMenuW( hmenu,
      indexMenu, 
      MF_STRING | MF_BYPOSITION, 
      idCmdFirst,// + IDM_OPEN,
      L"open" );


    UINT32 nMaxSpecifiedId = idCmdFirst;
/*
    return MAKE_HRESULT( SEVERITY_SUCCESS, 0, USHORT( IDM_OPEN + 1 ) );
      //nMaxSpecifiedId - idCmdFirst + 1 );
  }
*/
  return MAKE_HRESULT( SEVERITY_SUCCESS, 0, USHORT( 0 ) );
}


HRESULT
CArchiveFoldersContextMenu::
IContextMenu_InvokeCommand( 
    __in  CMINVOKECOMMANDINFO *pici )
{
  ::MessageBoxW( 0, L"Invoke", L"Cmd", 0 );


  ITEMIDLIST* poList = 0;
  SFGAOF nFlags = 0;
  ::SHParseDisplayName( m_awzObjectPath, 0, & poList,
    SFGAO_FILESYSTEM | SFGAO_FOLDER | SFGAO_HASSUBFOLDER, & nFlags );
/*
  WCHAR awzPath[ 1024 ] = { 0 };
  ::SHGetPathFromIDListW( poList, awzPath );
*/
  if( m_poShellBrowser )
  {
    m_poShellBrowser->BrowseObject( poList,
      SBSP_DEFBROWSER | SBSP_DEFMODE );
  }

/*
  WCHAR awzCmd[ 1024 ] = { 0 };
  ::wcscat( awzCmd, L"explorer.exe /idlist,:" );

  CHAR aczObjectPath[ 1024 ] = { 0 };

  WCHAR* pwzProcessCmdLine = ::GetCommandLineW();

  HANDLE hProcess = ::GetCurrentProcess();
  UINT32 nProcessId = ::GetProcessId( hProcess );
  ::CloseHandle( hProcess );

  WCHAR awzProcess[ 16 ] = { 0 };
  ::_itow( nProcessId, awzProcess, 10 );

  ITEMIDLIST* poList = 0;
  SFGAOF nFlags = 0;
  ::SHParseDisplayName( m_awzObjectPath, 0, & poList,
    0, & nFlags );
  UINT32 nSize = ::ILGetSize( poList );

  HANDLE hIdList = ::SHAllocShared(
    poList, nSize, nProcessId );

  WCHAR awzIdList[ 16 ] = { 0 };
  ::_itow( nhIdList, awzIdList, 10 );

  ::wcscat( awzCmd, awzIdList );
  ::wcscat( awzCmd, L":" );
  ::wcscat( awzCmd, awzProcess );
  ::wcscat( awzCmd, L"," );
  ::wcscat( awzCmd, m_awzObjectPath );

  STARTUPINFO oStart = { 0 };
  oStart.cb = sizeof( oStart );
  PROCESS_INFORMATION oPi = { 0 };
  if( ! ::CreateProcessW( 0, awzCmd,
    0, 0, TRUE, 0, 0, 0, & oStart, & oPi ) )
  {
    DWORD nErr = ::GetLastError();
    return nErr;
  }
*/
  return S_OK;
}

HRESULT
CArchiveFoldersContextMenu::
IContextMenu_GetCommandString( 
  __in  UINT_PTR idCmd,
  __in  UINT uType,
  __reserved  UINT *pReserved,
  __out_awcount(!(uType & GCS_UNICODE), cchMax)  LPSTR pszName,
  __in  UINT cchMax )
{
  if( GCS_VERBW == uType )
  {
    ::MessageBoxW( 0, L"Get Command String W", L"Cmd", 0 );
    return ::StringCchCopyNW( (LPWSTR)pszName,
      cchMax, L"open", 4 );
  }
  else if( GCS_VERBA == uType )
  {
    ::MessageBoxW( 0, L"Get Command String A", L"Cmd", 0 );
    return ::StringCchCopyNA( pszName,
      cchMax, "open", 4 );
  }


//  ::wcscpy( (WCHAR*)pszName, m_awzObjectName );
  return S_FALSE;
}
