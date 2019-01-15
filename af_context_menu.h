# pragma once

# include "./../../../avesus_commons/i_context_menu.h"
# include "./../../../avesus_commons/i_shell_ext_init.h"
# include "archive_folders.h"

# include "./../codexsrv/com/codexsrv.h"

class CArchiveFoldersContextMenu :
  IMPLEMENTS( IUnknown ),
  IMPLEMENTS( IContextMenu ),
  IMPLEMENTS( IShellExtInit ),
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
  IShellExtInit_Initialize( 
    __in_opt  PCIDLIST_ABSOLUTE pidlFolder,
    __in_opt  IDataObject *pdtobj,
    __in_opt  HKEY hkeyProgID );

  virtual HRESULT
  IContextMenu_QueryContextMenu( 
      __in  HMENU hmenu,
      __in  UINT indexMenu,
      __in  UINT idCmdFirst,
      __in  UINT idCmdLast,
      __in  UINT uFlags );


  virtual HRESULT
  IContextMenu_InvokeCommand( 
      __in  CMINVOKECOMMANDINFO *pici );

  virtual HRESULT
  IContextMenu_GetCommandString( 
    __in  UINT_PTR idCmd,
    __in  UINT uType,
    __reserved  UINT *pReserved,
    __out_awcount(!(uType & GCS_UNICODE), cchMax)  LPSTR pszName,
    __in  UINT cchMax );

  CArchiveFoldersContextMenu(
    WCHAR* i_pwzObjectName, WCHAR* i_pwzPath,
    IShellBrowser* i_poBrowser );

  virtual ~CArchiveFoldersContextMenu();

private:

  WCHAR m_awzObjectName[ MAX_PATH ];

  WCHAR m_awzObjectPath[ 1024 ];

  IShellBrowser* m_poShellBrowser;
};
