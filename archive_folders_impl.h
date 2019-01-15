# include "archive_folders.h"
# include "af_enum_id_list.h"

# include <shlobj.h>

//extern CCodexSrvCache* g_poCodexCache;

CArchiveFolders::
CArchiveFolders() :
  m_poIdList( 0 ),
  m_poICodex( 0 ),
  m_poEnumIdList( 0 ),
  m_poCodexAccess( 0 ),
  m_poView( 0 ),
  m_fnStdColumnsGetted( FALSE )
  //m_poParent( 0 ),
  //m_poIShellView( 0 )
{
  ::wcsncpy( m_awzInArchivePath, L"./", 3 );
  ::memset( m_awPathBuffer, 0, ALEN( m_awPathBuffer ) );
  m_bstrPath = & m_awPathBuffer[ 2 ];
  ::InitializeCriticalSection( & m_oCritSect );
  ::InterlockedIncrement( & g_nRefCounter );

  m_aoStdColumns[ 0 ].cxChar = 45;
  m_aoStdColumns[ 1 ].cxChar = 9;
  m_aoStdColumns[ 2 ].cxChar = 30;

  m_aoStdColumns[ 0 ].fmt = LVCFMT_LEFT;
  m_aoStdColumns[ 1 ].fmt = LVCFMT_LEFT;
  m_aoStdColumns[ 2 ].fmt = LVCFMT_LEFT;

  m_aoStdColumns[ 0 ].str.uType = STRRET_CSTR;
  m_aoStdColumns[ 1 ].str.uType = STRRET_CSTR;
  m_aoStdColumns[ 2 ].str.uType = STRRET_CSTR;

  ::strcpy( m_aoStdColumns[ 0 ].str.cStr, "Name" );
  ::strcpy( m_aoStdColumns[ 1 ].str.cStr, "Size" );
  ::strcpy( m_aoStdColumns[ 2 ].str.cStr, "Type" );

  m_poNameColumn = & m_aoStdColumns[ 0 ];
  m_poSizeColumn = & m_aoStdColumns[ 1 ];
  m_poTypeColumn = & m_aoStdColumns[ 2 ]; 

  m_oFolderSettings.fFlags = 0;
  m_oFolderSettings.ViewMode = FVM_DETAILS;

  ::memset( & m_oPersistFolder3Info, 0,
    sizeof( m_oPersistFolder3Info ) );
}

CArchiveFolders::
~CArchiveFolders()
{
  if( m_poICodex )
  {
    m_poICodex->Release();
    m_poICodex = 0;
  }
/*
  if( m_poParent )
  {
    m_poParent->IUnknown_Release();
  }
*/
  if( m_poCodexAccess )
  {
    delete m_poCodexAccess;
    m_poCodexAccess = 0;
  }

  if( m_poIdList )
  {
    //::free( m_poIdList );//
    ::CoTaskMemFree( m_poIdList );
    m_poIdList = 0;
  }

  ::InterlockedDecrement( & g_nRefCounter );

  ::DeleteCriticalSection( & m_oCritSect );
}

HRESULT
CArchiveFolders::
IUnknown_QueryInterface(
  __in  const IID &  i_poReqIfaceId,
  __out       void** o_ppoIface )
{
	if( ! VERIFY( o_ppoIface ) )
	{
		return E_INVALIDARG;
	}

	* o_ppoIface = 0;

# if 0
  WCHAR* pwzIdName = 0;//[ 512 ] = { 0 };
  ::StringFromIID( i_poReqIfaceId, & pwzIdName );

  ::MessageBoxW( 0, pwzIdName, L"QueryIface[CArchiveFolders]", 0 );

  if( pwzIdName )
  {
    ::CoTaskMemFree( pwzIdName );
  }
# endif

  if( __uuidof( IUnknown ) == i_poReqIfaceId )
	{
		* o_ppoIface
      = static_cast< IUnknown* >(
          static_cast< IUnknown_wrap* >( this ) );

		IUnknown_AddRef();
	}
  else if( __uuidof( IShellFolder ) == i_poReqIfaceId )
	{
		* o_ppoIface
      = static_cast< IShellFolder* >(
          static_cast< IShellFolder_wrap* >( this ) );

		IUnknown_AddRef();
	}
  else if( __uuidof( IShellFolder2 ) == i_poReqIfaceId )
	{
		* o_ppoIface
      = static_cast< IShellFolder2* >(
          static_cast< IShellFolder2_wrap* >( this ) );

		IUnknown_AddRef();
	}
  else if( __uuidof( IPersist ) == i_poReqIfaceId )
  {
	  * o_ppoIface
      = static_cast< IPersist* >(
          static_cast< IPersist_wrap* >( this ) );

	  IUnknown_AddRef();

    return S_OK;
  }
  else if( __uuidof( IPersistFolder ) == i_poReqIfaceId )
  {
	  * o_ppoIface
      = static_cast< IPersistFolder* >(
          static_cast< IPersistFolder_wrap* >( this ) );

	  IUnknown_AddRef();

    return S_OK;
  }
  else if( __uuidof( IPersistFolder2 ) == i_poReqIfaceId )
  {
	  * o_ppoIface
      = static_cast< IPersistFolder2* >(
          static_cast< IPersistFolder2_wrap* >( this ) );

	  IUnknown_AddRef();

    return S_OK;
  }
  else if( __uuidof( IPersistFolder3 ) == i_poReqIfaceId )
  {
	  * o_ppoIface
      = static_cast< IPersistFolder3* >(
          static_cast< IPersistFolder3_wrap* >( this ) );

	  IUnknown_AddRef();

    return S_OK;
  }
  else if( __uuidof( IPersistIDList ) == i_poReqIfaceId )
  {
	  * o_ppoIface
      = static_cast< IPersistIDList* >(
          static_cast< IPersistIDList_wrap* >( this ) );

	  IUnknown_AddRef();

    return S_OK;
  }
  else if( __uuidof( IDropTarget ) == i_poReqIfaceId )
  {
/*    ::MessageBoxW( 0, L"CArchiveFolders::QueryInterface\r\nIDropTarget",
      L"Archive Folders", 0 );*/
    return E_NOINTERFACE;
  }
  else if( __uuidof( IContextMenu ) == i_poReqIfaceId )
  {
/*    ::MessageBoxW( 0, L"CArchiveFolders::QueryInterface\r\nIContextMenu",
      L"Archive Folders", 0 );*/
    return E_NOINTERFACE;
  }
	else
	{
    CRegistryGuid oGuid;
    oGuid.Build( i_poReqIfaceId );

    static GUID s_lastGuid = { 0 };

    const GUID oLast = {
      0x8CD9494C, 0xAC44, 0x44D0,
      { 0x8F, 0xFD, 0xD4, 0xCF, 0x86, 0x72, 0xB0, 0xA0 } };

    if( s_lastGuid == oLast )
    {
      ENSURE( TRUE );
    }

    s_lastGuid = i_poReqIfaceId;


		return E_NOINTERFACE;
	}

	return S_OK;
}

ULONG
CArchiveFolders::
IUnknown_AddRef( void )
{
  return CRefCounted_AddRef();
}

ULONG
CArchiveFolders::
IUnknown_Release( void )
{
  return CRefCounted_Release();
}


// PRIVATE

LPITEMIDLIST
CArchiveFolders::
GetItemIDList( void )
{
  return m_poIdList;
}

LPITEMIDLIST
CArchiveFolders::
GetNextItemID( LPCITEMIDLIST pidl )
{ 
  // Check for valid pidl.
  if( ! pidl )
  {
    return 0;
  }

  // Get the size of the specified item identifier. 
  int cb = pidl->mkid.cb; 

  // If the size is zero, it is the end of the list. 
  if( ! cb )
  {
    return 0;
  }

  // Add cb to pidl (casting to increment by bytes). 
  pidl = (LPITEMIDLIST) (((LPBYTE) pidl) + cb); 

  // Return NULL if it is null-terminating, or a pidl otherwise. 
  return (pidl->mkid.cb == 0) ? NULL : (LPITEMIDLIST) pidl; 
}

UINT
CArchiveFolders::
GetSize( LPCITEMIDLIST pidl )
{
  UINT cbTotal = 0;
  if( pidl )
  {
    cbTotal += sizeof(pidl->mkid.cb);    // Terminating null character
    while (pidl)
    {
      cbTotal += pidl->mkid.cb;
      pidl = GetNextItemID(pidl);
    }
  }

  return cbTotal;
}


BOOL CArchiveFolders::GetCodex( ICodexArchive** o_ppICodex )
{
  ENSURE( o_ppICodex );

  * o_ppICodex = 0;

  CAveAutoGuard oGuard( & m_oCritSect );

  if( ! m_bstrPath[ 0 ] )
  {
    return TRUE;
  }

  if( ! m_poICodex )
  {
    BSTR bstrErr = 0;
    if( ! m_poCodexAccess )
    {
      m_poCodexAccess = new CCodexAccess();
      if( ! VERIFY( m_poCodexAccess ) )
      {
        return E_OUTOFMEMORY;
      }

      //BSTR bstrPath = ::SysAllocString( m_bstrPath );

      BOOL fnTryAdmin = FALSE;

      HANDLE hFile
        = ::CreateFileW( m_bstrPath, GENERIC_READ, FILE_SHARE_READ,
            0, OPEN_EXISTING, 0, 0 );
      if( INVALID_HANDLE_VALUE == hFile )
      {
        fnTryAdmin = TRUE;
      }
      else
      {
        ::CloseHandle( hFile );
      }

      if( ! VERIFY( m_poCodexAccess->Open(
        m_bstrPath, FALSE,
        & m_poICodex, & bstrErr ) ) )
      {
        //::SysFreeString( bstrPath );
        delete m_poCodexAccess;
        m_poCodexAccess = 0;

        return E_OUTOFMEMORY;
      }

      //::SysFreeString( bstrPath );

      if( ! m_poICodex )
      {
        delete m_poCodexAccess;
        m_poCodexAccess = 0;

/*        ::MessageBoxW( 0, bstrErr, L"Cannot open archive file:",
          MB_ICONWARNING );*/
        return RPC_S_SERVER_UNAVAILABLE;
      }
    }
  }

  * o_ppICodex = m_poICodex;

  return TRUE;
}