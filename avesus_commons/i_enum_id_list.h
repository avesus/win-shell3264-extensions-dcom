# pragma once

# include "avecom.h"

# include <shobjidl.h>

class
IEnumIDList_wrap :
  public IUnknown_wrap,
  public IEnumIDList
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
    IEnumIDList,
    Next, ( 
      /* [in] */ 
      __in  ULONG celt,
      /* [length_is][size_is][out] */ 
      __out_ecount_part(celt, *pceltFetched)  PITEMID_CHILD *rgelt,
      /* [out] */ __out_opt  ULONG *pceltFetched ),
      ( celt, rgelt, pceltFetched ) );
  
  IFACE_METHOD( HRESULT,
    IEnumIDList,
    Skip, ( 
      /* [in] */ ULONG celt ),
      ( celt ) );
  
  IFACE_METHOD( HRESULT,
    IEnumIDList,
    Reset, ( void ), () );
  
  IFACE_METHOD( HRESULT,
    IEnumIDList,
    Clone, ( 
      /* [out] */ __RPC__deref_out_opt IEnumIDList **ppenum ),
      ( ppenum ) );
};
