# pragma once

# include "./../../../avesus_commons/avedbg.h"
# include "./../../../avesus_commons/aveutils.h"
# include "./../../../avesus_commons/avecom.h"

# include "./com/codexsrv.h"

# include "codex_api.h"

# include "i_codex_archive.h"
# include "i_codex_cpl.h"

# include "parse_tree.h"

class CodexSrv :
  IMPLEMENTS( IUnknown ),
  IMPLEMENTS( ICodexArchive ),
  IMPLEMENTS( ICodexControlPanel ),
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
  ICodexArchive_OpenArchive(
    __in  BSTR i_bstrArchiveFilePath,
    __out BSTR* o_pbstrErrorText );
  
  virtual HRESULT
  ICodexArchive_ListFiles( 
      __in  BSTR i_bstrInFolder,
      __out SAFEARRAY** o_plbstrFiles,
      __out SAFEARRAY** o_plqFilesSizes,
      __out SAFEARRAY** o_plqFilesCompSizes,
      __out SAFEARRAY** o_plfPwdProtectedFlags);
  
  virtual HRESULT
  ICodexArchive_ListFolders( 
      __in  BSTR i_bstrInFolder,
      __out SAFEARRAY** o_plbstrFolders );
  
  virtual HRESULT
  ICodexArchive_CloseArchive( void );

  virtual HRESULT
  ICodexControlPanel_Show( __in __int64 i_hwndCPl );

private:

  static UINT32 CALLBACK ParentWndAssigner( void* i_pParam );

public:

  CodexSrv();

  virtual ~CodexSrv();

private:

  CCodexApiDll m_oCodexApiDll;

  HWND m_hWndParent;


  CAutoArray< CHAR > m_aczItems;

  CAutoArray< CHAR > m_aczPwdItems;

  // not requested in current version
  // because of plugins access violations
  // when the system data format other than EUROPEAN!!!
  CAutoArray< CHAR > m_aczDateTimes;

  CAutoArray< CHAR > m_aczSizes;

  CAutoArray< CHAR > m_aczCompSizes;

  CParseTree< CHAR > m_oFilesTree;

  BOOL m_fnOpened;
};
