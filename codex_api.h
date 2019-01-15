# pragma once

# include "./../../../avesus_commons/avedbg.h"
# include "./../../../avesus_commons/aveutils.h"

class CCodexApiDll :
  public CDllWrapper
{
public:

  CDllFuncPtr< INT32 (__stdcall*)( void ) > LoadPlugIns;

  CDllFuncPtr< void (__stdcall*)( void ) > FreePlugIns;

  CDllFuncPtr<
    INT32 (__stdcall*)(
      __in  const CHAR* i_pczFileName,
      __out INT32* o_pnItems,
      __out INT32* o_pnPwdItems,
      __out INT32* o_pnDateTimes,
      __out INT32* o_pnSizes,
      __out INT32* o_pnCompSizes )
  > QueryArchiveFieldLength;

  CDllFuncPtr<
    void (__stdcall*)(
      __out CHAR* o_acItems,
      __out CHAR* o_acPwdItems,
      __out CHAR* o_acDateTimes,
      __out CHAR* o_acSizes,
      __out CHAR* o_acCompSizes,
      __out CHAR* o_acError )
  > QueryArchiveEx;

private:

  virtual BOOL FillDllFuncPtrs( HMODULE i_hDll )
  {
    ENSURE( i_hDll );

    FILL_DLL_FUNC_PTR( LoadPlugIns );
    FILL_DLL_FUNC_PTR( FreePlugIns );
    FILL_DLL_FUNC_PTR( QueryArchiveFieldLength );
    FILL_DLL_FUNC_PTR( QueryArchiveEx );

    return TRUE;
  }  
};
