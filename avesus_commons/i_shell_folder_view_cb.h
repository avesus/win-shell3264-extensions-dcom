# pragma once

# include "avecom.h"

# include <shlobj.h>

class
IShellFolderViewCB_wrap :
  public IUnknown_wrap,
  public IShellFolderViewCB
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
    IShellFolderViewCB,
    MessageSFVCB,
      ( UINT uMsg, WPARAM wParam, LPARAM lParam ),
      ( uMsg, wParam, lParam ) );
};
