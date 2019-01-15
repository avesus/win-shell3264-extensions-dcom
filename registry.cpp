# include "common_header.h"

// LoadTypeLib/RegisterTypeLib - for default IPC marshalling support
# include <oleauto.h>

# include "codex_srv.h"

# include "./../../../avesus_commons/avecomreg.h"

BOOL RegServer( HRESULT* o_phRes )
{
  ENSURE( o_phRes );

  * o_phRes = E_UNEXPECTED;

  WCHAR awzModulePath[ MAX_PATH ] = { 0 };

  ENSURE( ::GetModuleFileNameW(
    0, awzModulePath, ALEN( awzModulePath ) ) );

  WCHAR awzLocalizedStringResourcePath[ MAX_PATH + 1 ] = { 0 };

  ENSURE( 0 == ::wcscpy_s( awzLocalizedStringResourcePath,
    ALEN( awzLocalizedStringResourcePath ), L"@" ) );

  ENSURE( 0 == ::wcscat_s( awzLocalizedStringResourcePath,
    ALEN( awzLocalizedStringResourcePath ), awzModulePath ) );

  ENSURE( 0 == ::wcscat_s( awzLocalizedStringResourcePath,
    ALEN( awzLocalizedStringResourcePath ), L",-100" ) );

  BOOL fnOk = FALSE;
  CAveSrvComReg oReg;
  ENSURE( oReg.InitReg( __uuidof( CodexSrv ),
    L"Codex Archive Plugins COM 32-bit wrapper",
    awzModulePath, awzLocalizedStringResourcePath, & fnOk ) );

  //g_oAppIdGuid,
  //  L"Codex Archive Plugins 32-bit COM server exe",

  if( fnOk )
  {
    * o_phRes = S_OK;
  }

  return TRUE;
}

BOOL UnregServer( HRESULT* o_phRes )
{
  ENSURE( o_phRes );

  * o_phRes = E_UNEXPECTED;

  WCHAR awzModulePath[ MAX_PATH ] = { 0 };

  ENSURE( ::GetModuleFileNameW(
    0, awzModulePath, ALEN( awzModulePath ) ) );

  WCHAR awzLocalizedStringResourcePath[ MAX_PATH + 1 ] = { 0 };
  ENSURE( 0 == ::wcscpy_s( awzLocalizedStringResourcePath,
    ALEN( awzLocalizedStringResourcePath ), awzModulePath ) );

  ENSURE( 0 == ::wcscat_s( awzLocalizedStringResourcePath,
    ALEN( awzLocalizedStringResourcePath ), L",-100" ) );

  BOOL fnOk = FALSE;
  CAveSrvComReg oReg;
  ENSURE( oReg.InitReg( __uuidof( CodexSrv ),
    L"Codex Archive Plugins COM 32-bit wrapper",
    awzModulePath, awzLocalizedStringResourcePath, & fnOk ) );
  // g_oAppIdGuid,
  //  L"Codex Archive Plugins 32-bit COM server exe",

  if( ! fnOk )
  {
    * o_phRes = S_FALSE;
    return TRUE;
  }

  ENSURE( oReg.Unreg( & fnOk ) );

  * o_phRes = fnOk ? S_OK : S_FALSE;

  return TRUE;
}

enum ECmdLineAction
{
  CMD_ACTION_NONE = 2,
  CMD_ACTION_REGSERVER,
  CMD_ACTION_UNREGSERVER
};

BOOL ParseCmdLine(
  __in  const CHAR*           i_pczCmdLine,
  __out       ECmdLineAction* o_peAction )
{
  ENSURE( o_peAction );

  * o_peAction = CMD_ACTION_NONE;

  if( ! i_pczCmdLine )
  {
    return TRUE;
  }

  if( ! i_pczCmdLine[ 0 ] )
  {
    return TRUE;
  }

  if( ! ( '-' == i_pczCmdLine[ 0 ]
          ||
          '/' == i_pczCmdLine[ 0 ] ) )
  {
    return TRUE;
  }

  const CHAR* pczRegCmd = & i_pczCmdLine[ 1 ];

  if( ! pczRegCmd[ 0 ] )
  {
    return TRUE;
  }

  if( 0 == ::_strnicmp( pczRegCmd, "RegServer", 9 ) )
  {
    * o_peAction = CMD_ACTION_REGSERVER;
  }
  else if( 0 == ::_strnicmp( pczRegCmd, "UnregServer", 11 ) )
  {
    * o_peAction = CMD_ACTION_UNREGSERVER;
  }

  return TRUE;
}

BOOL RegUnregOrRun( CHAR* i_pczCmdLine, BOOL* o_pfnRun )
{
  ENSURE( o_pfnRun );

  * o_pfnRun = FALSE;

  ECmdLineAction eAction = CMD_ACTION_NONE;
  if( ! VERIFY( ParseCmdLine( i_pczCmdLine, & eAction ) ) )
  {
    return E_UNEXPECTED;
  }

  switch( eAction )
  {
    break; case CMD_ACTION_REGSERVER:
    {
      HRESULT hRes = E_FAIL;
      if( ! VERIFY( RegServer( & hRes ) ) )
      {
        hRes = E_UNEXPECTED;
      }

      if( S_OK == hRes )
      {
        ::MessageBoxW( 0, L"CodexSrv registered.", L"CodexSrv", 0 );
      }
      else
      {
        ::MessageBoxW( 0, L"Registrastion failed.", L"CodexSrv", 0 );
      }

      return TRUE;
    }

    break; case CMD_ACTION_UNREGSERVER:
    {
      HRESULT hRes = E_FAIL;
      if( ! VERIFY( UnregServer( & hRes ) ) )
      {
        hRes = E_UNEXPECTED;
      }

      if( S_OK == hRes )
      {
        ::MessageBoxW( 0, L"CodexSrv unregistered.", L"CodexSrv", 0 );
      }
      else
      {
        ::MessageBoxW( 0, L"Unregistrastion failed.", L"CodexSrv", 0 );
      }

      return TRUE;
    }
  }

  * o_pfnRun = TRUE;

  return TRUE;
}