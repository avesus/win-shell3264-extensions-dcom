# pragma once

# include "common_header.h"

# include "./../../../avesus_commons/avecom.h"

# include "./com/codexsrv.h"

class __declspec( novtable )
ICodexControlPanel_wrap :
  public IUnknown_wrap,
  public ICodexControlPanel
{
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
    ICodexControlPanel,
    Show, ( __in __int64 i_hwndCPl ), ( i_hwndCPl ) );
};
