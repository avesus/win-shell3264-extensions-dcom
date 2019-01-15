# include "af_extract_icon.h"

# pragma comment( lib, "comctl32.lib" )

CArchiveFoldersExtractIcon::
CArchiveFoldersExtractIcon(
  const WCHAR* i_pwzIconPath,
  INT32 i_nIconIndex ) :
  m_nIconIndex( i_nIconIndex )
{
  if( i_pwzIconPath[ 0 ] )
  {
    ::wcscpy( m_awzIconPath, i_pwzIconPath );
  }
  else
  {
    m_awzIconPath[ 0 ] = 0;
  }
}

CArchiveFoldersExtractIcon::
~CArchiveFoldersExtractIcon()
{
}

HRESULT
CArchiveFoldersExtractIcon::
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
  else if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IExtractIconW ) ) )
	{
		* o_ppoIface
      = static_cast< IExtractIconW* >(
          static_cast< IExtractIconW_wrap* >( this ) );

		IUnknown_AddRef();
	}
  else
	{
		return E_NOINTERFACE;
	}

	return S_OK;
}

ULONG
CArchiveFoldersExtractIcon::
IUnknown_AddRef( void )
{
  return CRefCounted_AddRef();
}

ULONG
CArchiveFoldersExtractIcon::
IUnknown_Release( void )
{
  return CRefCounted_Release();
}

HRESULT
CArchiveFoldersExtractIcon::
IExtractIconW_GetIconLocation(
                         UINT uFlags,
    __out_ecount(cchMax) LPWSTR pszIconFile,
                         UINT   cchMax,
                   __out int*   piIndex,
                   __out UINT*  pwFlags )
{
  if( m_awzIconPath[ 0 ] )
  {
    ::wcscpy( pszIconFile, m_awzIconPath );
    * pwFlags = GIL_PERCLASS;
  }
  else
  {
    pszIconFile[ 0 ] = 0;
    * pwFlags = GIL_PERCLASS | GIL_NOTFILENAME;
  }

  * piIndex = m_nIconIndex;
  
  return S_OK;
}

HRESULT
CArchiveFoldersExtractIcon::
IExtractIconW_Extract(
    LPCWSTR pszFile,
    UINT    nIconIndex,
    __out_opt HICON   *phiconLarge,
    __out_opt HICON   *phiconSmall,
    UINT    nIconSize )
{
  if( ! pszFile[ 0 ] )
  {
    HIMAGELIST hNormal = 0;
    HIMAGELIST hSmall = 0;
    if( ! ::Shell_GetImageLists( & hNormal, & hSmall ) )
    {
      return E_UNEXPECTED;
    }

    * phiconLarge
      = ::ImageList_GetIcon( hNormal, nIconIndex, ILD_NORMAL );

    * phiconSmall
      = ::ImageList_GetIcon( hSmall, nIconIndex, ILD_NORMAL );

    return S_OK;
  }

  return ::SHDefExtractIcon( pszFile, nIconIndex, 0, phiconLarge,
    phiconSmall, nIconSize );
}

