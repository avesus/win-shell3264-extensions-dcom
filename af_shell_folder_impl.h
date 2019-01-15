# include "archive_folders.h"
# include "af_enum_id_list.h"
# include "af_extract_icon.h"
# include "af_context_menu.h"
//# include "af_shell_view.h"

# include <shlobj.h>

//IShellBrowser* g_poShellBrowser = 0;

//# pragma comment( lib, "propsys.lib" )

HRESULT FindItem( IShellFolder* i_poIShellFolder,
               WCHAR* i_pwzFindWhat, ITEMIDLIST** o_ppoFound )
{
  * o_ppoFound = 0;

  HRESULT hRes = E_UNEXPECTED;

  IEnumIDList* poList = 0;
  hRes = i_poIShellFolder->EnumObjects( 0,
    SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, & poList );
  if( hRes != S_OK )
  {
    if( poList )
    {
      poList->Release();
    }

    return hRes;
  }

  ITEMIDLIST* poItem = 0;
  ULONG nFetched = 0;
  while( S_OK == poList->Next( 1, & poItem, & nFetched ) )
  {
    if( 0 == ::wcscmp(
      (WCHAR*)( & poItem->mkid.abID[ 1 ] ), i_pwzFindWhat ) )
    {
      * o_ppoFound = poItem;
      break;
    }

    ::CoTaskMemFree( poItem );
  }

  poList->Release();

  return S_OK;
}


// Convert in-folder path into a relative pidl
HRESULT
CArchiveFolders::
IShellFolder_ParseDisplayName(
	__RPC__in_opt HWND hwnd,
	__RPC__in_opt IBindCtx *pbc,
	__RPC__in LPWSTR pszDisplayName,
	__reserved  ULONG *pchEaten,
	__RPC__deref_out_opt PIDLIST_RELATIVE *ppidl,
	__RPC__inout_opt ULONG *pdwAttributes )
{
  HRESULT hRes = E_UNEXPECTED;

  CAutoArray< WCHAR > awzInFolderPath;
  if( ! VERIFY( awzInFolderPath.Alloc(
    ::wcslen( pszDisplayName ) + 1 ) ) )
  {
    return E_OUTOFMEMORY;
  }
  WCHAR* pwzInFolderPath = awzInFolderPath.Ptr();
  if( ! pwzInFolderPath )
  {
    return E_UNEXPECTED;
  }

  ::wcscpy( pwzInFolderPath, pszDisplayName );

  ::PathRemoveBackslashW( pwzInFolderPath );

  IShellFolder* poShellFolder = static_cast< IShellFolder* >(
        static_cast< IShellFolder_wrap* >( this ) );
  poShellFolder->AddRef();

  ITEMIDLIST* poList = 0; //m_poIdList;

  //::MessageBoxW( 0, L"", L"IShellFolder::ParseDisplayName", 0 );

  BOOL fnIsItemFolder = FALSE;

  WCHAR* pwzNext = 0;
  do
  {
    pwzNext = ::PathFindNextComponentW( pwzInFolderPath );

    if( pwzNext[ 0 ] )
    {
      * (pwzNext - 1) = 0;
    }

    ITEMIDLIST* poItem = 0;
    FindItem( poShellFolder, pwzInFolderPath, & poItem );
    if( poItem )
    {
      poList = ::ILCombine( poList, poItem );
    }
    else
    {
      poShellFolder->Release();
      return E_UNEXPECTED;
    }

    IShellFolder* poOldShellFolder = poShellFolder;

    hRes = poShellFolder->BindToObject(
      poItem, 0, __uuidof( IShellFolder ),
      (void**) & poShellFolder );
    poOldShellFolder->Release();
    fnIsItemFolder = poItem->mkid.abID[ 0 ] ? TRUE : FALSE;
    ::CoTaskMemFree( poItem );
    if( hRes != S_OK )
    {
      if( poShellFolder )
      {
        poShellFolder->Release();
      }
      return hRes;
    }

    pwzInFolderPath = pwzNext;
  }
  while( pwzNext[ 0 ] );

  poShellFolder->Release();

  * ppidl = poList;

  if( pchEaten )
  {
    * pchEaten = awzInFolderPath.Size();
  }

  if( pdwAttributes )
  {
    if( fnIsItemFolder )
    {
      * pdwAttributes
        = SFGAO_BROWSABLE | SFGAO_FILESYSTEM | SFGAO_FOLDER
        | SFGAO_HASSUBFOLDER
        | SFGAO_ISSLOW | SFGAO_COMPRESSED;
    }
    else
    {
      * pdwAttributes
        = SFGAO_COMPRESSED | SFGAO_FILESYSTEM | SFGAO_ISSLOW;
    }
  }

	return S_OK;
}

//@ worked
HRESULT
CArchiveFolders::
IShellFolder_EnumObjects(
	__RPC__in_opt HWND i_hwnd,
	SHCONTF i_grfFlags,
	__RPC__deref_out_opt IEnumIDList** o_ppIEnumIDList )
{
  if( ! VERIFY( o_ppIEnumIDList ) )
  {
    return E_INVALIDARG;
  }

  if( ! m_poIdList )
  {
    return E_UNEXPECTED;
  }

  CAveAutoGuard oGuard( & m_oCritSect );

  if( ! m_poICodex )
  {
    ICodexArchive* poICodex = 0;

    //if( ! m_poParent )
    //{
    ENSURE( GetCodex( & poICodex ) );
    /*}
    else
    {
      ENSURE( m_poParent->GetCodex( & poICodex ) );
      m_poICodex = poICodex;
    }*/

    if( ! poICodex )
    {
      return S_FALSE;
    }
  }

  ENSURE( m_poICodex );
  
  m_poICodex->AddRef();

  CArchiveFoldersEnumIDList* poEnumIdList
    = new CArchiveFoldersEnumIDList(
        this, m_poICodex, i_grfFlags, m_awzInArchivePath );
  if( ! VERIFY( poEnumIdList ) )
  {
    m_poICodex->Release();
    return E_OUTOFMEMORY;
  }

  * o_ppIEnumIDList
    = static_cast< IEnumIDList* >(
        static_cast< IEnumIDList_wrap* >( poEnumIdList ) );

  IUnknown_AddRef();

  return S_OK;
}

BOOL GetInFolderPathFromIDList( 
  PCUIDLIST_RELATIVE i_poItemId,
  WCHAR* o_pawzPath )
{
  LPCITEMIDLIST poCurFolder = i_poItemId;

  WCHAR* pwzFolder = 0;

  while( poCurFolder->mkid.cb )
  {
    if( pwzFolder )
    {
      ::wcscat( o_pawzPath, L"\\" );
    }

    pwzFolder = (WCHAR*)( & poCurFolder->mkid.abID[ 1 ] );

    ::wcscat( o_pawzPath, pwzFolder );

    poCurFolder = (LPCITEMIDLIST)
      & ( ((BYTE*)poCurFolder)[ poCurFolder->mkid.cb ] );
  }
  
  return TRUE;
}
    
HRESULT
CArchiveFolders::
IShellFolder_BindToObject( 
	__RPC__in PCUIDLIST_RELATIVE i_poItemId,
	__RPC__in_opt IBindCtx* i_poIBindCtx,
	__RPC__in REFIID i_poReqShellFolderIface,
	__RPC__deref_out_opt void** o_ppIShellFolder )
{
  if( __uuidof( IShellFolder ) == i_poReqShellFolderIface
      ||
      __uuidof( IShellFolder2 ) == i_poReqShellFolderIface )
  {
    CArchiveFolders* poNewFolder = new CArchiveFolders();
    if( ! poNewFolder )
    {
      return E_OUTOFMEMORY;
    }
/*
    if( * (UINT16*)(& ((BYTE*)i_poItemId)[ i_poItemId->mkid.cb ]) )
    {
      ::MessageBoxW( 0,
        L"IShellFolder::BindToObject[IShellFolder]",
        L"Archive Folders", 0 );
    }

    */
/*
    if( m_poParent )
    {
      ::MessageBoxW( 0,
        L"IShellFolder::BindToObject[IShellFolder]",
        L"Archive Folders", 0 );
    }

    */

/*    ::wcscat( & poNewFolder->m_awPathBuffer[ 2 ], L"\\" );

    ::wcscat( & poNewFolder->m_awPathBuffer[ 2 ],
      (WCHAR*)( & i_poItemId->mkid.abID[ 1 ] ) );

    * ((UINT32*)poNewFolder->m_awPathBuffer)
      += (i_poItemId->mkid.cb - 3) / sizeof( WCHAR ) + 1;

    * ((UINT32*)m_awPathBuffer)
      = sizeof( WCHAR )
        * (::wcsnlen_s( m_bstrPath, ALEN( m_awPathBuffer ) - 3 ) + 1);
*/
    poNewFolder->m_awzInArchivePath[ 0 ] = 0;

    WCHAR awzLocalPath[ 1024 ] = { 0 };

    ENSURE( GetInFolderPathFromIDList( i_poItemId, awzLocalPath ) );
    if( 0 != ::wcsncmp( m_awzInArchivePath, L"./", 2 ) )
    {
      ::wcscat( poNewFolder->m_awzInArchivePath, m_awzInArchivePath );
      ::wcscat( poNewFolder->m_awzInArchivePath, L"\\" );
    }

    ::wcscat( poNewFolder->m_awzInArchivePath, awzLocalPath );
/*
    if( 0 == ::wcsncmp( m_awzInArchivePath, L"./", 2 ) )
    {
      ::wsprintf( poNewFolder->m_awzInArchivePath,
        L"%s", (WCHAR*)(& i_poItemId->mkid.abID[ 1 ]) );
    }
    else
    {
      ::MessageBoxW( 0,
        L"IShellFolder::BindToObject[IShellFolder]",
        L"Archive Folders", 0 );

      ::wsprintf( poNewFolder->m_awzInArchivePath,
        L"%s\\%s", m_awzInArchivePath,
        (WCHAR*)(& i_poItemId->mkid.abID[ 1 ]) );
    }
    */

/*    CArchiveFolders* poRootFolder = 0;
    CArchiveFolders* poFolder = this;
    while( poFolder )
    {
      poRootFolder = poFolder;
      poFolder = poFolder->m_poParent;
    }
*/
    ::memcpy( poNewFolder->m_awPathBuffer,
      m_awPathBuffer,
      ALEN( m_awPathBuffer ) );

    // Calculate size of list.
    UINT32 nSize = 0;
    nSize = GetSize( m_poIdList );

    LPITEMIDLIST poMyIdList = (LPITEMIDLIST)::CoTaskMemAlloc( nSize );
    if( ! VERIFY( poMyIdList ) )
    {
      return E_OUTOFMEMORY;
    }

    ::CopyMemory( poMyIdList, m_poIdList, nSize );

    poNewFolder->m_poIdList
      = ::ILCombine( poMyIdList, i_poItemId );

    //poNewFolder->IPersistFolder_Initialize( poMyIdList );

    //poNewFolder->m_poParent = this;
    //poNewFolder->m_poICodex = m_poICodex;

    //IUnknown_AddRef();
    /*if( m_poICodex )
    {
      m_poICodex->AddRef();
    }*/

    if( __uuidof( IShellFolder ) == i_poReqShellFolderIface )
    {
      * o_ppIShellFolder
      = static_cast< IShellFolder* >(
          static_cast< IShellFolder_wrap* >( poNewFolder ) );
    }
    else if( __uuidof( IShellFolder2 ) == i_poReqShellFolderIface )
    {
      * o_ppIShellFolder
        = static_cast< IShellFolder2* >(
            static_cast< IShellFolder2_wrap* >( poNewFolder ) );
    }

    return S_OK;
  }
  else if( __uuidof( IShellItem ) == i_poReqShellFolderIface )
  {
    ::MessageBoxW( 0, L"IShellFolder::BindToObject", L"Archive Folders", 0 );
  }
  else if( __uuidof( IShellItem ) == i_poReqShellFolderIface )
  {
    ::MessageBoxW( 0, L"IShellFolder::BindToObject", L"Archive Folders", 0 );
  }

  return E_NOTIMPL;
/*
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
  */

  if( o_ppIShellFolder )
  {
    * o_ppIShellFolder = 0;
/*
    CTest* poTest = new CTest();
    if( ! VERIFY( poTest ) )
    {
	    return E_OUTOFMEMORY;
    }

    HRESULT hRes = poTest->QueryInterface( i_poReqShellFolderIface,
      o_ppIShellFolder );
    //VERIFY( S_OK == hRes );

    poTest->Release();

    return hRes;
*/
    return IUnknown_QueryInterface( i_poReqShellFolderIface,
      o_ppIShellFolder );
    //* o_ppIShellFolder = 0;//static_cast< IShellFolder2* >( this );
  }

	//::MessageBoxW( 0, L"", L"IShellFolder::BindToObject", 0 );

	return E_NOINTERFACE;
}

HRESULT
CArchiveFolders::
IShellFolder_BindToStorage( 
		/* [in] */ __RPC__in PCUIDLIST_RELATIVE pidl,
		/* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
		/* [in] */ __RPC__in REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out_opt void **ppv)
{
	::MessageBoxW( 0, L"", L"IShellFolder::BindToStorage", 0 );
	return E_NOTIMPL;
}

HRESULT
CArchiveFolders::
IShellFolder_CompareIDs(
		/* [in] */ LPARAM lParam,
		/* [in] */ __RPC__in PCUIDLIST_RELATIVE pidl1,
		/* [in] */ __RPC__in PCUIDLIST_RELATIVE pidl2)
{
	//::MessageBoxW( 0, L"", L"IShellFolder::CompareIDs", 0 );
	return E_NOTIMPL;
}

//@ worked
HRESULT
CArchiveFolders::
IShellFolder_CreateViewObject( 
  __RPC__in_opt HWND i_hwndOwner,
  __RPC__in REFIID i_poReqIfaceId,
  __RPC__deref_out_opt void** o_ppIShellView )
{
  UNREFERENCED_PARAMETER( i_hwndOwner );

  if( ! VERIFY( o_ppIShellView ) )
  {
    return E_INVALIDARG;
  }

  * o_ppIShellView = 0;

  //IExplorerCommandProvider

  struct __declspec(uuid("31CD94F2-0364-4C41-A1D4-23ACB33F0ECC"))
    IFakeUnknownIfaceForDebugOnly;

  struct __declspec(uuid("93F81976-6A0D-42C3-94DD-AA258A155470"))
    IFakeUnknownIfaceForDebugOnly2;

  struct __declspec(uuid("E864A697-83E2-4CF6-B232-BA2B59D7AD54"))
    IFakeUnknownIfaceForDebugOnly3;

  struct __declspec(uuid("176C11B1-4302-4164-8430-D5A9F0EEACDB"))
    IFrameLayoutDefinition;

  struct __declspec(uuid("32AE3A1F-D90E-4417-9DD9-23B0DFA4621D"))
    IItemSetOperations;

  struct __declspec(uuid("4CA18419-902A-4F03-8DB4-B24D218AC5AA"))
    IFolderWithSearchRoot;

  struct __declspec(uuid("48addd32-3ca5-4124-abe3-b5a72531b207"))
    My_ITransferDestination;
/*
  struct __declspec(uuid("64961751-0835-43c0-8ffe-d57686530e64"))
    My_IExplorerCommandProvider;
*/
  if( __uuidof( IFakeUnknownIfaceForDebugOnly ) == i_poReqIfaceId
      ||
      __uuidof( IFakeUnknownIfaceForDebugOnly2 ) == i_poReqIfaceId
      ||
      __uuidof( IFakeUnknownIfaceForDebugOnly3 ) == i_poReqIfaceId
      ||
      __uuidof( IFrameLayoutDefinition ) == i_poReqIfaceId
      ||
      __uuidof( IItemSetOperations ) == i_poReqIfaceId
      ||
      __uuidof( IFolderWithSearchRoot ) == i_poReqIfaceId
      ||
      __uuidof( My_ITransferDestination ) == i_poReqIfaceId
      )
  {
    return E_NOINTERFACE;
  }

  if( __uuidof( IShellView ) == i_poReqIfaceId )
  {
//    ::MessageBoxW( 0, L"IShellFolder::CreateViewObject", L"abc", 0 );

    IShellBrowser* poShellBrowser
      = (IShellBrowser*)::SendMessage( i_hwndOwner, WM_USER + 7, 0, 0 );

    if( ! m_fnStdColumnsGetted && poShellBrowser )
    {
      // Obtain default type columns

      IShellFolder2* poIDesktop2 = 0;

      IShellView* poICurrView = 0;
      if( S_OK == poShellBrowser->QueryActiveShellView(
        & poICurrView ) )
      {
        poICurrView->GetCurrentInfo( & m_oFolderSettings );
/*
        IShellFolder* poIDesktop = 0;
        poICurrView->GetItemObject( SVGIO_BACKGROUND,
          __uuidof( IShellFolder ), (void**) & poIDesktop );

        if( poIDesktop )
        {
          poIDesktop->QueryInterface(
            __uuidof( IShellFolder2 ), (void**) & poIDesktop2 );
          poIDesktop->Release();
        }
*/
        poICurrView->Release();
      }

      if( ! poIDesktop2 )
      {
        //::MessageBoxW( 0, L"sfdsf", L"sdfsdaf", 0 );

        IShellFolder* poIDesktop = 0;
        if( S_OK == ::SHGetDesktopFolder( & poIDesktop ) )
        {
          poIDesktop->QueryInterface(
            __uuidof( IShellFolder2 ), (void**) & poIDesktop2 );
          poIDesktop->Release();
        }
      }

      if( poIDesktop2 )
      {
        UINT32 nColumn = 0;

        const GUID oStorageType = { 0xB725F130, 0x47EF, 0x101A,
          { 0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC } };

        SHCOLUMNID oColumnId = { 0 };
        SHELLDETAILS oShDetails = { 0 };

        UINT32 nAssigned = 0;
        UINT32 nMax = ALEN( m_aoStdColumns );

        while( S_OK == poIDesktop2->MapColumnToSCID( nColumn,
          & oColumnId ) )
        {
          if( oStorageType == oColumnId.fmtid )
          {
            switch( oColumnId.pid )
            {
              // PID_STG_NAME
              break; case 10:
                if( S_OK == poIDesktop2->GetDetailsOf(
                  0, nColumn, & oShDetails ) )
                {
                  if( nAssigned < nMax )
                  {
                    m_aoStdColumns[ nAssigned ] = oShDetails;
                    m_poNameColumn = & m_aoStdColumns[ nAssigned++ ];
                  }
                }

              // PID_STG_SIZE
              break; case 12:
                if( S_OK == poIDesktop2->GetDetailsOf(
                  0, nColumn, & oShDetails ) )
                {
                  if( nAssigned < nMax )
                  {
                    m_aoStdColumns[ nAssigned ] = oShDetails;
                    m_poSizeColumn = & m_aoStdColumns[ nAssigned++ ];
                  }
                }

              // PID_STG_STORAGETYPE
              break; case 4:
                if( S_OK == poIDesktop2->GetDetailsOf(
                  0, nColumn, & oShDetails ) )
                {
                  if( nAssigned < nMax )
                  {
                    m_aoStdColumns[ nAssigned ] = oShDetails;
                    m_poTypeColumn = & m_aoStdColumns[ nAssigned++ ];
                  }
                }
            }
          }

          ++ nColumn;

          if( nAssigned == nMax )
          {
            m_fnStdColumnsGetted = TRUE;
            break;
          }
        }

        poIDesktop2->Release();
      }
    }

    SFV_CREATE oCreate = { 0 };
    oCreate.cbSize = sizeof( oCreate );
/*    if( m_poParent && m_poParent->m_poIShellView )
    {
      oCreate.psvOuter = m_poParent->m_poIShellView;
    }
*/
    oCreate.pshf
      = static_cast< IShellFolder2* >(
          static_cast< IShellFolder2_wrap* >( this ) );
    oCreate.psfvcb
      = static_cast< IShellFolderViewCB* >(
          static_cast< IShellFolderViewCB_wrap* >( this ) );
        
    IShellView* poIShellView = 0;
    HRESULT hRes = ::SHCreateShellFolderView( & oCreate,
      & poIShellView );
    if( VERIFY( S_OK == hRes ) )
    {
      //m_poIShellView = poIShellView;

/*      CArchiveFoldersShellView* poView
        = new CArchiveFoldersShellView( poIShellView );
      if( ! poView )
      {
        poIShellView->Release();
        return E_OUTOFMEMORY;
      }

      m_poView = poView;
*/

      //--------------------------------------------------------------
      // Windows 7 support stuff:

      if( poShellBrowser )
      {
        IServiceProvider* poShellViewService = 0;
        if( S_OK == poIShellView->QueryInterface(
          __uuidof( IServiceProvider ),
          (void**) & poShellViewService ) )
        {
          IProfferService* poProfferService = 0;
          if( S_OK == poShellBrowser->QueryInterface(
            __uuidof( IProfferService ), (void**) & poProfferService ) )
          {
            DWORD dwCookie = 0;
            poProfferService->ProfferService(
              SID_SFolderView,
              poShellViewService, & dwCookie );

            poProfferService->Release();
            poProfferService = 0;
          }

          poShellViewService->Release();
          poShellViewService = 0;
        }
      }

      //--------------------------------------------------------------

      * o_ppIShellView = poIShellView;
      /*static_cast< IShellView* >(
          static_cast< IShellView2_wrap* >( poIShellView ) );*/

      return S_OK;
    }
    else
    {
      return hRes;
    }
  }
  else if( __uuidof( IExplorerCommandProvider ) == i_poReqIfaceId )
  {
    * o_ppIShellView
      = static_cast< IExplorerCommandProvider* >(
          static_cast< IExplorerCommandProvider_wrap* >( this ) );

		IUnknown_AddRef();
    /*
    ::MessageBoxW( 0,
      L"IShellFolder::CreateViewObject\r\nIExplorerCommandProvider",
      L"Archive Folders", 0 );
      */

    return S_OK;
  }
  else if( __uuidof( IDropTarget ) == i_poReqIfaceId )
  {
    // NOT IMPL WHILE
    //::MessageBoxW( 0, L"IShellFolder::CreateViewObject\r\nIDropTarget",
    //  L"Archive Folders", 0 );
  }
  /*
  else if( __uuidof( IContextMenu ) == i_poReqIfaceId )
  {
//    ::MessageBoxW( 0, L"IShellFolder::CreateViewObject\r\nIContextMenu",
//      L"Archive Folders", 0 );

    DEFCONTEXTMENU oMenuParams = { 0 };
    oMenuParams.hwnd = i_hwndOwner;
    oMenuParams.pcmcb = 0;
    oMenuParams.pidlFolder = 0;
    oMenuParams.psf
      = static_cast< IShellFolder* >(
          static_cast< IShellFolder_wrap* >( this ) );
    oMenuParams.cidl = 0;
    oMenuParams.apidl = 0;
    oMenuParams.punkAssociationInfo = 0;
    oMenuParams.cKeys = 0;
    oMenuParams.aKeys = 0;

    return ::SHCreateDefaultContextMenu( & oMenuParams, i_poReqIfaceId,
      o_ppIShellView );
  }
  */
  else if( __uuidof( ICategoryProvider ) == i_poReqIfaceId )
  {
//    ::MessageBoxW( 0, L"IShellFolder::CreateViewObject\r\nICategoryProvider",
//      L"Archive Folders", 0 );
  }
/*  else
  {
    ::MessageBoxW( 0, L"IShellFolder::CreateViewObject\r\nOther Iface",
      L"Archive Folders", 0 );
  }*/

	return E_NOINTERFACE;
}

// For given PIDL, this method returns its visible name.
HRESULT
CArchiveFolders::
IShellFolder_GetDisplayNameOf( 
		/* [in] */ __RPC__in PCUITEMID_CHILD pidl,
		/* [in] */ SHGDNF uFlags,
		/* [out] */ __RPC__out STRRET* o_poName )
{
  if( ! VERIFY( o_poName ) )
  {
    return E_INVALIDARG;
  }

  WCHAR awzRelativePath[ 1024 ] = { 0 };
  if( ! GetInFolderPathFromIDList( pidl, awzRelativePath ) )
  {
    return E_UNEXPECTED;
  }

  BOOL fnIsRelativePath = ( SHGDN_INFOLDER & uFlags ) ? TRUE : FALSE;

  o_poName->uType = STRRET_WSTR;

  if( fnIsRelativePath )
  {
    UINT32 nLen = ::wcslen( awzRelativePath );
    o_poName->pOleStr = (WCHAR*)::CoTaskMemAlloc(
      sizeof( WCHAR ) * (nLen + 1) );

    ::wcscpy( o_poName->pOleStr, awzRelativePath );
  }
  else
  {
    WCHAR awzFullPath[ 1024 ] = { 0 };
    ::wcscat( awzFullPath, m_bstrPath );
    ::wcscat( awzFullPath, L"\\" );
    if( 0 != ::wcscmp( m_awzInArchivePath, L"./" ) )
    {
      ::wcscat( awzFullPath, m_awzInArchivePath );
      ::wcscat( awzFullPath, L"\\" );
    }
    ::wcscat( awzFullPath, awzRelativePath );

    UINT32 nLen = ::wcslen( awzFullPath );
    o_poName->pOleStr = (WCHAR*)::CoTaskMemAlloc(
      sizeof( WCHAR ) * (nLen + 1) );

    ::wcscpy( o_poName->pOleStr, awzFullPath );
  }

	return S_OK;
}

// Get attributes for IEnumIDList-returned PIDLs.
// This method should determine for what is the PIDL-
// a folder or a file.
HRESULT
CArchiveFolders::
IShellFolder_GetAttributesOf( 
		UINT i_cidl,
		__RPC__in_ecount_full_opt( i_cidl )
      PCUITEMID_CHILD_ARRAY i_apidl,
		__RPC__inout SFGAOF* io_prgfInOut )
{
//  ::MessageBoxW( 0, L"IShellFolder::GetAttributesOf", L"Archive Folders", 0 );

  * io_prgfInOut = (* io_prgfInOut) & (
    //SFGAO_BROWSABLE
    SFGAO_CANCOPY | SFGAO_CANDELETE
    | SFGAO_CANLINK
    | SFGAO_CANMOVE | SFGAO_CANRENAME
    | SFGAO_COMPRESSED
    | SFGAO_DROPTARGET
    | SFGAO_HASPROPSHEET
    | ( (*i_apidl)->mkid.abID[ 0 ]
        ?
        (SFGAO_BROWSABLE
        |SFGAO_FOLDER
        |SFGAO_HASSUBFOLDER
        |SFGAO_FILESYSANCESTOR) : 0)
    //| SFGAO_STREAM
    //| 
    //| SFGAO_HIDDEN
    //| SFGAO_FILESYSTEM
    | SFGAO_ISSLOW
    );
  
	//::MessageBoxW( 0, L"", L"IShellFolder::GetAttributesOf", 0 );

	return S_OK;
}

BOOL CopyShellDetails( __out SHELLDETAILS* o_poTo,
                       __in const SHELLDETAILS* i_poFrom )
{
  ENSURE( o_poTo && i_poFrom );

  o_poTo->cxChar = i_poFrom->cxChar;
  o_poTo->fmt = i_poFrom->fmt;
  switch( i_poFrom->str.uType )
  {
    break; case STRRET_CSTR:
      ::strncpy( o_poTo->str.cStr, i_poFrom->str.cStr,
        ALEN( i_poFrom->str.cStr ) );

    break; case STRRET_OFFSET:
      o_poTo->str.uOffset = i_poFrom->str.uOffset;

    break; case STRRET_WSTR:
    {
      UINT32 nLen = ::wcslen( i_poFrom->str.pOleStr );
      o_poTo->str.pOleStr
        = (WCHAR*)::CoTaskMemAlloc( sizeof( WCHAR ) * ( nLen + 1 ) );
      ENSURE( o_poTo->str.pOleStr );
      ::wcsncpy( o_poTo->str.pOleStr, i_poFrom->str.pOleStr, nLen + 1 );
    }

    break; default:
      ENSURE( FALSE );
  }

  o_poTo->str.uType = i_poFrom->str.uType;

  return TRUE;
}

// Query column-related data about PIDL
// (including name of file or folder)
// or column header info (if PIDL=0)
//@ worked
HRESULT
CArchiveFolders::
IShellFolder2_GetDetailsOf(
  __RPC__in_opt PCUITEMID_CHILD i_poItemIdList,
  UINT i_nColumn,
  __RPC__out SHELLDETAILS* o_poShellDetails )
{
//  ::MessageBoxW( 0,
//    L"IShellFolder::GetDetailsOf", L"Archive Folders", 0 );

  if( ! VERIFY( o_poShellDetails ) )
  {
    return E_INVALIDARG;
  }

  if( i_poItemIdList )
  {
    ::MessageBoxW( 0,
    L"IShellFolder::GetDetailsOf", L"Archive Folders", 0 );

    // the title of the information field
    // specified by nColumn is returned.
    if( i_nColumn < ALEN( m_aoStdColumns ) )
    {
      BOOL fnIsFolder = i_poItemIdList->mkid.abID[ 0 ];
      WCHAR* pwzObjectName
        = (WCHAR*)( & i_poItemIdList->mkid.abID[ 1 ] );
      UINT32 nNameLen = ( (i_poItemIdList->mkid.cb - 1) / 2 );

      SHFILEINFOW oInfo = { 0 };
      ::SHGetFileInfoW( pwzObjectName,
        (fnIsFolder ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL),
        & oInfo,
        sizeof( oInfo ),
        SHGFI_USEFILEATTRIBUTES
        | SHGFI_TYPENAME );

      if( & m_aoStdColumns[ i_nColumn ] == m_poNameColumn )
      {
        o_poShellDetails->cxChar = m_poNameColumn->cxChar;
        o_poShellDetails->fmt = m_poNameColumn->fmt;
        o_poShellDetails->str.uType = STRRET_WSTR;
        o_poShellDetails->str.pOleStr
          = (WCHAR*)::CoTaskMemAlloc(
            sizeof( WCHAR ) * ( nNameLen + 1 ) );
        ENSURE( o_poShellDetails->str.pOleStr );
        ::wcsncpy( o_poShellDetails->str.pOleStr,
          pwzObjectName, nNameLen + 1 );
      }
      else if( & m_aoStdColumns[ i_nColumn ] == m_poSizeColumn )
      {
        o_poShellDetails->cxChar = m_poSizeColumn->cxChar;
        o_poShellDetails->fmt = m_poSizeColumn->fmt;
        o_poShellDetails->str.uType = STRRET_WSTR;
        o_poShellDetails->str.pOleStr
          = (WCHAR*)::CoTaskMemAlloc(
            sizeof( WCHAR ) * 5 );
        ENSURE( o_poShellDetails->str.pOleStr );
        ::wcsncpy( o_poShellDetails->str.pOleStr,
          L"1024", 5 );
      }
      else if( & m_aoStdColumns[ i_nColumn ] == m_poTypeColumn )
      {
        o_poShellDetails->cxChar = m_poTypeColumn->cxChar;
        o_poShellDetails->fmt = m_poTypeColumn->fmt;
        o_poShellDetails->str.uType = STRRET_WSTR;
        UINT32 nTypeLen = ::wcslen( oInfo.szTypeName );
        o_poShellDetails->str.pOleStr
          = (WCHAR*)::CoTaskMemAlloc(
            sizeof( WCHAR ) * ( nTypeLen + 1 ) );
        ENSURE( o_poShellDetails->str.pOleStr );
        ::wcsncpy( o_poShellDetails->str.pOleStr,
          oInfo.szTypeName, nTypeLen + 1 );
      }
      else
      {
        return E_INVALIDARG;
      }

      return S_OK;
    }
    else
    {
      return E_INVALIDARG;
    }
  }
/*
  IPropertySystem* poIProps = 0;
  if( S_OK == ::PSGetPropertySystem( __uuidof( IPropertySystem ),
    (void**)& poIProps ) )
  {
    if( poIProps )
    {
      poIProps->Release();
      poIProps = 0;
    }
  }
*/
  switch( i_nColumn )
  {
    // Name
    break; case 0:
           case 1:
           case 2:
      if( CopyShellDetails( o_poShellDetails,
        & m_aoStdColumns[ i_nColumn ] ) )
      {
        return S_OK;
      }
    // Date modified
    break; case 3:
      return E_FAIL;

    break; default:
      return E_FAIL;
  }

  /*
  WCHAR awzPath[ MAX_PATH ] = { 0 };
  if( i_poItemIdList )
  {
    ENSURE( ::SHGetPathFromIDListW( i_poItemIdList, awzPath ) );
  }

  WCHAR awzText[ MAX_PATH * 2 ] = { 0 };
  ::swprintf_s( awzText, ALEN( awzText ), L"ID: %d\nPath: %s",
    i_nColumn, awzPath );

	::MessageBoxW( 0, awzText, L"IShellFolder2::GetDetailsOf", 0 );
  */
}


// Query information type that column represents.
// This may be name, date/time, size, etc.
//@ worked
HRESULT
CArchiveFolders::
IShellFolder2_MapColumnToSCID( UINT iColumn,
                 __RPC__out SHCOLUMNID* o_pscid )
{
//  ::MessageBoxW( 0, L"IShellFolder::MapColumnToSCID", L"Archive Folders", 0 );

  if( ! VERIFY( o_pscid ) )
  {
    return E_INVALIDARG;
  }

  if( iColumn < ALEN( m_aoStdColumns ) )
  {
    GUID oColumnType = { 0xB725F130, 0x47EF, 0x101A,
      { 0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC } };

    o_pscid->fmtid = oColumnType;

    if( & m_aoStdColumns[ iColumn ] == m_poNameColumn )
    {
       o_pscid->pid = 10;//PID_STG_NAME;
    }
    else if( & m_aoStdColumns[ iColumn ] == m_poSizeColumn )
    {
      o_pscid->pid = 12;//PID_STG_SIZE;
    }
    else if( & m_aoStdColumns[ iColumn ] == m_poTypeColumn )
    {
      o_pscid->pid = 4;//PID_STG_NAME;
    }
    else
    {
      return E_INVALIDARG;
    }

    return S_OK;
  }

  return E_INVALIDARG;
/*
  switch( iColumn )
  {
    break; case 0:
    {
      GUID oColumnType = { 0xB725F130, 0x47EF, 0x101A,
        { 0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC } };

      o_pscid->fmtid = oColumnType;
      o_pscid->pid = 10;//PID_STG_NAME;

	    return S_OK;
    }

    break; case 1:
    {
      GUID oColumnType = { 0xB725F130, 0x47EF, 0x101A,
        { 0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC } };

      o_pscid->fmtid = oColumnType;
      o_pscid->pid = 12;//PID_STG_SIZE;

	    return S_OK;
    }

    break; case 2:
    {
      GUID oColumnType = { 0xB725F130, 0x47EF, 0x101A,
        { 0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC } };

      o_pscid->fmtid = oColumnType;
      o_pscid->pid = 4;//PID_STG_STORAGETYPE;

	    return S_OK;
    }

    break; default:
    {
      return E_INVALIDARG;
    }
  }
  */
}



// Extract specified type of information from the PIDL.
// Method works similar to GetDetailsOf(), but uses column type
// instead its number. Method MapColumnToSCID allow to get
// information type related to the given column number.
// But comparing with GetDetailsOf(), this method not allows to
// get captions for column headers.
HRESULT
CArchiveFolders::
IShellFolder2_GetDetailsEx(
  __RPC__in PCUITEMID_CHILD i_poItemIdList,
  __RPC__in const SHCOLUMNID* i_pscid,
  __RPC__out VARIANT* o_pv )
{
  //::MessageBoxW( 0, L"IShellFolder::GetDetailsEx", L"Archive Folders", 0 );
  //return E_NOTIMPL;

  /*
  struct SMyItem
  {
    ITEMIDLIST m_oList;
    BYTE m_bFinalZeroes[ 2 ];
  };

  SMyItem oItem = { 0 };
  oItem.m_oList.mkid.cb = sizeof( oItem );
  oItem.m_oList.mkid.abID[ 0 ] = 1;

  */

//  if( 0 == ::memcmp( i_pidl, & oItem, sizeof( oItem ) ) )
//  {

  if( ! i_poItemIdList )
  {
    return E_INVALIDARG;
  }

  const GUID oStorageType = { 0xB725F130, 0x47EF, 0x101A,
    { 0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC } };

  if( oStorageType == i_pscid->fmtid )
  {
    if( 4 == i_pscid->pid )
    {
      BOOL fnIsFolder = i_poItemIdList->mkid.abID[ 0 ];
      WCHAR* pwzObjectName
        = (WCHAR*)( & i_poItemIdList->mkid.abID[ 1 ] );
      UINT32 nNameLen = ( (i_poItemIdList->mkid.cb - 1) / 2 );

      SHFILEINFOW oInfo = { 0 };
      ::SHGetFileInfoW( pwzObjectName,
        (fnIsFolder ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL),
        & oInfo,
        sizeof( oInfo ),
        SHGFI_USEFILEATTRIBUTES
        | SHGFI_TYPENAME );

      //VARIANTARG oArg = { 0 };
      //::VariantInit( & oArg );
      o_pv->vt = VT_BSTR;
      o_pv->bstrVal = ::SysAllocString( oInfo.szTypeName );

      //::VariantCopy( o_pv, & oArg );
      //::SysFreeString( oArg.bstrVal );

/*      ::MessageBoxW( 0,
        L"IShellFolder::GetDetailsEx", L"Archive Folders", 0 );
*/
      return S_OK;
    }

    return S_OK;

      //{B725F130-47EF-101A-A5F1-02608C9EEBAC}

      VARIANTARG oArg = { 0 };
      ::VariantInit( & oArg );

      ::MessageBoxW( 0, L"IShellFolder::GetDetailsEx", L"Archive Folders", 0 );

      oArg.vt = VT_BSTR;
//      oArg.bstrVal = (BSTR)i_pidl;//::SysAllocString( L"Test Item" );

      ::VariantCopy( & oArg, o_pv );
      

      //o_pv->vt = VT_BSTR;
      //o_pv->bstrVal = ::SysAllocString( L"Test Item" );

      
      //o_pv- = L"Test Item";
    }
    else
    {
      return S_FALSE;
    }

/*  }
  else
  {
    return E_FAIL;
  }*/

	//::MessageBoxW( 0, L"", L"IShellFolder2::GetDetailsEx", 0 );

	return S_OK;
}


//@ worked
HRESULT
CArchiveFolders::
IShellFolder2_GetDefaultColumn( DWORD dwRes,
                  __RPC__out ULONG* o_pnSort,
                  __RPC__out ULONG* o_pnDisplay )
{
  //::MessageBoxW( 0, L"IShellFolder::GetDefaultColumn", L"Archive Folders", 0 );
  //return E_NOTIMPL;

  UNREFERENCED_PARAMETER( dwRes );

  if( ! VERIFY( o_pnSort && o_pnDisplay ) )
  {
    return E_INVALIDARG;
  }

  * o_pnSort = 0;
  * o_pnDisplay = 0;

	//::MessageBoxW( 0, L"", L"IShellFolder2::GetDefaultColumn", 0 );
	return S_OK;
}

//@ worked
HRESULT
CArchiveFolders::
IShellFolder2_GetDefaultColumnState( UINT i_iColumn,
                       __RPC__out SHCOLSTATEF* o_pcsFlags )
{
  //::MessageBoxW( 0, L"IShellFolder::GetDefaultColumnState", L"Archive Folders", 0 );

  if( ! VERIFY( o_pcsFlags ) )
  {
    return E_INVALIDARG;
  }

  switch( i_iColumn )
  {
    break; case 0:
      * o_pcsFlags = SHCOLSTATE_TYPE_STR
        | SHCOLSTATE_ONBYDEFAULT | SHCOLSTATE_SLOW
        | SHCOLSTATE_PREFER_VARCMP;
    break; case 1:
      * o_pcsFlags = SHCOLSTATE_TYPE_STR
        | SHCOLSTATE_ONBYDEFAULT | SHCOLSTATE_SLOW
        | SHCOLSTATE_PREFER_FMTCMP;
    break; case 2:
      * o_pcsFlags = SHCOLSTATE_TYPE_STR
        | SHCOLSTATE_ONBYDEFAULT | SHCOLSTATE_SLOW
        | SHCOLSTATE_PREFER_FMTCMP;
    break; default:
      return E_FAIL;
  }

	//::MessageBoxW( 0, L"", L"IShellFolder2::GetDefaultColumnState", 0 );

	return S_OK;
}



HRESULT
CArchiveFolders::
IShellFolder_GetUIObjectOf(
           __RPC__in_opt HWND i_hwndOwner,
                              UINT i_cidl,
	__RPC__in_ecount_full_opt(i_cidl)
              PCUITEMID_CHILD_ARRAY apidl,
	        __RPC__in REFIID i_poReqIfaceId,
	
	        __reserved UINT* io_rgfReserved,
	  __RPC__deref_out_opt void** o_ppoIface )
{
  if( ! VERIFY( o_ppoIface ) )
	{
		return E_INVALIDARG;
	}

	* o_ppoIface = 0;

  if( __uuidof( IContextMenu ) == i_poReqIfaceId )
  {
    DEFCONTEXTMENU oMenuParams = { 0 };
    oMenuParams.hwnd = i_hwndOwner;
    oMenuParams.pcmcb = 0;
    oMenuParams.pidlFolder = 0;
    oMenuParams.psf
      = static_cast< IShellFolder* >(
          static_cast< IShellFolder_wrap* >( this ) );
    oMenuParams.cidl = i_cidl;
    oMenuParams.apidl = apidl;
    oMenuParams.punkAssociationInfo = 0;
    oMenuParams.cKeys = 0;
    oMenuParams.aKeys = 0;

    return ::SHCreateDefaultContextMenu( & oMenuParams, i_poReqIfaceId,
      o_ppoIface );

    IShellBrowser* poShellBrowser
      = (IShellBrowser*)::SendMessage(
        i_hwndOwner, WM_USER + 7, 0, 0 );

    ::MessageBoxW( 0, L"IShellFolder::GetUIObjectOf\r\nIContextMenu",
      L"Archive Folders", 0 );

    /*
    if( ! poShellBrowser )
    {
      return E_NOINTERFACE;
    }*/

    if( 1 == i_cidl )
    {
      BOOL fnIsFolder = (*apidl)->mkid.abID[ 0 ];
      WCHAR* pwzObjectName = (WCHAR*)( & (*apidl)->mkid.abID[ 1 ] );
      UINT32 nNameLen = ( ((*apidl)->mkid.cb - 1) / 2 );

      WCHAR awzPath[ 1024 ] = { 0 };
      ::wcscat( awzPath, m_bstrPath );
      if( 0 != ::wcsncmp( L"./", m_awzInArchivePath, 2 ) )
      {
        ::wcscat( awzPath, L"\\" );
        ::wcscat( awzPath, m_awzInArchivePath );
      }

      CArchiveFoldersContextMenu* poMenu
        = new CArchiveFoldersContextMenu( pwzObjectName, awzPath,
          poShellBrowser );
      if( ! poMenu )
      {
        return E_OUTOFMEMORY;
      }

      * o_ppoIface
        = static_cast< IContextMenu* >(
            static_cast< IContextMenu_wrap* >( poMenu ) );

      return S_OK;
    }
    else
    {
      return E_FAIL;
    }
  }
  else if( __uuidof( IExtractIconW ) == i_poReqIfaceId )
  {
    if( i_cidl > 1 )
    {
      return E_INVALIDARG;
    }

    BOOL fnIsFolder = (*apidl)->mkid.abID[ 0 ];
    WCHAR* pwzObjectName = (WCHAR*)( & (*apidl)->mkid.abID[ 1 ] );
    UINT32 nNameLen = ( ((*apidl)->mkid.cb - 1) / 2 );
/*
    if( ! fnIsFolder )
    {
      ::MessageBoxW( 0, L"IShellFolder::GetUIObjectOf\r\nIExtractIconW",
      L"Archive Folders", 0 );
    }
*/
    SHFILEINFO oInfo = { 0 };
    ::SHGetFileInfoW( pwzObjectName,
      (fnIsFolder ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL),
      & oInfo,
      sizeof( oInfo ),
      SHGFI_USEFILEATTRIBUTES
      | SHGFI_TYPENAME
      | SHGFI_ICONLOCATION );
/*
    if( ! oInfo.szDisplayName[ 0 ] && ! oInfo.iIcon )
    {
      return E_FAIL;
    }
*/
    CArchiveFoldersExtractIcon* poIcon
      = new CArchiveFoldersExtractIcon( oInfo.szDisplayName,
      oInfo.iIcon );
    if( ! poIcon )
    {
      return E_OUTOFMEMORY;
    }

    * o_ppoIface
      = static_cast< IExtractIconW* >(
          static_cast< IExtractIconW_wrap* >( poIcon ) );

    /*::MessageBoxW( 0, L"IShellFolder::GetUIObjectOf\r\nIContextMenu",
      L"Archive Folders", 0 );*/
    return S_OK;
  }

  else if( __uuidof( IQueryAssociations ) == i_poReqIfaceId )
  {
    return ::AssocCreate( CLSID_QueryAssociations, i_poReqIfaceId,
      o_ppoIface );
  }
//  ::MessageBoxW( 0, L"IShellFolder::GetUIObjectOf\r\n???",
//     L"Archive Folders", 0 );

  return E_NOINTERFACE;

  if( __uuidof( IUnknown ) == i_poReqIfaceId )
	{
		* o_ppoIface
      = static_cast< IUnknown* >(
          static_cast< IUnknown_wrap* >( this ) );

		IUnknown_AddRef();
	}
  else if( __uuidof( IShellFolder ) == i_poReqIfaceId )
	{
    ::MessageBoxW( 0, L"IShellFolder::GetUIObjectOf\r\nIShellFolder",
      L"Archive Folders", 0 );

		* o_ppoIface
      = static_cast< IShellFolder* >(
          static_cast< IShellFolder_wrap* >( this ) );

		IUnknown_AddRef();
	}
  else if( __uuidof( IShellFolder2 ) == i_poReqIfaceId )
	{
    ::MessageBoxW( 0, L"IShellFolder::GetUIObjectOf\r\nIShellFolder2",
      L"Archive Folders", 0 );

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

    ::MessageBoxW( 0, L"IShellFolder::GetUIObjectOf\r\nIPersist",
      L"Archive Folders", 0 );

	  IUnknown_AddRef();

    return S_OK;
  }
  else if( __uuidof( IPersistFolder ) == i_poReqIfaceId )
  {
    ::MessageBoxW( 0, L"IShellFolder::GetUIObjectOf\r\nIPersistFolder",
      L"Archive Folders", 0 );

	  * o_ppoIface
      = static_cast< IPersistFolder* >(
          static_cast< IPersistFolder_wrap* >( this ) );

	  IUnknown_AddRef();

    return S_OK;
  }
  else if( __uuidof( IPersistFolder2 ) == i_poReqIfaceId )
  {
    ::MessageBoxW( 0, L"IShellFolder::GetUIObjectOf\r\nIPersistFolder2",
      L"Archive Folders", 0 );
	  * o_ppoIface
      = static_cast< IPersistFolder2* >(
          static_cast< IPersistFolder2_wrap* >( this ) );

	  IUnknown_AddRef();

    return S_OK;
  }
  else if( __uuidof( IDropTarget ) == i_poReqIfaceId )
  {
    ::MessageBoxW( 0, L"IShellFolder::GetUIObjectOf\r\nIDropTarget",
      L"Archive Folders", 0 );
    return E_NOINTERFACE;
  }
  else if( __uuidof( IContextMenu ) == i_poReqIfaceId )
  {
    ::MessageBoxW( 0, L"IShellFolder::GetUIObjectOf\r\nIContextMenu",
      L"Archive Folders", 0 );
    return E_NOINTERFACE;
  }
  else if( __uuidof( IExtractIconW ) == i_poReqIfaceId )
  {
    return E_NOINTERFACE;
  }
	else
	{
    CRegistryGuid oGuid;
    oGuid.Build( i_poReqIfaceId );

		return E_NOINTERFACE;
	}

	return S_OK;

  /*
  Interface Identifier Allowed cidl Value 
  IContextMenu         The cidl parameter can be greater than or equal to one. 
  IContextMenu2        The cidl parameter can be greater than or equal to one. 
  IDataObject          The cidl parameter can be greater than or equal to one. 
  IDropTarget          The cidl parameter can only be one. 
  IExtractIcon         The cidl parameter can only be one. 
  IQueryInfo           The cidl parameter can only be one. 

  */

	// ::MessageBoxW( 0, L"", L"IShellFolder::GetUIObjectOf", 0 );

	return E_NOINTERFACE;
}


// Renaming
HRESULT
CArchiveFolders::
IShellFolder_SetNameOf( 
		/* [unique][in] */ __RPC__in_opt HWND hwnd,
		/* [in] */ __RPC__in PCUITEMID_CHILD pidl,
		/* [string][in] */ __RPC__in LPCWSTR pszName,
		/* [in] */ SHGDNF uFlags,
		/* [out] */ __RPC__deref_out_opt PITEMID_CHILD *ppidlOut)
{
	::MessageBoxW( 0, L"", L"IShellFolder::SetNameOf", 0 );
	return E_NOTIMPL;
}

// search in archive - not impl while
HRESULT
CArchiveFolders::
IShellFolder2_GetDefaultSearchGUID( 
    /* [out] */ __RPC__out GUID *pguid)
{
	//::MessageBoxW( 0, L"", L"IShellFolder2::GetDefaultSearchGUID", 0 );

	return E_NOTIMPL;
}

// search in archive - not impl while
HRESULT
CArchiveFolders::
IShellFolder2_EnumSearches( 
    /* [out] */ __RPC__deref_out_opt IEnumExtraSearch** ppenum)
{
  //////////////!!!!!!!!!

  * ppenum = 0;

	//::MessageBoxW( 0, L"", L"IShellFolder2::EnumSearches", 0 );
	return E_NOTIMPL;
}

HRESULT
CArchiveFolders::
IShellFolderViewCB_MessageSFVCB(
  UINT uMsg, WPARAM wParam, LPARAM lParam )
{
  //::MessageBoxW( 0, L"Message", L"Archive Folders", 0 );

  if( uMsg == 83 )
  {
    ::MessageBoxW( 0, L"Gotcha!", L"Archive Folders", 0 );
  }

  switch( uMsg )
  {
    break; case SFVM_DEFVIEWMODE:
      if( lParam )
      {
        * ((FOLDERVIEWMODE*) lParam)
          = (FOLDERVIEWMODE)m_oFolderSettings.ViewMode;
      }
      return S_OK;

    break; default:
      return E_NOTIMPL;
  }
}

HRESULT
CArchiveFolders::
IExplorerCommandProvider_GetCommands( 
  /* [in] */ __RPC__in_opt IUnknown *punkSite,
  /* [in] */ __RPC__in REFIID riid,
  /* [iid_is][out] */ __RPC__deref_out_opt void** ppv )
{
  if( __uuidof( IEnumExplorerCommand ) == riid )
  {
    * ppv = static_cast< IEnumExplorerCommand* >(
          static_cast< IEnumExplorerCommand_wrap* >( this ) );

		IUnknown_AddRef();
    return S_OK;
  }

  return E_NOINTERFACE;
}

HRESULT
CArchiveFolders::
IExplorerCommandProvider_GetCommand( 
  /* [in] */ __RPC__in REFGUID rguidCommandId,
  /* [in] */ __RPC__in REFIID riid,
  /* [iid_is][out] */ __RPC__deref_out_opt void **ppv)
{
  return E_NOTIMPL;
}

HRESULT
CArchiveFolders::
IEnumExplorerCommand_Next( 
    ULONG celt,
    __out IExplorerCommand** pUICommand,
    ULONG* pceltFetched )
{
  return S_FALSE;
}

HRESULT
CArchiveFolders::
IEnumExplorerCommand_Reset( void )
{
  return S_OK;
}

HRESULT
CArchiveFolders::
IEnumExplorerCommand_Clone( IEnumExplorerCommand **ppenum )
{
  return E_NOTIMPL;
}

HRESULT
CArchiveFolders::
IEnumExplorerCommand_Skip( ULONG i_celt )
{
  return E_NOTIMPL;
}