# pragma once

# include "common_header.h"

# include "./../codexsrv/com/codexsrv.h"
# include "./../codex_man/codex_man.h"

//# include "./../../../avesus_commons/avecom.h"

//# include <process.h>

# include <psapi.h>
# include <shlwapi.h>

# pragma comment( lib, "psapi" )

//# define CODEX_SRV_AUTO_UNLOAD_TIMEOUT (30000)
//# define TIMEOUT_30_DAYS (2592000000)

// Object for accessing codex COM server.
// Should been used only within a single thread.
// Client of this object can open any count of
// archives.
class CCodexAccess
{
public:

  CCodexAccess() :
    m_fnOpened( FALSE ),
    m_nThreadId( ::GetCurrentThreadId() ),
    m_poApi( 0 )
  {
  }

  BOOL Open(
    __in  BSTR i_bstrArchiveFilePath,
    __in  BOOL i_fnAdminAccess,
    __out ICodexArchive** o_ppoCodexApi,
    __out BSTR* o_pbstrError )
  {
    ENSURE( ! m_fnOpened && ! m_poApi );
    ENSURE( i_bstrArchiveFilePath && o_ppoCodexApi && o_pbstrError );

    * o_ppoCodexApi = 0;

    ENSURE( ::GetCurrentThreadId() == m_nThreadId );

    ENSURE( LoadDll() );

    if( m_oDll.AccessArchive( i_bstrArchiveFilePath,
      i_fnAdminAccess, & m_poApi, o_pbstrError ) )
    {
      if( m_poApi )
      {
        ICodexArchive* poCurrThreadApi = 0;
        if( ! VERIFY( m_poApi->GetICodex( & poCurrThreadApi ) ) )
        {
          delete m_poApi;
          m_poApi = 0;
        }

        * o_ppoCodexApi = poCurrThreadApi;

        m_fnOpened = TRUE;

        return TRUE;
      }
    }

    ENSURE( m_oDll.Free() );

    // error opening is not a failure!
    return TRUE;
  }

  
  ~CCodexAccess()
  {
    //VERIFY( ::GetCurrentThreadId() == m_nThreadId );

    if( m_fnOpened )
    {
      if( VERIFY( m_poApi ) )
      {
        delete m_poApi;
        m_poApi = 0;
      }

      m_fnOpened = FALSE;
    }

    if( m_oDll.IsLoaded() )
    {
      m_oDll.Free();
    }
  }

private:

  BOOL LoadDll( void )
  {
    WCHAR awzPath[ 1024 ] = { 0 };
    HANDLE hCurrProcess = ::GetCurrentProcess();
    // get mComUltra dll path
    ENSURE( ::GetModuleFileNameExW(
      hCurrProcess, g_hInst, awzPath, ALEN( awzPath ) ) );
    ::CloseHandle( hCurrProcess );

    WCHAR* pwzFileName = ::PathFindFileNameW( awzPath );
    ENSURE( pwzFileName );
    ENSURE( 0 == ::wcscpy_s( pwzFileName,
      ALEN( awzPath ) - (pwzFileName - awzPath), L"codex_man.dll" ) );

    ENSURE( m_oDll.Load( awzPath ) );

    return TRUE;
  }
  
  class CCodexManDll :
    public CDllWrapper
  {
  public:

    CDllFuncPtr< AccessArchiveExportedFuncType* > AccessArchive;

  private:

    virtual BOOL FillDllFuncPtrs( HMODULE i_hDll )
    {
      ENSURE( i_hDll );

      FILL_DLL_FUNC_PTR( AccessArchive );

      return TRUE;
    }  
  };

  
  CCodexManDll m_oDll;

  BOOL m_fnOpened;

  UINT32 m_nThreadId;

  ICodexArchiveApi* m_poApi;
};

/*

class CCodexSrvCache
{
  class CCodexManDll :
    public CDllWrapper
  {
  public:

    CDllFuncPtr<
      BOOL (__stdcall*)(
        __in  BSTR i_bstrArchiveFilePath,
        __in  BOOL i_fnTemporary,
        __in  BOOL i_fnAdminAccess,
        __out ICodexArchive** o_ppoCodexApi,
        __out BSTR* o_pbstrError )
      > OpenCodex;

    CDllFuncPtr<
      BOOL (__stdcall*)( ICodexArchive* i_poArchiveApi,
        BOOL i_fnImmediate )
      > CloseCodex;

  private:

    virtual BOOL FillDllFuncPtrs( HMODULE i_hDll )
    {
      ENSURE( i_hDll );

      FILL_DLL_FUNC_PTR( OpenCodex );
      FILL_DLL_FUNC_PTR( CloseCodex );

      return TRUE;
    }  
  };

public:

  // o_pbstrError - pointer to BSTR with error information,
  // if archive file cannot be opened. It should be freed by
  // SysFreeString() if returned not null.
  BOOL AccessArchive(
    __in  BSTR i_bstrArchiveFilePath,
    __in  BOOL i_fnTemporary,
    __in  BOOL i_fnAdminAccess,
    __out ICodexArchive** o_ppoCodexApi,
    __out BSTR* o_pbstrError )
  {
    CAveAutoGuard oGuard( & m_oAccessGuard );

//    if( ! m_oDll.IsLoaded() )
//    {
      WCHAR awzPath[ 1024 ] = { 0 };
      HANDLE hCurrProcess = ::GetCurrentProcess();
      ENSURE( ::GetModuleFileNameExW( hCurrProcess, g_hInst, awzPath, ALEN( awzPath ) ) );
      ::CloseHandle( hCurrProcess );

      WCHAR* pwzFileName = ::PathFindFileNameW( awzPath );
      ENSURE( 0 == ::wcscpy_s( pwzFileName,
        ALEN( awzPath ) - (pwzFileName - awzPath), L"codex_man.dll" ) );

      ENSURE( m_oDll.Load( awzPath ) );
  //  }

    return m_oDll.OpenCodex( i_bstrArchiveFilePath,
      i_fnTemporary, i_fnAdminAccess, o_ppoCodexApi,
      o_pbstrError );
  }

  // If i_fnImmediate == TRUE, archive will be closed
  // immediately, without waiting the timeout,
  // if it has no any references to it.
  // This flag is used for handling delete notifications.
  BOOL ReleaseArchive( ICodexArchive* i_poArchiveApi,
    BOOL i_fnImmediate )
  {
    CAveAutoGuard oGuard( & m_oAccessGuard );

    BOOL fnRet = FALSE;

    if( m_oDll.IsLoaded() )
    {

      fnRet = m_oDll.CloseCodex( i_poArchiveApi, i_fnImmediate );

      VERIFY( m_oDll.Free() );
    }

    return fnRet;
  }

  ~CCodexSrvCache()
  {
    ::DeleteCriticalSection( & m_oAccessGuard );
  }

  CCodexSrvCache()
  {
    ::InitializeCriticalSection( & m_oAccessGuard );
  }

private:

  CRITICAL_SECTION m_oAccessGuard;

  CCodexManDll m_oDll;
};

*/