# include "archive_folders.h"
# include "af_enum_id_list.h"

# include <shlobj.h>

# include "./../codexsrv/com/codexsrv.h"

//extern CCodexSrvCache* g_poCodexCache;

HRESULT
CArchiveFolders::
IPersist_GetClassID(
  __RPC__out CLSID* o_poClsID )
{
  if( ! VERIFY( o_poClsID ) )
  {
    return E_INVALIDARG;
  }

  //::MessageBoxW( 0, L"", L"IPersistFolder2::GetClassID", 0 );

  * o_poClsID = __uuidof( CArchiveFolders );

  return S_OK;
}
/*
BOOL LoadCodexArchiveComServer( BOOL i_fnElevated,
                                ICodexArchive** o_ppoICodex )
{
  ENSURE( o_ppoICodex );

  * o_ppoICodex = 0;

  IClassFactory* poIClassFactory = 0;
  HRESULT hRes = E_UNEXPECTED;

  if( i_fnElevated )
  {
    BIND_OPTS3 bindOptions;
    ::memset( & bindOptions, 0, sizeof( bindOptions ) );
    bindOptions.cbStruct = sizeof( bindOptions );
    bindOptions.hwnd = ::GetForegroundWindow();
    bindOptions.dwClassContext
      = CLSCTX_LOCAL_SERVER | CLSCTX_ACTIVATE_32_BIT_SERVER;

    CRegistryGuid oClsId;
    ENSURE( oClsId.Build( __uuidof( CodexSrv ) ) );

    WCHAR awzElevationMonikerName[ 256 ] = { 0 };
    ENSURE( 0 == ::wcsncpy_s( awzElevationMonikerName,
      ALEN( awzElevationMonikerName ),
      L"Elevation:Administrator!clsid:", 255 ) );

    ENSURE( 0 == ::wcsncat_s( awzElevationMonikerName,
      ALEN( awzElevationMonikerName ), oClsId.Get(), 255 ) );

    hRes = ::CoGetObject(
      awzElevationMonikerName, & bindOptions,
      __uuidof( IClassFactory ),
      reinterpret_cast< void** >( & poIClassFactory ) );
    if( hRes != S_OK )
    {
      // user cancel elevation
      return TRUE;
    }
  }
  else
  {
    hRes = ::CoGetClassObject( __uuidof( CodexSrv ),
		  CLSCTX_LOCAL_SERVER | CLSCTX_ACTIVATE_32_BIT_SERVER,
		  0,
		  IID_IClassFactory, (void**) & poIClassFactory );
	  ENSURE( S_OK == hRes );
  }

  ICodexArchive* poICodex = 0;
	hRes = poIClassFactory->CreateInstance(
    0, __uuidof( ICodexArchive ),
		(void**) & poICodex );

  ENSURE( S_OK == hRes );

  poIClassFactory->Release();
  poIClassFactory = 0;

  * o_ppoICodex = poICodex;

  return TRUE;
}
*/

HRESULT
CArchiveFolders::
IPersistFolder_Initialize( __RPC__in PCIDLIST_ABSOLUTE i_poIdList )
{
  if( ! VERIFY( i_poIdList ) )
  {
    return E_INVALIDARG;
  }

  if( ! m_bstrPath[ 0 ] )
  {
    if( ! VERIFY( ::SHGetPathFromIDList( i_poIdList, m_bstrPath ) ) )
    {
      return E_INVALIDARG;
    }

    * ((UINT32*)m_awPathBuffer)
      = sizeof( WCHAR )
        * ::wcsnlen_s( m_bstrPath, ALEN( m_awPathBuffer ) - 3 );
  }
  else
  {
    ENSURE( TRUE );
  }

  // Calculate size of list.
  UINT32 nSize = 0;
  nSize = GetSize( i_poIdList );

  LPITEMIDLIST poMyIdList = (LPITEMIDLIST)
    ::CoTaskMemAlloc( nSize );
  if( ! VERIFY( poMyIdList ) )
  {
    return E_OUTOFMEMORY;
  }

  ::CopyMemory( poMyIdList, i_poIdList, nSize );

  if( m_poIdList )
  {
    ::CoTaskMemFree( m_poIdList );
  }

  m_poIdList = poMyIdList;

  return S_OK;
}

// IPersistFolder2
HRESULT
CArchiveFolders::
IPersistFolder2_GetCurFolder(
  __RPC__deref_out_opt PIDLIST_ABSOLUTE* o_ppoIdList )
{
  if( ! VERIFY( o_ppoIdList ) )
  {
    return E_INVALIDARG;
  }

  if( ! m_poIdList )
  {
    return E_FAIL;
  }

  // Calculate size of list.
  UINT32 nSize = 0;
  nSize = GetSize( m_poIdList );

  LPITEMIDLIST poMyIdList = (LPITEMIDLIST)::CoTaskMemAlloc( nSize );
  if( ! VERIFY( poMyIdList ) )
  {
    return E_OUTOFMEMORY;
  }

  ::CopyMemory( poMyIdList, m_poIdList, nSize );

  * o_ppoIdList = poMyIdList;

  //::MessageBoxW( 0, L"", L"IPersistFolder2::GetCurFolder", 0 );

  return NOERROR;
}

HRESULT
CArchiveFolders::
IPersistIDList_SetIDList( __RPC__in PCIDLIST_ABSOLUTE i_poIdList )
{
  return IPersistFolder_Initialize( i_poIdList );
}

HRESULT
CArchiveFolders::
IPersistIDList_GetIDList(
  __RPC__deref_out_opt PIDLIST_ABSOLUTE* o_ppoIdList )
{
  return IPersistFolder2_GetCurFolder( o_ppoIdList );
}

HRESULT
CArchiveFolders::
IPersistFolder3_InitializeEx( 
  __RPC__in_opt IBindCtx *pbc,
  __RPC__in PCIDLIST_ABSOLUTE pidlRoot,
  __RPC__in_opt const PERSIST_FOLDER_TARGET_INFO *ppfti )
{
  if( ! ppfti )
  {
    return E_INVALIDARG;
  }

  if( m_oPersistFolder3Info.pidlTargetFolder )
  {
    ::CoTaskMemFree( m_oPersistFolder3Info.pidlTargetFolder );
  }

  m_oPersistFolder3Info = * ppfti;
  m_oPersistFolder3Info.pidlTargetFolder = 0;

  if( ppfti->pidlTargetFolder )
  {
    m_oPersistFolder3Info.pidlTargetFolder
      = ::ILCloneFull( ppfti->pidlTargetFolder );
    if( ! m_oPersistFolder3Info.pidlTargetFolder )
    {
      return E_OUTOFMEMORY;
    }
  }

  ITEMIDLIST* poList = 0;
  SFGAOF nFlags = 0;
  ::SHParseDisplayName( ppfti->szTargetParsingName, 0, & poList,
    SFGAO_FILESYSTEM | SFGAO_FOLDER | SFGAO_HASSUBFOLDER, & nFlags );
  if( ! poList )
  {
    return E_OUTOFMEMORY;
  }

  HRESULT hRes = IPersistFolder_Initialize( poList );

  ::CoTaskMemFree( poList );

  return hRes;
}
      
HRESULT
CArchiveFolders::
IPersistFolder3_GetFolderTargetInfo( 
  __RPC__out PERSIST_FOLDER_TARGET_INFO *ppfti )
{
  if( ! ppfti )
  {
    return E_INVALIDARG;
  }

  * ppfti = m_oPersistFolder3Info;

  return S_OK;
}