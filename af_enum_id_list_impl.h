# include "af_enum_id_list.h"

# include "archive_folders.h"

HRESULT
CArchiveFoldersEnumIDList::
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
  else if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IEnumIDList ) ) )
	{
		* o_ppoIface
      = static_cast< IEnumIDList* >(
          static_cast< IEnumIDList_wrap* >( this ) );

		IUnknown_AddRef();
	}
  else
	{
		return E_NOINTERFACE;
	}

	return S_OK;
}

ULONG
CArchiveFoldersEnumIDList::
IUnknown_AddRef( void )
{
  return CRefCounted_AddRef();
}

ULONG
CArchiveFoldersEnumIDList::
IUnknown_Release( void )
{
  return CRefCounted_Release();
}

BOOL CreateSimpleIDList( BSTR i_bstrName,
  BOOL i_fnIsFolder,
  PITEMID_CHILD* o_pIL )
{
  ENSURE( i_bstrName && i_bstrName[ 0 ] && o_pIL );
  * o_pIL = 0;

  UINT32 nLen = AVE_BSTR_LEN( i_bstrName );
  ENSURE( nLen && nLen < 64000 );

  UINT32 nHeaderSize = sizeof( ITEMID_CHILD ) - 1;
  UINT32 nNameSize = (nLen + 1) * sizeof( WCHAR );

  UINT32 nDescrSize = 1;

  UINT32 nTotalSize = nHeaderSize + nDescrSize
    + nNameSize + sizeof( ITEMID_CHILD );

  ITEMID_CHILD* pIL = (ITEMID_CHILD*)::CoTaskMemAlloc( nTotalSize );
  ENSURE( pIL );

  ::memset( pIL, 0, nTotalSize );
  
  pIL->mkid.cb = (UINT16)(nHeaderSize + nDescrSize + nNameSize);

  pIL->mkid.abID[ 0 ] = i_fnIsFolder ? 1 : 0;

  ::wcsncpy( (WCHAR*)(& pIL->mkid.abID[ nDescrSize ] ),
    i_bstrName, nLen );

  * o_pIL = pIL;

  return TRUE;
}

HRESULT
CArchiveFoldersEnumIDList::
IEnumIDList_Next( __in  ULONG i_celt,
      __out_ecount_part(i_celt, * o_pceltFetched)
        PITEMID_CHILD* o_prgelt,
      __out_opt ULONG* o_pceltFetched )
{
  if( o_pceltFetched )
  {
    * o_pceltFetched = 0;
  }

  if( ! i_celt )
  {
    return E_INVALIDARG;
  }

  if( ! o_prgelt || (! o_pceltFetched && i_celt != 1) )
  {
    return E_INVALIDARG;
  }

  // lazy init:

  if( m_nFlags & SHCONTF_NONFOLDERS )
  {
    if( ! m_fnFilesListGetted )
    {
      BSTR bstrInFolder = ::SysAllocString( m_pwzInFolderPath );
      if( ! bstrInFolder )
      {
        return E_OUTOFMEMORY;
      }
/*
      if( m_poParent )
      {
        ::MessageBoxW( 0, L"List files", L"Archive folders", 0 );
      }*/

      HRESULT hRes = m_poArchive->ListFiles( bstrInFolder,
        & m_plFilesList, & m_plFilesSizes, & m_plFilesCompSizes,
        & m_plPwdProtFlags );

      ::SysFreeString( bstrInFolder );

      if( hRes != S_OK )
      {
        return S_FALSE;
      }

      m_nCountOfFiles = 0;

      if( ! m_plFilesList )
      {
        m_fnFilesListGetted = TRUE;
      }
      else
      { // if there is some files

        ENSURE( ! m_abstrFiles );

        if( S_OK != ::SafeArrayAccessData(
              m_plFilesList, (void**) & m_abstrFiles ) )
        {
          ::SafeArrayDestroy( m_plFilesList );
          m_plFilesList = 0;
          m_abstrFiles = 0;
          m_nCountOfFiles = 0;
          return S_FALSE;
        }

        LONG nMaxFileIndex = 0;
        if( S_OK
          == ::SafeArrayGetUBound( m_plFilesList, 1, & nMaxFileIndex ) )
        {
          m_nCountOfFiles = (UINT32)nMaxFileIndex + 1;
        }
        else
        {
          ::SafeArrayDestroy( m_plFilesList );
          m_plFilesList = 0;
          m_abstrFiles = 0;
          return S_FALSE;
        }

        m_fnFilesListGetted = TRUE;
      }
    }
  }

  if( m_nFlags & SHCONTF_FOLDERS )
  {
    if( ! m_fnFoldersListGetted )
    {
      BSTR bstrInFolder = ::SysAllocString( m_pwzInFolderPath );
      if( ! bstrInFolder )
      {
        return E_OUTOFMEMORY;
      }

      HRESULT hRes = m_poArchive->ListFolders( bstrInFolder,
        & m_plFoldersList );

      ::SysFreeString( bstrInFolder );

      if( hRes != S_OK )
      {
        return S_FALSE;
      }

      m_nCountOfFolders = 0;

      if( ! m_plFoldersList )
      {
        m_fnFoldersListGetted = TRUE;
      }
      else
      { // if there is some files

        ENSURE( ! m_abstrFolders );

        if( S_OK != ::SafeArrayAccessData(
              m_plFoldersList, (void**) & m_abstrFolders ) )
        {
          ::SafeArrayDestroy( m_plFoldersList );
          m_plFoldersList = 0;
          m_abstrFolders = 0;
          return S_FALSE;
        }

        LONG nMaxFolderIndex = 0;
        if( S_OK
          == ::SafeArrayGetUBound(
                m_plFoldersList, 1, & nMaxFolderIndex ) )
        {
          m_nCountOfFolders = (UINT32)nMaxFolderIndex + 1;
        }
        else
        {
          ::SafeArrayDestroy( m_plFoldersList );
          m_plFoldersList = 0;
          m_abstrFolders = 0;
          return S_FALSE;
        }

        m_fnFoldersListGetted = TRUE;
      }
    }
  }

  m_nTotalCount = m_nCountOfFiles + m_nCountOfFolders;

  if( ! m_nTotalCount )
  {
    return S_FALSE;
  }
  else
  {
    if( m_nCurrItem == m_nTotalCount )
    {
      m_nCurrItem = 0;
      return S_FALSE;
    }
  }

  if( m_nCountOfFolders )
  {
    if( m_nCurrItem < m_nCountOfFolders )
    {
      VERIFY( CreateSimpleIDList(
        m_abstrFolders[ m_nCurrItem ++ ], TRUE, o_prgelt ) );
      return S_OK;
    }
  }

  if( m_nCountOfFiles )
  {
    if( m_nCurrItem >= m_nCountOfFolders )
    {
      VERIFY( CreateSimpleIDList(
        m_abstrFiles[ (m_nCurrItem - m_nCountOfFolders) ], FALSE, o_prgelt ) );
      ++ m_nCurrItem;

      return S_OK;
    }
  }

  return S_FALSE;
}

HRESULT
CArchiveFoldersEnumIDList::
IEnumIDList_Skip( ULONG i_celt )
{
  //::MessageBoxW( 0, L"", L"IEnumIDList::Skip", 0 );

  if( ! m_nTotalCount )
  {
    return S_FALSE;
  }
  else
  {
    if( m_nCurrItem == m_nTotalCount )
    {
      m_nCurrItem = 0;
      return S_FALSE;
    }
  }

  m_nCurrItem += i_celt;
  if( m_nCurrItem >= m_nTotalCount )
  {
    m_nCurrItem = 0;
    return S_FALSE;
  }

  return S_OK;
}

HRESULT
CArchiveFoldersEnumIDList::
IEnumIDList_Reset( void )
{
  m_nCurrItem = 0;

  //::MessageBoxW( 0, L"", L"IEnumIDList::Next", 0 );

  return S_OK;
}

HRESULT
CArchiveFoldersEnumIDList::
IEnumIDList_Clone( __RPC__deref_out_opt IEnumIDList** o_ppIEnumIDList )
{
  //::MessageBoxW( 0, L"", L"IEnumIDList::Clone", 0 );

  return E_NOTIMPL;
}

CArchiveFoldersEnumIDList::
CArchiveFoldersEnumIDList(
  class CArchiveFolders* i_poParent,
  ICodexArchive* i_poArchive,
  SHCONTF i_nFlags,
  const WCHAR* i_pwzInFolderPath ) :

  m_nCurrItem( 0 ),
  m_nTotalCount( 0 ),
  m_nFlags( i_nFlags ),

  // Parent itself does AddRef()!
  m_poArchive( i_poArchive ),
  // Parent itself does AddRef()!
  m_poParent( i_poParent ),

  m_plFilesList( 0 ),
  m_plFoldersList( 0 ),
  m_plFilesSizes( 0 ),
  m_plFilesCompSizes( 0 ),
  m_plPwdProtFlags( 0 ),
  m_pwzInFolderPath( i_pwzInFolderPath ),
  m_abstrFiles( 0 ),
  m_nCountOfFiles( 0 ),
  m_abstrFolders( 0 ),
  m_nCountOfFolders( 0 ),
  m_fnFoldersListGetted( FALSE ),
  m_fnFilesListGetted( FALSE )
{
  
}

CArchiveFoldersEnumIDList::
~CArchiveFoldersEnumIDList()
{
  if( m_abstrFiles )
  {
    if( m_plFilesList )
    {
      ::SafeArrayUnaccessData( m_plFilesList );
      ::SafeArrayDestroy( m_plFilesList );
      m_plFilesList = 0;
      m_abstrFiles = 0;
    }
  }

  if( m_abstrFolders )
  {
    if( m_plFoldersList )
    {
      ::SafeArrayUnaccessData( m_plFoldersList );
      ::SafeArrayDestroy( m_plFoldersList );
      m_plFoldersList = 0;
      m_abstrFolders = 0;
    }
  }

  m_poArchive->Release();

  m_poParent->IUnknown_Release();
}
