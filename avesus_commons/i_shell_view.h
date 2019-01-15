# pragma once

# include "avecom.h"

# include <shobjidl.h>

class
IShellView2_wrap :
  public IUnknown_wrap,
  public IShellView2
{
public:

  IFACE_METHOD( HRESULT,
    IUnknown,
    QueryInterface, ( REFIID riid, void** ppvObject ),
      ( riid, ppvObject ) );

  IFACE_METHOD( ULONG,
    IUnknown,
    AddRef, ( void ), () );

  IFACE_METHOD( ULONG,
    IUnknown,
    Release, ( void ), () );

  IFACE_METHOD( HRESULT,
    IOleWindow,
    GetWindow, ( 
            /* [out] */ __RPC__deref_out_opt HWND *phwnd),
            ( phwnd ) );
        
  IFACE_METHOD( HRESULT,
    IOleWindow,
    ContextSensitiveHelp, ( 
            /* [in] */ BOOL fEnterMode),
            ( fEnterMode ) );


  IFACE_METHOD( HRESULT,
    IShellView,
    TranslateAccelerator, ( 
      /* [in] */ __RPC__in MSG *pmsg),
            ( pmsg ) );

  IFACE_METHOD( HRESULT,
    IShellView,
    EnableModeless, ( 
    /* [in] */ BOOL fEnable),
            ( fEnable ) );

  IFACE_METHOD( HRESULT,
    IShellView,
    UIActivate, ( 
    /* [in] */ UINT uState),
            ( uState ) );

  IFACE_METHOD( HRESULT,
    IShellView,
    Refresh, ( void),
            (  ) );

  IFACE_METHOD( HRESULT,
    IShellView,
    CreateViewWindow, ( 
      /* [unique][in] */ __RPC__in_opt IShellView *psvPrevious,
      /* [in] */ __RPC__in LPCFOLDERSETTINGS pfs,
      /* [in] */ __RPC__in_opt IShellBrowser *psb,
      /* [in] */ __RPC__in RECT *prcView,
      /* [out] */ __RPC__deref_out_opt HWND *phWnd),
            ( psvPrevious, pfs, psb, prcView, phWnd ) );

  IFACE_METHOD( HRESULT,
    IShellView,
    DestroyViewWindow, ( void),
            (  ) );

  IFACE_METHOD( HRESULT,
    IShellView,
    GetCurrentInfo, ( 
    /* [out] */ __RPC__out LPFOLDERSETTINGS pfs),
            ( pfs ) );

  IFACE_METHOD( HRESULT,
    IShellView,
    AddPropertySheetPages, ( 
      /* [in] */ 
      __in  DWORD dwReserved,
      /* [in] */ 
      __in  LPFNSVADDPROPSHEETPAGE pfn,
      /* [in] */ 
      __in  LPARAM lparam),
            ( dwReserved, pfn, lparam ) );

  IFACE_METHOD( HRESULT,
    IShellView,
    SaveViewState, ( void),
            (  ) );

  IFACE_METHOD( HRESULT,
    IShellView,
    SelectItem, ( 
      /* [unique][in] */ __RPC__in_opt PCUITEMID_CHILD pidlItem,
      /* [in] */ SVSIF uFlags),
            ( pidlItem, uFlags ) );

  IFACE_METHOD( HRESULT,
    IShellView,
    GetItemObject, ( 
      /* [in] */ UINT uItem,
      /* [in] */ __RPC__in REFIID riid,
      /* [iid_is][out] */ __RPC__deref_out_opt void **ppv),
            ( uItem, riid, ppv ) );

  IFACE_METHOD( HRESULT,
    IShellView2,
    GetView, ( 
      /* [out][in] */ __RPC__inout SHELLVIEWID *pvid,
      /* [in] */ ULONG uView),
            ( pvid, uView ) );

  IFACE_METHOD( HRESULT,
    IShellView2,
    CreateViewWindow2, ( 
    /* [in] */ __RPC__in LPSV2CVW2_PARAMS lpParams),
            ( lpParams ) );

  IFACE_METHOD( HRESULT,
    IShellView2,
    HandleRename, ( 
      /* [unique][in] */ __RPC__in_opt PCUITEMID_CHILD pidlNew),
            ( pidlNew ) );

  IFACE_METHOD( HRESULT,
    IShellView2,
    SelectAndPositionItem, ( 
      /* [in] */ __RPC__in PCUITEMID_CHILD pidlItem,
      /* [in] */ UINT uFlags,
      /* [unique][in] */ __RPC__in_opt POINT *ppt),
            ( pidlItem, uFlags, ppt ) );
};
