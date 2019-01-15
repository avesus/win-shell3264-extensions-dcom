
# include "codex_srv.h"

# include <cpl.h>
# include <process.h>

class CCodexControlPanelDll :
  public CDllWrapper
{
public:

  CDllFuncPtr< LONG (__stdcall*)( HWND, UINT, LPARAM, LPARAM ) >
    CPlApplet;

private:

  virtual BOOL FillDllFuncPtrs( HMODULE i_hDll )
  {
    ENSURE( i_hDll );

    FILL_DLL_FUNC_PTR( CPlApplet );

    return TRUE;
  }
};

HRESULT
CodexSrv::ICodexControlPanel_Show( __in __int64 i_hwndCPl )
{
  CCodexControlPanelDll oCplDll;
  if( ! VERIFY( oCplDll.Load( L"mControlPanel.dll" ) ) )
  {
    return ERROR_FILE_NOT_FOUND;
  }

  class CWndTopMover
  {
  public:

    BOOL Start( void )
    {
      m_hFindThread =
        (HANDLE) ::_beginthreadex( 0, 0, WndTopMoverThread, 0, 0, 0 );
      ENSURE( m_hFindThread );

      return TRUE;
    }

    CWndTopMover() :
      m_hFindThread( 0 )
    {
    }

    ~CWndTopMover()
    {
      if( m_hFindThread )
      {
        if( WAIT_TIMEOUT == ::WaitForSingleObject( m_hFindThread, 5000 ) )
        {
          ::TerminateThread( m_hFindThread, (DWORD)E_UNEXPECTED );
        }
      }
    }

  private:

    static UINT32 CALLBACK WndTopMoverThread( void* )
    {
      ::Sleep( 10 );

      UINT32 nRetry = 50;
      HWND hwndApplet = 0;
      while( ! hwndApplet && nRetry )
      {
        hwndApplet = ::FindWindowA( 0, "MagicRAR Settings" );

        ::Sleep( 50 );

        --nRetry;
      }

      ::AllowSetForegroundWindow( ::GetCurrentProcessId() );
      ::SetWindowPos( hwndApplet, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );
      ::SetForegroundWindow( hwndApplet );
      ::SetWindowPos( hwndApplet, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );

      return 0;
    }

    HANDLE m_hFindThread;
  };

  CWndTopMover oWndTopMover;
  if( ! VERIFY( oWndTopMover.Start() ) )
  {
    return E_OUTOFMEMORY;
  }

  oCplDll.CPlApplet( (HWND)i_hwndCPl, CPL_DBLCLK, 0, 0 );

  return S_OK;
}

