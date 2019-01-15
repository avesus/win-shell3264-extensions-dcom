
// XP
# define _WIN32_WINNT 0x0501

// From XP (so the first 64-bit OS)
# define _WIN32_IE    0x0600
# define _WIN32_DCOM
# include <windows.h>
# include <cpl.h>
# include <shlobj.h>
# include <process.h>

# include <Objidl.h>

# include "./../../../avesus_commons/avedbg.h"
# include "./../../../avesus_commons/aveutils.h"

# include "./../codexsrv/com/codexsrv.h"

//ICodexControlPanel* g_poCtrlPanelCom = 0;

HINSTANCE g_hInst = 0;

BOOL LoadCodexCtrlPanelComServer( BOOL i_fnElevated,
                                 ICodexControlPanel** o_ppoICpl )
{
  ENSURE( o_ppoICpl );

  * o_ppoICpl = 0;

  IClassFactory* poIClassFactory = 0;
  HRESULT hRes = E_UNEXPECTED;

# ifdef WIN64

  DWORD nActivate32BitServerFlag = CLSCTX_ACTIVATE_32_BIT_SERVER;

# else

  DWORD nActivate32BitServerFlag
    = CVersionUtils::IsWow64() ? CLSCTX_ACTIVATE_32_BIT_SERVER : 0;

# endif

  if( i_fnElevated )
  {
    BIND_OPTS3 bindOptions;
    ::memset( & bindOptions, 0, sizeof( bindOptions ) );
    bindOptions.cbStruct = sizeof( bindOptions );
    bindOptions.hwnd = ::GetForegroundWindow();
    bindOptions.dwClassContext
      = CLSCTX_LOCAL_SERVER | nActivate32BitServerFlag;

    CRegistryGuid oClsId;
    ENSURE( oClsId.Build( __uuidof( CodexSrv ) ) );

    WCHAR awzElevationMonikerName[ 256 ] = { 0 };
    ENSURE( 0 == ::wcsncpy_s( awzElevationMonikerName,
      ALEN( awzElevationMonikerName ),
      L"Elevation:Administrator!clsid:", 255 ) );

    ENSURE( 0 == ::wcsncat_s( awzElevationMonikerName,
      ALEN( awzElevationMonikerName ), oClsId.Get(), 255 ) );

    hRes = ::CoGetObject(
      awzElevationMonikerName, & bindOptions,
      __uuidof( IClassFactory ),
      reinterpret_cast< void** >( & poIClassFactory ) );
    if( hRes != S_OK )
    {
      // user cancel elevation
      return TRUE;
    }
  }
  else
  {
    hRes = ::CoGetClassObject( __uuidof( CodexSrv ),
		  CLSCTX_LOCAL_SERVER | nActivate32BitServerFlag,
		  0,
		  IID_IClassFactory, (void**) & poIClassFactory );
	  if( hRes != S_OK )
    {
      ::DebugPrintA(
        "Failed to CoGetClassObject for Codex, error 0x%X", hRes );
      return TRUE;
    }
  }

  ICodexControlPanel* pICpl = 0;
	hRes = poIClassFactory->CreateInstance(
    0, __uuidof( ICodexControlPanel ),
		(void**) & pICpl );

  poIClassFactory->Release();
  poIClassFactory = 0;

  if( hRes != S_OK )
  {
    ::DebugPrintA(
      "Failed to Create Codex Instance, error 0x%X", hRes );
    return TRUE;
  }

  * o_ppoICpl = pICpl;

  return TRUE;
}

//CRITICAL_SECTION g_oInterfacePtrGuard;

UINT32 CALLBACK ParentWndAssigner( void* i_pParam )
{
  i_pParam;
  //CICodexControlPanelImpl* poThis = (CICodexControlPanelImpl*)i_pParam;
  //ENSURE( poThis );

  UINT32 nRetry = 5;
  HWND hwndApplet = 0;
  while( ! hwndApplet && nRetry )
  {
    hwndApplet = ::FindWindowA( 0, "MagicRAR Settings" );

    ::Sleep( 50 );

    --nRetry;
  }

  ::SetForegroundWindow( hwndApplet );
  //::SetWindowPos( hwndApplet, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE );

  return 0;
}

LONG __stdcall CPlApplet( HWND i_hwndCPl,
                UINT i_uMsg,
                LPARAM i_lParam1,
                __inout LPARAM io_lParam2 )
{
  //CAveAutoGuard oGuard( & g_oInterfacePtrGuard );

  LONG nRet = -1;

  BOOL fnDoRelease = FALSE;
  BOOL fnDoCall = TRUE;

  switch( i_uMsg )
  {
    break; case CPL_DBLCLK:
    {
      __in UINT32 i_nAppNum = (UINT32) i_lParam1;
      __in void* i_pData = (void*) io_lParam2;

      //::MessageBoxW( 0, L"CPL_DBLCLK message", L"CodexCpl", 0 );

      HMODULE hShell32 = ::LoadLibraryW( L"shell32.dll" );
      ENSURE( hShell32 );

      BOOL (__stdcall* xIsUserAnAdmin)( void ) =
        (BOOL (__stdcall*)( void ))
        ::GetProcAddress( hShell32, "IsUserAnAdmin" );

      ::FreeLibrary( hShell32 );

      BOOL fnDoElevation = FALSE;

      if( xIsUserAnAdmin )
      {
        fnDoElevation = ! xIsUserAnAdmin();
      }

      ICodexControlPanel* poICpl = 0;
      ENSURE( LoadCodexCtrlPanelComServer( fnDoElevation,
        & poICpl ) );

      if( poICpl )
      {
        /*HANDLE hFindThread =
          (HANDLE) ::_beginthreadex( 0, 0, ParentWndAssigner,
            (void*)i_hwndCPl, 0, 0 );*/

        HRESULT hRes = poICpl->Show( (__int64)i_hwndCPl );

        //::WaitForSingleObject( hFindThread, INFINITE );

        poICpl->Release();
        poICpl = 0;

        if( S_OK != hRes )
        {
          ::MessageBoxW( 0, L"Error in CodexSrv RPC communication", L"CodexCpl", 0 );
          return -1;
        }
      }
      else
      {
        ::MessageBoxW( 0,
L"Unfortunately, MagicRAR Control Panel\r\n"
L"cannot be opened because of configuration problems.\r\n"
L"\r\n"
L"Perhaps, it is need to reinstall MagicRAR,\r\n"
L"or its version is incompatible with your version of Windows.", L"CodexCpl", 0 );
      }

      return 0;

      // return: success - 0, fail - other
    }

    break; case CPL_EXIT:
    {
      fnDoRelease = TRUE;

      //::MessageBoxW( 0, L"CPL_EXIT message", L"CodexCpl", 0 );

      return 0;

      // return: success - 0, fail - other
    }

    break; case CPL_GETCOUNT:
    {
      //::MessageBoxW( 0, L"CPL_GETCOUNT message", L"CodexCpl", 0 );

      return 1;

      // return: count of items
    }

    break; case CPL_INIT:
    {
      /*
      if( ! g_poCtrlPanelCom )
      {
        if( ! VERIFY( LoadCodexCtrlPanelComServer( FALSE ) ) )
        {
          nRet = FALSE;
          fnDoCall = FALSE;
        }
      }
      */

      //::MessageBoxW( 0, L"CPL_INIT message", L"CodexCpl", 0 );

      return TRUE;

      // return: TRUE - ok, FALSE - unload me
    }

    break; case CPL_INQUIRE:
    {
      __in UINT32 i_nAppNum = (UINT32) i_lParam1;
      __out CPLINFO* o_poCplInfo = (CPLINFO*)((void*)io_lParam2);

      fnDoCall = FALSE;

      //::MessageBoxW( 0, L"CPL_INQUIRE message", L"CodexCpl", 0 );

      return 0;

      // return: success - 0, fail - other
    }

    break; case CPL_NEWINQUIRE:
    {
      __in UINT32 i_nAppNum = (UINT32) i_lParam1;
      __out NEWCPLINFO* o_poNewCplInfo = (NEWCPLINFO*)((void*)io_lParam2);

      //::MessageBoxW( 0, L"CPL_NEWINQUIRE message", L"CodexCpl", 0 );

      o_poNewCplInfo->dwSize = sizeof( * o_poNewCplInfo );
      ENSURE( 0 == ::wcscpy_s( o_poNewCplInfo->szName,
        ALEN( o_poNewCplInfo->szName ), L"MagicRAR Settings" ) );

      ENSURE( 0 == ::wcscpy_s( o_poNewCplInfo->szInfo,
        ALEN( o_poNewCplInfo->szInfo ),
        L"Configure MagicRAR data (de)compression." ) );

      o_poNewCplInfo->hIcon = ::LoadIconW( g_hInst, MAKEINTRESOURCEW( 1 ) );

      return 0;

      // return: success - 0, fail - other
    }

    break; case CPL_STARTWPARMS:
    {
      __in UINT32 i_nAppNum = (UINT32) i_lParam1;
      __in WCHAR* i_pwzExtraDirs = (WCHAR*)((void*)io_lParam2);

      /*
      if( ! ::IsUserAnAdmin() )
      {
        // reload object as admin:

        g_poCtrlPanelCom->Release();
        g_poCtrlPanelCom = 0;

        if( ! VERIFY( LoadCodexCtrlPanelComServer( TRUE ) ) )
        {
          nRet = FALSE;
          fnDoCall = FALSE;
        }
      }
      */

      //io_lParam2 = 0;

      ::MessageBoxW( 0, L"CPL_STARTWPARMS message", L"CodexCpl", 0 );

      return TRUE;

      // return: TRUE - handled, FALSE - not handled
    }

    break; case CPL_STOP:
    {
      __in UINT32 i_nAppNum = (UINT32) i_lParam1;
      __in void* i_pData = (void*) io_lParam2;

      //::MessageBoxW( 0, L"CPL_STOP message", L"CodexCpl", 0 );

      return 0;

      // return: success - 0, fail - other
    }

    break; default:
    {
      ::MessageBoxW( 0, L"Unexpected message", L"CodexCpl", 0 );
      return -1;
    }
  }
/*
  if( g_poCtrlPanelCom && fnDoCall )
  {
    HRESULT hRes = g_poCtrlPanelCom->CPlApplet( (__int64)i_hwndCPl,
      i_uMsg, (__int64)i_lParam1, (__int64)io_lParam2, & nRet );
    if( S_OK != hRes )
    {
      ::MessageBoxW( 0, L"Error in CodexSrv RPC communication", L"CodexCpl", 0 );
      nRet = -1;
    }

    if( CPL_INIT == i_uMsg )
    {
      if( nRet )
      {
        //::MessageBoxW( 0, L"CPL_INIT returned TRUE", L"CodexCpl", 0 );
      }
      else
      {
        ::MessageBoxW( 0, L"CPL_INIT returned FALSE", L"CodexCpl", 0 );
      }
    }
  }

  if( fnDoRelease )
  {
    g_poCtrlPanelCom->Release();
    g_poCtrlPanelCom = 0;
  }
*/

  return nRet;
}


extern "C"
BOOL WINAPI
DllMain( HINSTANCE i_hInst, DWORD i_nReason, void* )
{
  switch( i_nReason )
  {
    break; case DLL_PROCESS_ATTACH:
    {
      //::InitializeCriticalSection( & g_oInterfacePtrGuard );
      g_hInst = i_hInst;
    }

    break; case DLL_PROCESS_DETACH:
    {
      /*::EnterCriticalSection( & g_oInterfacePtrGuard );

      if( g_poCtrlPanelCom )
      {
        g_poCtrlPanelCom->Release();
        g_poCtrlPanelCom = 0;
      }

      ::LeaveCriticalSection( & g_oInterfacePtrGuard );*/
    }
  }

  return TRUE;
}

