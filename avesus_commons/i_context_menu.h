# pragma once

# include "avecom.h"

# include <shobjidl.h>

class
IContextMenu_wrap :
  public IUnknown_wrap,
  public IContextMenu
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
    IContextMenu,
    QueryContextMenu, ( 
      __in  HMENU hmenu,
      __in  UINT indexMenu,
      __in  UINT idCmdFirst,
      __in  UINT idCmdLast,
      __in  UINT uFlags ),
    ( hmenu, indexMenu, idCmdFirst, idCmdLast, uFlags ) );


  IFACE_METHOD( HRESULT,
    IContextMenu,
    InvokeCommand, ( 
      __in  CMINVOKECOMMANDINFO *pici ),
    ( pici ) );

  IFACE_METHOD( HRESULT,
    IContextMenu,
    GetCommandString, ( 
    __in  UINT_PTR idCmd,
    __in  UINT uType,
    __reserved  UINT *pReserved,
    __out_awcount(!(uType & GCS_UNICODE), cchMax)  LPSTR pszName,
    __in  UINT cchMax ),
    ( idCmd, uType, pReserved, pszName, cchMax ) );
};
