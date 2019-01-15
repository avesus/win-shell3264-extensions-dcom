# pragma once

# include "avecom.h"

# include <shobjidl.h>

class
IExtractIconW_wrap :
  public IUnknown_wrap,
  public IExtractIconW
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
    IExtractIconW,
    GetIconLocation, (
                         UINT uFlags,
    __out_ecount(cchMax) LPWSTR pszIconFile,
                         UINT   cchMax,
                   __out int*   piIndex,
                   __out UINT*  pwFlags ),
    ( uFlags, pszIconFile, cchMax, piIndex, pwFlags ) );

  IFACE_METHOD( HRESULT,
    IExtractIconW,
    Extract, ( LPCWSTR pszFile,
      UINT    nIconIndex,
      __out_opt HICON   *phiconLarge,
      __out_opt HICON   *phiconSmall,
      UINT    nIconSize ),
      ( pszFile, nIconIndex, phiconLarge, phiconSmall, nIconSize ) );

};
