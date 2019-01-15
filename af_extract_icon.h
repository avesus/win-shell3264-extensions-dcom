# pragma once

# include "./../../../avesus_commons/i_extract_icon_w.h"
# include "archive_folders.h"

# include "./../codexsrv/com/codexsrv.h"

class CArchiveFoldersExtractIcon :
  IMPLEMENTS( IUnknown ),
  IMPLEMENTS( IExtractIconW ),
  public CRefCounted
{
public:

  virtual HRESULT
	IUnknown_QueryInterface(
	  __in  const IID &  i_poReqIfaceId,
	  __out       void** o_ppoIface );

  virtual ULONG
  IUnknown_AddRef( void );

  virtual ULONG
  IUnknown_Release( void );

  virtual HRESULT
  IExtractIconW_GetIconLocation(
                         UINT uFlags,
    __out_ecount(cchMax) LPWSTR pszIconFile,
                         UINT   cchMax,
                   __out int*   piIndex,
                   __out UINT*  pwFlags );

  virtual HRESULT
  IExtractIconW_Extract(
    LPCWSTR pszFile,
    UINT    nIconIndex,
    __out_opt HICON   *phiconLarge,
    __out_opt HICON   *phiconSmall,
    UINT    nIconSize );

  CArchiveFoldersExtractIcon(
    const WCHAR* i_pwzIconPath,
    INT32 i_nIconIndex );

  virtual ~CArchiveFoldersExtractIcon();

private:

  WCHAR m_awzIconPath[ MAX_PATH ];
  INT32 m_nIconIndex;
};