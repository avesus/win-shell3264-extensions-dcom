# pragma once

# include "avecom.h"

# include <shlobj.h>

class
IExplorerCommandProvider_wrap :
  public IUnknown_wrap,
  public IExplorerCommandProvider
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
    IExplorerCommandProvider,
    GetCommands, ( 
      /* [in] */ __RPC__in_opt IUnknown *punkSite,
      /* [in] */ __RPC__in REFIID riid,
      /* [iid_is][out] */ __RPC__deref_out_opt void **ppv),
      ( punkSite, riid, ppv ) );

  IFACE_METHOD( HRESULT,
    IExplorerCommandProvider,
    GetCommand, ( 
      /* [in] */ __RPC__in REFGUID rguidCommandId,
      /* [in] */ __RPC__in REFIID riid,
      /* [iid_is][out] */ __RPC__deref_out_opt void **ppv),
      ( rguidCommandId, riid, ppv ) );
};

class
IEnumExplorerCommand_wrap :
  public IUnknown_wrap,
  public IEnumExplorerCommand
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
    IEnumExplorerCommand,
    Next, ( 
      ULONG celt,
      __out IExplorerCommand** pUICommand,
      ULONG* pceltFetched ),
      ( celt, pUICommand, pceltFetched ) );
  
  IFACE_METHOD( HRESULT,
    IEnumExplorerCommand,
    Skip, ( 
      /* [in] */ ULONG celt ),
      ( celt ) );
  
  IFACE_METHOD( HRESULT,
    IEnumExplorerCommand,
    Reset, ( void ), () );
  
  IFACE_METHOD( HRESULT,
    IEnumExplorerCommand,
    Clone, ( 
      IEnumExplorerCommand **ppenum ),
      ( ppenum ) );
};
