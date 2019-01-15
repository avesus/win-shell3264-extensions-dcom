# pragma once

# include "avecom.h"

# include <shobjidl.h>

class
IPersistFolder_wrap :
  public IUnknown_wrap,
  public IPersist_wrap,
  public IPersistFolder
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
    IPersist,
    GetClassID, ( __RPC__out CLSID *pClassID ),
      ( pClassID ) );

  IFACE_METHOD( HRESULT,
    IPersistFolder,
    Initialize, ( __RPC__in PCIDLIST_ABSOLUTE pidl ),
      ( pidl ) );
};

class //__declspec( novtable )
IPersistFolder2_wrap :
  public IUnknown_wrap,
  public IPersist_wrap,
  public IPersistFolder_wrap,
  public IPersistFolder2
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
    IPersist,
    GetClassID, ( __RPC__out CLSID *pClassID ),
      ( pClassID ) );

  IFACE_METHOD( HRESULT,
    IPersistFolder,
    Initialize, ( __RPC__in PCIDLIST_ABSOLUTE pidl ),
      ( pidl ) );

  IFACE_METHOD( HRESULT,
    IPersistFolder2,
    GetCurFolder,
      ( __RPC__deref_out_opt PIDLIST_ABSOLUTE* o_ppoIdList ),
      ( o_ppoIdList ) );
};

class
IPersistFolder3_wrap :
  public IUnknown_wrap,
  public IPersist_wrap,
  public IPersistFolder_wrap,
  public IPersistFolder2_wrap,
  public IPersistFolder3
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
    IPersist,
    GetClassID, ( __RPC__out CLSID *pClassID ),
      ( pClassID ) );

  IFACE_METHOD( HRESULT,
    IPersistFolder,
    Initialize, ( __RPC__in PCIDLIST_ABSOLUTE pidl ),
      ( pidl ) );

  IFACE_METHOD( HRESULT,
    IPersistFolder2,
    GetCurFolder,
      ( __RPC__deref_out_opt PIDLIST_ABSOLUTE* o_ppoIdList ),
      ( o_ppoIdList ) );

  IFACE_METHOD( HRESULT,
    IPersistFolder3,
    InitializeEx, ( 
      /* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
      /* [in] */ __RPC__in PCIDLIST_ABSOLUTE pidlRoot,
      /* [unique][in] */ __RPC__in_opt const
      PERSIST_FOLDER_TARGET_INFO *ppfti ),
      ( pbc, pidlRoot, ppfti ) );

  IFACE_METHOD( HRESULT,
    IPersistFolder3,
    GetFolderTargetInfo,
      ( __RPC__out PERSIST_FOLDER_TARGET_INFO *ppfti ),
      ( ppfti ) );

};

class
IPersistIDList_wrap :
  public IUnknown_wrap,
  public IPersist_wrap,
  public IPersistIDList
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
    IPersist,
    GetClassID, ( __RPC__out CLSID *pClassID ),
      ( pClassID ) );

  IFACE_METHOD( HRESULT,
    IPersistIDList,
    SetIDList, ( __RPC__in PCIDLIST_ABSOLUTE pidl ),
      ( pidl ) );

  IFACE_METHOD( HRESULT,
    IPersistIDList,
    GetIDList,
      ( __RPC__deref_out_opt PIDLIST_ABSOLUTE* o_ppoIdList ),
      ( o_ppoIdList ) );
};
