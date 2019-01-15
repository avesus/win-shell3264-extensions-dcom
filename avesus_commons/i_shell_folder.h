# pragma once

# include "avecom.h"

# include <shobjidl.h>

class
IShellFolder_wrap :
  public IUnknown_wrap,
  public IShellFolder
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
    IShellFolder,
    ParseDisplayName, (
      /* [unique][in] */ __RPC__in_opt HWND hwnd,
      /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
      /* [string][in] */ __RPC__in LPWSTR pszDisplayName,
      /* [unique][out][in] */ 
      __reserved  ULONG *pchEaten,
      /* [out] */ __RPC__deref_out_opt PIDLIST_RELATIVE *ppidl,
      /* [unique][out][in] */ __RPC__inout_opt ULONG *pdwAttributes ),
      ( hwnd, pbc, pszDisplayName, pchEaten, ppidl, pdwAttributes ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder,
    EnumObjects, ( 
      /* [unique][in] */ __RPC__in_opt HWND hwnd,
      /* [in] */ SHCONTF grfFlags,
      /* [out] */ __RPC__deref_out_opt IEnumIDList **ppenumIDList ),
      ( hwnd, grfFlags, ppenumIDList ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder,
    BindToObject, ( 
      /* [in] */ __RPC__in PCUIDLIST_RELATIVE pidl,
      /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
      /* [in] */ __RPC__in REFIID riid,
      /* [iid_is][out] */ __RPC__deref_out_opt void **ppv ),
      ( pidl, pbc, riid, ppv ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder,
    BindToStorage, ( 
      /* [in] */ __RPC__in PCUIDLIST_RELATIVE pidl,
      /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
      /* [in] */ __RPC__in REFIID riid,
      /* [iid_is][out] */ __RPC__deref_out_opt void **ppv ),
      ( pidl, pbc, riid, ppv ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder,
    CompareIDs, ( 
      /* [in] */ LPARAM lParam,
      /* [in] */ __RPC__in PCUIDLIST_RELATIVE pidl1,
      /* [in] */ __RPC__in PCUIDLIST_RELATIVE pidl2 ),
      ( lParam, pidl1, pidl2 ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder,
    CreateViewObject, ( 
      /* [unique][in] */ __RPC__in_opt HWND hwndOwner,
      /* [in] */ __RPC__in REFIID riid,
      /* [iid_is][out] */ __RPC__deref_out_opt void **ppv ),
      ( hwndOwner, riid, ppv ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder,
    GetAttributesOf, ( 
      /* [in] */ UINT cidl,
      /* [unique][size_is][in] */ __RPC__in_ecount_full_opt(cidl) PCUITEMID_CHILD_ARRAY apidl,
      /* [out][in] */ __RPC__inout SFGAOF *rgfInOut ),
      ( cidl, apidl, rgfInOut ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder,
    GetUIObjectOf, ( 
      /* [unique][in] */ __RPC__in_opt HWND hwndOwner,
      /* [in] */ UINT cidl,
      /* [unique][size_is][in] */ __RPC__in_ecount_full_opt(cidl) PCUITEMID_CHILD_ARRAY apidl,
      /* [in] */ __RPC__in REFIID riid,
      /* [unique][out][in] */ 
      __reserved  UINT *rgfReserved,
      /* [iid_is][out] */ __RPC__deref_out_opt void **ppv ),
      ( hwndOwner, cidl, apidl, riid, rgfReserved, ppv ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder,
    GetDisplayNameOf, ( 
      /* [in] */ __RPC__in PCUITEMID_CHILD pidl,
      /* [in] */ SHGDNF uFlags,
      /* [out] */ __RPC__out STRRET *pName ),
      ( pidl, uFlags, pName ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder,
    SetNameOf, ( 
      /* [unique][in] */ __RPC__in_opt HWND hwnd,
      /* [in] */ __RPC__in PCUITEMID_CHILD pidl,
      /* [string][in] */ __RPC__in LPCWSTR pszName,
      /* [in] */ SHGDNF uFlags,
      /* [out] */ __RPC__deref_out_opt PITEMID_CHILD *ppidlOut ),
      ( hwnd, pidl, pszName, uFlags, ppidlOut ) );
};

class
IShellFolder2_wrap :
  public IUnknown_wrap,
  public IShellFolder_wrap,
  public IShellFolder2
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
    IShellFolder,
    ParseDisplayName, (
      /* [unique][in] */ __RPC__in_opt HWND hwnd,
      /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
      /* [string][in] */ __RPC__in LPWSTR pszDisplayName,
      /* [unique][out][in] */ 
      __reserved  ULONG *pchEaten,
      /* [out] */ __RPC__deref_out_opt PIDLIST_RELATIVE *ppidl,
      /* [unique][out][in] */ __RPC__inout_opt ULONG *pdwAttributes ),
      ( hwnd, pbc, pszDisplayName, pchEaten, ppidl, pdwAttributes ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder,
    EnumObjects, ( 
      /* [unique][in] */ __RPC__in_opt HWND hwnd,
      /* [in] */ SHCONTF grfFlags,
      /* [out] */ __RPC__deref_out_opt IEnumIDList **ppenumIDList ),
      ( hwnd, grfFlags, ppenumIDList ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder,
    BindToObject, ( 
      /* [in] */ __RPC__in PCUIDLIST_RELATIVE pidl,
      /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
      /* [in] */ __RPC__in REFIID riid,
      /* [iid_is][out] */ __RPC__deref_out_opt void **ppv ),
      ( pidl, pbc, riid, ppv ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder,
    BindToStorage, ( 
      /* [in] */ __RPC__in PCUIDLIST_RELATIVE pidl,
      /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
      /* [in] */ __RPC__in REFIID riid,
      /* [iid_is][out] */ __RPC__deref_out_opt void **ppv ),
      ( pidl, pbc, riid, ppv ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder,
    CompareIDs, ( 
      /* [in] */ LPARAM lParam,
      /* [in] */ __RPC__in PCUIDLIST_RELATIVE pidl1,
      /* [in] */ __RPC__in PCUIDLIST_RELATIVE pidl2 ),
      ( lParam, pidl1, pidl2 ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder,
    CreateViewObject, ( 
      /* [unique][in] */ __RPC__in_opt HWND hwndOwner,
      /* [in] */ __RPC__in REFIID riid,
      /* [iid_is][out] */ __RPC__deref_out_opt void **ppv ),
      ( hwndOwner, riid, ppv ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder,
    GetAttributesOf, ( 
      /* [in] */ UINT cidl,
      /* [unique][size_is][in] */ __RPC__in_ecount_full_opt(cidl) PCUITEMID_CHILD_ARRAY apidl,
      /* [out][in] */ __RPC__inout SFGAOF *rgfInOut ),
      ( cidl, apidl, rgfInOut ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder,
    GetUIObjectOf, ( 
      /* [unique][in] */ __RPC__in_opt HWND hwndOwner,
      /* [in] */ UINT cidl,
      /* [unique][size_is][in] */ __RPC__in_ecount_full_opt(cidl) PCUITEMID_CHILD_ARRAY apidl,
      /* [in] */ __RPC__in REFIID riid,
      /* [unique][out][in] */ 
      __reserved  UINT *rgfReserved,
      /* [iid_is][out] */ __RPC__deref_out_opt void **ppv ),
      ( hwndOwner, cidl, apidl, riid, rgfReserved, ppv ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder,
    GetDisplayNameOf, ( 
      /* [in] */ __RPC__in PCUITEMID_CHILD pidl,
      /* [in] */ SHGDNF uFlags,
      /* [out] */ __RPC__out STRRET *pName ),
      ( pidl, uFlags, pName ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder,
    SetNameOf, ( 
      /* [unique][in] */ __RPC__in_opt HWND hwnd,
      /* [in] */ __RPC__in PCUITEMID_CHILD pidl,
      /* [string][in] */ __RPC__in LPCWSTR pszName,
      /* [in] */ SHGDNF uFlags,
      /* [out] */ __RPC__deref_out_opt PITEMID_CHILD *ppidlOut ),
      ( hwnd, pidl, pszName, uFlags, ppidlOut ) );

  IFACE_METHOD( HRESULT,
    IShellFolder2,
    GetDefaultSearchGUID, ( 
      /* [out] */ __RPC__out GUID *pguid ),
      ( pguid ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder2,
    EnumSearches, ( 
      /* [out] */ __RPC__deref_out_opt IEnumExtraSearch **ppenum ),
      ( ppenum ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder2,
    GetDefaultColumn, ( 
      /* [in] */ DWORD dwRes,
      /* [out] */ __RPC__out ULONG *pSort,
      /* [out] */ __RPC__out ULONG *pDisplay ),
      ( dwRes, pSort, pDisplay ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder2,
    GetDefaultColumnState, ( 
      /* [in] */ UINT iColumn,
      /* [out] */ __RPC__out SHCOLSTATEF *pcsFlags ),
      ( iColumn, pcsFlags ) );
  
  IFACE_METHOD( HRESULT,
    IShellFolder2,
    GetDetailsEx, ( 
      /* [in] */ __RPC__in PCUITEMID_CHILD pidl,
      /* [in] */ __RPC__in const SHCOLUMNID *pscid,
      /* [out] */ __RPC__out VARIANT *pv ),
      ( pidl, pscid, pv ) );
  
  IFACE_METHOD( HRESULT,IShellFolder2,
    GetDetailsOf, ( 
      /* [unique][in] */ __RPC__in_opt PCUITEMID_CHILD pidl,
      /* [in] */ UINT iColumn,
      /* [out] */ __RPC__out SHELLDETAILS *psd ),
      ( pidl, iColumn, psd ) );
  
  IFACE_METHOD( HRESULT,IShellFolder2,
    MapColumnToSCID, ( 
      /* [in] */ UINT iColumn,
      /* [out] */ __RPC__out SHCOLUMNID *pscid ),
      ( iColumn, pscid ) );
};
