# include "af_shell_view.h"

CArchiveFoldersShellView::CArchiveFoldersShellView(
  IShellView* i_poOuterShellView ) :
  m_poShellView( i_poOuterShellView ),
  m_poShellView2( 0 ),
  m_poShellBrowser( 0 )
{
}

CArchiveFoldersShellView::~CArchiveFoldersShellView()
{
  if( m_poShellView2 )
  {
    m_poShellView2->Release();
  }

  if( m_poShellView )
  {
    m_poShellView->Release();
  }
}

BOOL
CArchiveFoldersShellView::
GetShellBrowser( IShellBrowser** o_ppoShellBrowser )
{
  ENSURE( o_ppoShellBrowser );

  * o_ppoShellBrowser = m_poShellBrowser;

  return TRUE;
}

HRESULT
CArchiveFoldersShellView::
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
  else if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IShellView ) ) )
	{
		* o_ppoIface
      = static_cast< IShellView* >(
          static_cast< IShellView2_wrap* >( this ) );

		IUnknown_AddRef();
	}
  else if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IShellView2 ) ) )
	{
    if( ! m_poShellView2 )
    {
      IShellView2* poView2 = 0;
      HRESULT hRes = E_UNEXPECTED;
      hRes = m_poShellView->QueryInterface(
        i_poReqIfaceId, (void**) & poView2 );
      if( poView2 )
      {
        m_poShellView2 = poView2;
      }
      else
      {
        return hRes;
      }
    }

		* o_ppoIface
      = static_cast< IShellView2* >(
          static_cast< IShellView2_wrap* >( this ) );

		IUnknown_AddRef();
	}
  else
	{
		return m_poShellView->QueryInterface( i_poReqIfaceId, o_ppoIface );
	}

	return S_OK;
}

ULONG
CArchiveFoldersShellView::
IUnknown_AddRef( void )
{
  return CRefCounted_AddRef();
}

ULONG
CArchiveFoldersShellView::
IUnknown_Release( void )
{
  return CRefCounted_Release();
}

HRESULT CArchiveFoldersShellView::
IOleWindow_GetWindow( 
          /* [out] */ __RPC__deref_out_opt HWND *phwnd)
{
  return m_poShellView->GetWindow( phwnd );
}
      
HRESULT CArchiveFoldersShellView::
IOleWindow_ContextSensitiveHelp( 
          /* [in] */ BOOL fEnterMode)
{
  return m_poShellView->ContextSensitiveHelp( fEnterMode );
}


HRESULT CArchiveFoldersShellView::
IShellView_TranslateAccelerator( 
    /* [in] */ __RPC__in MSG *pmsg)
{
  return m_poShellView->TranslateAccelerator( pmsg );
}

HRESULT CArchiveFoldersShellView::
IShellView_EnableModeless( 
  /* [in] */ BOOL fEnable)
{
  return m_poShellView->EnableModeless( fEnable );
}

HRESULT CArchiveFoldersShellView::
IShellView_UIActivate( 
  /* [in] */ UINT uState)
{
  return m_poShellView->UIActivate( uState );
}

HRESULT CArchiveFoldersShellView::
IShellView_Refresh( void )
{
  return m_poShellView->Refresh();
}

//extern IShellBrowser* g_poShellBrowser = 0;

HRESULT CArchiveFoldersShellView::
IShellView_CreateViewWindow( 
    /* [unique][in] */ __RPC__in_opt IShellView *psvPrevious,
    /* [in] */ __RPC__in LPCFOLDERSETTINGS pfs,
    /* [in] */ __RPC__in_opt IShellBrowser *psb,
    /* [in] */ __RPC__in RECT *prcView,
    /* [out] */ __RPC__deref_out_opt HWND *phWnd )
{
  ::MessageBoxW( 0, L"IShellView_CreateViewWindow",
      L"Archive Folders", 0 );

  g_poShellBrowser = m_poShellBrowser = psb;
  return m_poShellView->CreateViewWindow( psvPrevious, pfs, psb,
    prcView, phWnd );
}

HRESULT CArchiveFoldersShellView::
IShellView_DestroyViewWindow( void )
{
  return m_poShellView->DestroyViewWindow();
}

HRESULT CArchiveFoldersShellView::
IShellView_GetCurrentInfo( 
  /* [out] */ __RPC__out LPFOLDERSETTINGS pfs)
{
  return m_poShellView->GetCurrentInfo( pfs );
}

HRESULT CArchiveFoldersShellView::
IShellView_AddPropertySheetPages( 
    /* [in] */ 
    __in  DWORD dwReserved,
    /* [in] */ 
    __in  LPFNSVADDPROPSHEETPAGE pfn,
    /* [in] */ 
    __in  LPARAM lparam)
{
  return m_poShellView->AddPropertySheetPages(
    dwReserved, pfn, lparam );
}

HRESULT CArchiveFoldersShellView::
IShellView_SaveViewState( void)
{
  return m_poShellView->SaveViewState();
}

HRESULT CArchiveFoldersShellView::
IShellView_SelectItem( 
    /* [unique][in] */ __RPC__in_opt PCUITEMID_CHILD pidlItem,
    /* [in] */ SVSIF uFlags)
{
  return m_poShellView->SelectItem( pidlItem, uFlags );
}

HRESULT CArchiveFoldersShellView::
IShellView_GetItemObject( 
    /* [in] */ UINT uItem,
    /* [in] */ __RPC__in REFIID riid,
    /* [iid_is][out] */ __RPC__deref_out_opt void **ppv)
{
  return m_poShellView->GetItemObject( uItem, riid, ppv );
}

HRESULT CArchiveFoldersShellView::
IShellView2_GetView( 
    /* [out][in] */ __RPC__inout SHELLVIEWID *pvid,
    /* [in] */ ULONG uView)
{
  if( ! m_poShellView2 )
  {
    return E_NOTIMPL;
  }

  return m_poShellView2->GetView( pvid, uView );
}

HRESULT CArchiveFoldersShellView::
IShellView2_CreateViewWindow2( 
  /* [in] */ __RPC__in LPSV2CVW2_PARAMS lpParams)
{
  if( ! m_poShellView2 )
  {
    return E_NOTIMPL;
  }

  return m_poShellView2->CreateViewWindow2( lpParams );
}

HRESULT CArchiveFoldersShellView::
IShellView2_HandleRename( 
    /* [unique][in] */ __RPC__in_opt PCUITEMID_CHILD pidlNew)
{
  if( ! m_poShellView2 )
  {
    return E_NOTIMPL;
  }

  return m_poShellView2->HandleRename( pidlNew );
}

HRESULT CArchiveFoldersShellView::
IShellView2_SelectAndPositionItem( 
    /* [in] */ __RPC__in PCUITEMID_CHILD pidlItem,
    /* [in] */ UINT uFlags,
    /* [unique][in] */ __RPC__in_opt POINT *ppt)
{
  if( ! m_poShellView2 )
  {
    return E_NOTIMPL;
  }

  return m_poShellView2->SelectAndPositionItem(
    pidlItem, uFlags, ppt );
}
