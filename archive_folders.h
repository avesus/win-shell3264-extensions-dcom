# pragma once

# include "./../../../avesus_commons/avecom.h"
# include "./../../../avesus_commons/i_shell_folder.h"
# include "./../../../avesus_commons/i_persist_folder.h"
# include "./../../../avesus_commons/i_shell_folder_view_cb.h"
# include "./../../../avesus_commons/i_explorer_cmd_provider.h"

# include "./../codexsrv/com/codexsrv.h"

//# include "af_enum_id_list.h"


class
  __declspec( uuid( "AF08BC20-466C-4685-8BDC-9DB713B2A64B" ) )
CArchiveFolders :
  IMPLEMENTS( IUnknown ),

  IMPLEMENTS( IShellFolder ),
  IMPLEMENTS( IShellFolder2 ),

  IMPLEMENTS( IPersist ),
  IMPLEMENTS( IPersistFolder ),
  IMPLEMENTS( IPersistFolder2 ),
  IMPLEMENTS( IPersistFolder3 ),
  IMPLEMENTS( IPersistIDList ),

  IMPLEMENTS( IShellFolderViewCB ),
  IMPLEMENTS( IExplorerCommandProvider ),
  IMPLEMENTS( IEnumExplorerCommand ),
 
  public CRefCounted
{
public:

  CArchiveFolders();

  virtual ~CArchiveFolders();

  virtual HRESULT
	IUnknown_QueryInterface(
	  __in  const IID &  i_poReqIfaceId,
	  __out       void** o_ppoIface );

  virtual ULONG
  IUnknown_AddRef( void );

  virtual ULONG
  IUnknown_Release( void );

  virtual HRESULT
	IShellFolder_ParseDisplayName(
		__RPC__in_opt HWND hwnd,
		__RPC__in_opt IBindCtx *pbc,
		__RPC__in LPWSTR pszDisplayName,
		__reserved  ULONG *pchEaten,
		__RPC__deref_out_opt PIDLIST_RELATIVE *ppidl,
		__RPC__inout_opt ULONG *pdwAttributes );

  //@ worked
  virtual HRESULT
	IShellFolder_EnumObjects(
		__RPC__in_opt HWND i_hwnd,
		SHCONTF i_grfFlags,
		__RPC__deref_out_opt IEnumIDList** o_ppIEnumIDList );
      
	virtual HRESULT
	IShellFolder_BindToObject( 
		__RPC__in PCUIDLIST_RELATIVE i_poItemId,
		__RPC__in_opt IBindCtx* i_poIBindCtx,
		__RPC__in REFIID i_poReqShellFolderIface,
		__RPC__deref_out_opt void** o_ppIShellFolder );

	virtual HRESULT
	IShellFolder_BindToStorage( 
			/* [in] */ __RPC__in PCUIDLIST_RELATIVE pidl,
			/* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
			/* [in] */ __RPC__in REFIID riid,
			/* [iid_is][out] */ __RPC__deref_out_opt void **ppv );
  
	virtual HRESULT
  IShellFolder_CompareIDs(
			/* [in] */ LPARAM lParam,
			/* [in] */ __RPC__in PCUIDLIST_RELATIVE pidl1,
			/* [in] */ __RPC__in PCUIDLIST_RELATIVE pidl2 );
  
  //@ worked
	virtual HRESULT
  IShellFolder_CreateViewObject( 
	  __RPC__in_opt HWND i_hwndOwner,
	  __RPC__in REFIID i_poReqIfaceId,
	  __RPC__deref_out_opt void** o_ppIShellView );

	virtual HRESULT
  IShellFolder_GetAttributesOf( 
			UINT i_cidl,
			__RPC__in_ecount_full_opt( i_cidl )
        PCUITEMID_CHILD_ARRAY i_apidl,
			__RPC__inout SFGAOF* io_prgfInOut );
  
	virtual HRESULT
  IShellFolder_GetUIObjectOf(
	  __RPC__in_opt HWND i_hwndOwner,
	  UINT i_cidl,
		__RPC__in_ecount_full_opt(i_cidl)
      PCUITEMID_CHILD_ARRAY apidl,
		__RPC__in REFIID i_riid,
		
		__reserved UINT* io_rgfReserved,
		__RPC__deref_out_opt void** o_ppv );
  
	virtual HRESULT
  IShellFolder_GetDisplayNameOf( 
			/* [in] */ __RPC__in PCUITEMID_CHILD pidl,
			/* [in] */ SHGDNF uFlags,
			/* [out] */ __RPC__out STRRET* o_poName );
  
	virtual HRESULT
  IShellFolder_SetNameOf( 
			/* [unique][in] */ __RPC__in_opt HWND hwnd,
			/* [in] */ __RPC__in PCUITEMID_CHILD pidl,
			/* [string][in] */ __RPC__in LPCWSTR pszName,
			/* [in] */ SHGDNF uFlags,
			/* [out] */ __RPC__deref_out_opt PITEMID_CHILD *ppidlOut );


  // IShellFolder2 INTERFACE

  virtual HRESULT
  IShellFolder2_GetDefaultSearchGUID( 
      /* [out] */ __RPC__out GUID *pguid );
  
  virtual HRESULT
  IShellFolder2_EnumSearches( 
      /* [out] */ __RPC__deref_out_opt IEnumExtraSearch** ppenum );
  
  //@ worked
  virtual HRESULT
  IShellFolder2_GetDefaultColumn( DWORD dwRes,
                    __RPC__out ULONG* o_pnSort,
                    __RPC__out ULONG* o_pnDisplay );
  
  //@ worked
  virtual HRESULT
  IShellFolder2_GetDefaultColumnState( UINT i_iColumn,
                         __RPC__out SHCOLSTATEF* o_pcsFlags );
  
  virtual HRESULT
  IShellFolder2_GetDetailsEx(
    __RPC__in PCUITEMID_CHILD i_pidl,
    __RPC__in const SHCOLUMNID* i_pscid,
    __RPC__out VARIANT* o_pv );
  
  //@ worked
  virtual HRESULT
  IShellFolder2_GetDetailsOf( __RPC__in_opt PCUITEMID_CHILD i_poItemIdList,
                UINT i_nColumn,
                __RPC__out SHELLDETAILS* o_poShellDetails );
  
  //@ worked
  virtual HRESULT
  IShellFolder2_MapColumnToSCID( UINT iColumn,
                   __RPC__out SHCOLUMNID* o_pscid );


  virtual HRESULT
  IPersist_GetClassID(
    __RPC__out CLSID* o_poClsID );

  virtual HRESULT
  IPersistFolder_Initialize( __RPC__in PCIDLIST_ABSOLUTE i_poIdList );

  virtual HRESULT
  IPersistFolder2_GetCurFolder(
    __RPC__deref_out_opt PIDLIST_ABSOLUTE* o_ppoIdList );

  virtual HRESULT
  IShellFolderViewCB_MessageSFVCB(
    UINT uMsg, WPARAM wParam, LPARAM lParam );

  virtual HRESULT
  IExplorerCommandProvider_GetCommands( 
    /* [in] */ __RPC__in_opt IUnknown *punkSite,
    /* [in] */ __RPC__in REFIID riid,
    /* [iid_is][out] */ __RPC__deref_out_opt void **ppv );

  virtual HRESULT
  IExplorerCommandProvider_GetCommand( 
    /* [in] */ __RPC__in REFGUID rguidCommandId,
    /* [in] */ __RPC__in REFIID riid,
    /* [iid_is][out] */ __RPC__deref_out_opt void **ppv );

  virtual HRESULT
  IEnumExplorerCommand_Next( 
      ULONG celt,
      __out IExplorerCommand** pUICommand,
      ULONG* pceltFetched );

  virtual HRESULT
  IEnumExplorerCommand_Skip( ULONG i_celt );
  
  virtual HRESULT
  IEnumExplorerCommand_Reset( void );

  virtual HRESULT
  IEnumExplorerCommand_Clone( IEnumExplorerCommand **ppenum );

  virtual HRESULT
  IPersistIDList_SetIDList( __RPC__in PCIDLIST_ABSOLUTE i_poIdList );

  virtual HRESULT
  IPersistIDList_GetIDList(
    __RPC__deref_out_opt PIDLIST_ABSOLUTE* o_ppoIdList );

  virtual HRESULT
  IPersistFolder3_InitializeEx( 
    __RPC__in_opt IBindCtx *pbc,
    __RPC__in PCIDLIST_ABSOLUTE pidlRoot,
    __RPC__in_opt const PERSIST_FOLDER_TARGET_INFO *ppfti );
        
  virtual HRESULT
  IPersistFolder3_GetFolderTargetInfo( 
    __RPC__out PERSIST_FOLDER_TARGET_INFO *ppfti );

  BOOL GetCodex( ICodexArchive** o_ppICodex );

private:

//  class CArchiveFolders* m_poParent;

//  IShellView* m_poIShellView;

  LPITEMIDLIST GetItemIDList( void );

  LPITEMIDLIST GetNextItemID( LPCITEMIDLIST pidl );

  UINT GetSize( LPCITEMIDLIST pidl );


  LPITEMIDLIST m_poIdList;

//  LPITEMIDLIST m_poPersistIdList;

  PERSIST_FOLDER_TARGET_INFO m_oPersistFolder3Info;


  ICodexArchive* m_poICodex;

  class CCodexAccess* m_poCodexAccess;

  CRITICAL_SECTION m_oCritSect;

  class CArchiveFoldersEnumIDList* m_poEnumIdList;

  // Path to archive file
  WCHAR m_awPathBuffer[ MAX_PATH + 3 ];

  WCHAR m_awzInArchivePath[ MAX_PATH ];

  BSTR m_bstrPath;

  class CArchiveFoldersShellView* m_poView;

  BOOL m_fnStdColumnsGetted;

  SHELLDETAILS m_aoStdColumns[ 3 ];

  SHELLDETAILS* m_poNameColumn;
  SHELLDETAILS* m_poSizeColumn;
  SHELLDETAILS* m_poTypeColumn;

  FOLDERSETTINGS m_oFolderSettings;
};
