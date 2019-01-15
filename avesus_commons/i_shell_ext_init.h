# pragma once

# include "avecom.h"

# include <shobjidl.h>

class
IShellExtInit_wrap :
  public IUnknown_wrap,
  public IShellExtInit
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
    IShellExtInit,
    Initialize, ( 
            __in_opt  PCIDLIST_ABSOLUTE pidlFolder,
            __in_opt  IDataObject *pdtobj,
            __in_opt  HKEY hkeyProgID ),
    ( pidlFolder, pdtobj, hkeyProgID ) );
};
