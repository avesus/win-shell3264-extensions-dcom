# pragma once

# include "./../../../avesus_commons/i_shell_view.h"
# include "archive_folders.h"

# include "./../codexsrv/com/codexsrv.h"

class CArchiveFoldersShellView :
  IMPLEMENTS( IUnknown ),
  IMPLEMENTS( IShellView2 ),
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
  IOleWindow_GetWindow( 
            /* [out] */ __RPC__deref_out_opt HWND *phwnd);
        
  virtual HRESULT
  IOleWindow_ContextSensitiveHelp( 
            /* [in] */ BOOL fEnterMode);


  virtual HRESULT
  IShellView_TranslateAccelerator( 
      /* [in] */ __RPC__in MSG *pmsg);

  virtual HRESULT
  IShellView_EnableModeless( 
    /* [in] */ BOOL fEnable);

  virtual HRESULT
  IShellView_UIActivate( 
    /* [in] */ UINT uState);

  virtual HRESULT
  IShellView_Refresh( void);

  virtual HRESULT
  IShellView_CreateViewWindow( 
      /* [unique][in] */ __RPC__in_opt IShellView *psvPrevious,
      /* [in] */ __RPC__in LPCFOLDERSETTINGS pfs,
      /* [in] */ __RPC__in_opt IShellBrowser *psb,
      /* [in] */ __RPC__in RECT *prcView,
      /* [out] */ __RPC__deref_out_opt HWND *phWnd);

  virtual HRESULT
  IShellView_DestroyViewWindow( void );

  virtual HRESULT
  IShellView_GetCurrentInfo( 
    /* [out] */ __RPC__out LPFOLDERSETTINGS pfs);

  virtual HRESULT
  IShellView_AddPropertySheetPages( 
      /* [in] */ 
      __in  DWORD dwReserved,
      /* [in] */ 
      __in  LPFNSVADDPROPSHEETPAGE pfn,
      /* [in] */ 
      __in  LPARAM lparam);

  virtual HRESULT
  IShellView_SaveViewState( void);

  virtual HRESULT
  IShellView_SelectItem( 
      /* [unique][in] */ __RPC__in_opt PCUITEMID_CHILD pidlItem,
      /* [in] */ SVSIF uFlags);

  virtual HRESULT
  IShellView_GetItemObject( 
      /* [in] */ UINT uItem,
      /* [in] */ __RPC__in REFIID riid,
      /* [iid_is][out] */ __RPC__deref_out_opt void **ppv);

  virtual HRESULT
  IShellView2_GetView( 
      /* [out][in] */ __RPC__inout SHELLVIEWID *pvid,
      /* [in] */ ULONG uView);

  virtual HRESULT
  IShellView2_CreateViewWindow2( 
    /* [in] */ __RPC__in LPSV2CVW2_PARAMS lpParams);

  virtual HRESULT
  IShellView2_HandleRename( 
      /* [unique][in] */ __RPC__in_opt PCUITEMID_CHILD pidlNew);

  virtual HRESULT
  IShellView2_SelectAndPositionItem( 
      /* [in] */ __RPC__in PCUITEMID_CHILD pidlItem,
      /* [in] */ UINT uFlags,
      /* [unique][in] */ __RPC__in_opt POINT *ppt);

  CArchiveFoldersShellView( IShellView* i_poOuterShellView );
  virtual ~CArchiveFoldersShellView();

  BOOL GetShellBrowser( IShellBrowser** o_ppoShellBrowser );

private:

  IShellView* m_poShellView;
  IShellView2* m_poShellView2;
  IShellBrowser* m_poShellBrowser;
};