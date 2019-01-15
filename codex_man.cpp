// XP
# define _WIN32_WINNT 0x0501

// From XP (so the first 64-bit OS)
# define _WIN32_IE    0x0600
# define _WIN32_DCOM
# include <windows.h>
# include <psapi.h>
# include <shlobj.h>

# pragma comment( lib, "psapi" )

# include "./../codexsrv/com/codexsrv.h"

# include "./../../../avesus_commons/avedbg.h"
# include "./../../../avesus_commons/aveutils.h"
# include "./../../../avesus_commons/avecom.h"

# include <process.h>

# define CODEX_SRV_AUTO_UNLOAD_TIMEOUT (5000)
# define MANAGER_DLL_AUTO_UNLOAD_TIMEOUT (5000)
# define TIMEOUT_30_DAYS (2592000000)

# include "codex_man.h"

/*
  2 внутренних списка:
    1 - объекты с гарантированным админским доступом
    2 - объекты с обычным доступом.

  По запросу выдаётся объект прежде всего с админским доступом,
  даже если просят обычный.

  Если что-то при создании и открытии даёт сбой,
  то никакие объекты не должны создаваться и ничего
  не должно добавляться в списки.

  При выходе таймаута, объект должен закрывать работу с интерфейсом
  и освобождать его.

  При выходе таймаута, объект прежде всего вынимается из списка
  в рамках крит. секции, а потом уже свободно освобождается.



*/

HINSTANCE g_hInst = 0;
CRITICAL_SECTION g_oAccessGuard = { 0 };
volatile HANDLE g_hManThread = 0;

HANDLE g_hEventInitialized = 0;

IUnknown* g_poInstanceExplorer = 0;

volatile HRESULT g_hRes = E_UNEXPECTED;

BOOL g_fThreadCanExits = FALSE;

HANDLE g_hUnloadTimer = 0;

struct SCachedContainer
{
  BSTR m_bstrFilePath;

  ICodexArchive* m_poICodexInternal;
  UINT32 m_nRefCount;
  BOOL m_fnAdminAccess;

  HANDLE m_hAutoreleaseTimer;

  // because derived in list!!!
  virtual ~SCachedContainer() { }
};

CAveLinkedList< SCachedContainer > g_loNormalCache;
CAveLinkedList< SCachedContainer > g_loAdminCache;

static BOOL ManagerThread( void )
{
  // --------------- BEGIN OF AUTO-SYNC BLOCK ------------------------

  BOOL fRetOk = FALSE;
  BOOL fInitOk = FALSE;

  g_fThreadCanExits = FALSE;
  g_hRes = E_UNEXPECTED;
  g_poInstanceExplorer = 0;

  if( VERIFY( g_hEventInitialized && g_hInst ) )
  {
    g_hRes = ::CoInitializeEx( 0, COINIT_APARTMENTTHREADED );
    if( VERIFY( S_OK == g_hRes ) )
    {
      ::SHGetInstanceExplorer( & g_poInstanceExplorer );

      HANDLE hCurrProcess = ::GetCurrentProcess();
      if( VERIFY( hCurrProcess ) )
      {

        WCHAR awzPath[ 1024 ] = { 0 };
        if( VERIFY( ::GetModuleFileNameExW( hCurrProcess,
          g_hInst, awzPath, ALEN( awzPath ) ) ) )
        {
          // increment load counter
          if( VERIFY( ::LoadLibraryW( awzPath ) ) )
          {
            fInitOk = TRUE;
          }
        }

        ::CloseHandle( hCurrProcess );
      }

      if( fInitOk )
      {
        g_hRes = S_OK;
      }

      // --------------- END OF AUTO-SYNC BLOCK ----------------------
      fInitOk = fInitOk && VERIFY( ::SetEvent( g_hEventInitialized ) );

      if( fInitOk )
      {
        // wait for commands
        for(;;)
        {
          UINT32 nObj = ::SleepEx( INFINITE, TRUE );
          if( WAIT_IO_COMPLETION == nObj )
          {
            if( g_fThreadCanExits )
            {
              fRetOk = ( S_OK == g_hRes );
              break;
            }
          }
          else if( nObj )
          {
            // not a timeout

            fRetOk = FALSE;
            break;
          }
        }
      }

      {
        CAveAutoGuard oGuard( & g_oAccessGuard );

        fRetOk = fRetOk && VERIFY( ::CloseHandle( g_hManThread ) );
        g_hManThread = 0;

        if( g_poInstanceExplorer )
        {
          g_poInstanceExplorer->Release();
          g_poInstanceExplorer = 0;
        }

        ::CoUninitialize();

        g_hRes = fRetOk ? S_OK : E_UNEXPECTED;

        if( fInitOk )
        {
          // This step not necessary unloads dll
          ::LeaveCriticalSection( & g_oAccessGuard );
          ::FreeLibraryAndExitThread( g_hInst, g_hRes );
        }
      }
    }
  }
  
  return fRetOk;
}

static UINT32 CALLBACK ManagerThread( void* )
{
  VERIFY( ManagerThread() );

  return g_hRes;
}

static BOOL CreateCodexInterface(
  BOOL i_fnAdmin,
  ICodexArchive** o_ppoICodex )
{
  ENSURE( o_ppoICodex );

  * o_ppoICodex = 0;

  IClassFactory* poIClassFactory = 0;
  g_hRes = E_UNEXPECTED;

# ifdef WIN64

  DWORD nActivate32BitServerFlag = CLSCTX_ACTIVATE_32_BIT_SERVER;

# else

  DWORD nActivate32BitServerFlag
    = CVersionUtils::IsWow64() ? CLSCTX_ACTIVATE_32_BIT_SERVER : 0;

# endif

  if( i_fnAdmin )
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

    g_hRes = ::CoGetObject(
      awzElevationMonikerName, & bindOptions,
      __uuidof( IClassFactory ),
      reinterpret_cast< void** >( & poIClassFactory ) );
    if( g_hRes != S_OK )
    {
      // user cancels elevation
      return TRUE;
    }
  }
  else
  {
    g_hRes = ::CoGetClassObject( __uuidof( CodexSrv ),
	    CLSCTX_LOCAL_SERVER | nActivate32BitServerFlag,
	    0,
	    IID_IClassFactory, (void**) & poIClassFactory );
    if( g_hRes != S_OK )
    {
      // object unavailable
      ::DebugPrintA(
        "Failed to CoGetClassObject for Codex, error 0x%X", g_hRes );
      return TRUE;
    }
  }

  ICodexArchive* poICodex = 0;
  g_hRes = poIClassFactory->CreateInstance(
    0, __uuidof( ICodexArchive ),
	  (void**) & poICodex );

  poIClassFactory->Release();
  poIClassFactory = 0;

  if( g_hRes != S_OK )
  {
    // not fatal
    ::DebugPrintA(
      "Failed to Create Codex Instance, error 0x%X", g_hRes );
    return TRUE;
  }

  * o_ppoICodex = poICodex;

  return TRUE;
}

static BOOL DisableDllUnloadTimer( void )
{
  ENSURE( ::CancelWaitableTimer( g_hUnloadTimer ) );
  return TRUE;
}

static void CALLBACK AutounloadTimerApc( void*, DWORD, DWORD )
{
  g_hRes = E_UNEXPECTED;
  g_fThreadCanExits = TRUE;

  // If all lists are empty, then unload the library.

  VERIFY( DisableDllUnloadTimer() );

  if( g_loAdminCache.IsEmpty() && g_loNormalCache.IsEmpty() )
  {
    g_hRes = S_OK;
    g_fThreadCanExits = TRUE;
  }
  else
  {
    g_hRes = S_OK;
    g_fThreadCanExits = FALSE;
  }
}

static BOOL EnableDllUnloadTimerIfEmpty( void )
{
  if( g_loAdminCache.IsEmpty() && g_loNormalCache.IsEmpty() )
  {
    return VERIFY( CTimeUtils::SetWaitableTimerMsTimeout(
      g_hUnloadTimer,
      MANAGER_DLL_AUTO_UNLOAD_TIMEOUT, AutounloadTimerApc ) );
  }

  return TRUE;
}

static BOOL InitManagerThread( void )
{
  g_hEventInitialized = ::CreateEventW( 0, TRUE, FALSE, 0 );
  g_hUnloadTimer = ::CreateWaitableTimer( 0, TRUE, 0 );
  if( VERIFY( g_hEventInitialized && g_hUnloadTimer ) )
  {
    HANDLE hThread
      = (HANDLE) ::_beginthreadex( 0, 0, ManagerThread, 0, 0, 0 );
    if( VERIFY( hThread ) )
    {
      if( VERIFY( WAIT_OBJECT_0
              == ::WaitForSingleObject( g_hEventInitialized, INFINITE ) ) )
      {
        VERIFY( ::CloseHandle( g_hEventInitialized ) );
        g_hEventInitialized = 0;

        g_hManThread = hThread;
        return TRUE;
      }
      else
      {
        VERIFY( ::TerminateThread( hThread, (DWORD)E_UNEXPECTED ) );
        VERIFY( WAIT_OBJECT_0
              == ::WaitForSingleObject( hThread, INFINITE ) );
        VERIFY( ::CloseHandle( hThread ) );
      }
    }

    VERIFY( ::CloseHandle( g_hEventInitialized ) );
    g_hEventInitialized = 0;
  }

  return FALSE;
}

static void CALLBACK AutoreleaseTimerApc(
  void* i_pParam, DWORD, DWORD )
{
  SCachedContainer* poContainer
    = static_cast< SCachedContainer* >( i_pParam );
  if( ! VERIFY( poContainer ) )
  {
    g_hRes = E_UNEXPECTED;
    g_fThreadCanExits = TRUE;
    return;
  }

  VERIFY( ::CancelWaitableTimer( poContainer->m_hAutoreleaseTimer ) );

  // Test ref counter. It should be 1. If not 1,
  // then just continue.
  if( 1 == poContainer->m_nRefCount )
  {
    // Remove object and check list emptiness.
    if( poContainer->m_fnAdminAccess )
    {
      if( ! VERIFY( g_loAdminCache.Remove( poContainer ) ) )
      {
        g_hRes = E_UNEXPECTED;
        g_fThreadCanExits = TRUE;
        return;
      }
    }
    else
    {
      if( ! VERIFY( g_loNormalCache.Remove( poContainer ) ) )
      {
        g_hRes = E_UNEXPECTED;
        g_fThreadCanExits = TRUE;
        return;
      }
    }

    poContainer->m_poICodexInternal->CloseArchive();
    while( poContainer->m_poICodexInternal->Release() );
    poContainer->m_poICodexInternal = 0;

    if( poContainer->m_bstrFilePath )
    {
      ::SysFreeString( poContainer->m_bstrFilePath );
      poContainer->m_bstrFilePath = 0;
    }

    poContainer->m_nRefCount = 0;

    VERIFY( ::CloseHandle( poContainer->m_hAutoreleaseTimer ) );
    poContainer->m_hAutoreleaseTimer = 0;

    delete poContainer;
  }

  // If all lists are empty, then start auto-unload library timer.
  VERIFY( EnableDllUnloadTimerIfEmpty() );

  g_hRes = S_OK;
  g_fThreadCanExits = FALSE;
}

struct SReleaseArchiveCall :
  public SApcCall
{
  SCachedContainer* i_poCached;
};

static void CALLBACK ReleaseArchiveApc( ULONG_PTR i_pParam )
{
  g_hRes = E_UNEXPECTED;
  g_fThreadCanExits = TRUE;

  SReleaseArchiveCall* poCall
    = static_cast< SReleaseArchiveCall* >(
        static_cast< SApcCall* >( (void*) i_pParam ) );
  if( ! VERIFY( poCall ) || ! VERIFY( poCall->m_hCompleted ) )
  {
    return;
  }

  poCall->m_fOk = FALSE;

  SCachedContainer* poContainer = poCall->i_poCached;
  if( ! VERIFY( poContainer ) )
  {
    VERIFY( ::SetEvent( poCall->m_hCompleted ) );
    return;
  }

  // Decrement ref counter:
  -- poContainer->m_nRefCount;

  // If ref counter reaches 1, then start autorelease timer on it:
  if( 1 == poContainer->m_nRefCount )
  {
    if( ! VERIFY(
      CTimeUtils::SetWaitableTimerMsTimeout(
      poContainer->m_hAutoreleaseTimer,
      CODEX_SRV_AUTO_UNLOAD_TIMEOUT, AutoreleaseTimerApc, poContainer ) ) )
    {
      VERIFY( ::SetEvent( poCall->m_hCompleted ) );
      return;
    }
  }

  g_hRes = S_OK;
  g_fThreadCanExits = FALSE;
  poCall->m_fOk = TRUE;

  VERIFY( ::SetEvent( poCall->m_hCompleted ) );
}

struct SMarshalArchiveCall :
  public SApcCall
{
  SCachedContainer* i_poCached;
  IStream* o_poMarshalledCodexInterface;
};

static void CALLBACK MarshalArchiveApc( ULONG_PTR i_pParam )
{
  g_hRes = E_UNEXPECTED;
  g_fThreadCanExits = TRUE;

  SMarshalArchiveCall* poCall
    = static_cast< SMarshalArchiveCall* >(
        static_cast< SApcCall* >( (void*) i_pParam ) );
  if( ! VERIFY( poCall ) || ! VERIFY( poCall->m_hCompleted ) )
  {
    return;
  }

  poCall->m_fOk = FALSE;
  poCall->o_poMarshalledCodexInterface = 0;

  SCachedContainer* poContainer = poCall->i_poCached;
  if( ! VERIFY( poContainer ) )
  {
    VERIFY( ::SetEvent( poCall->m_hCompleted ) );
    return;
  }

  // Marshal interface:

  IStream* poICodexStream = 0;
  g_hRes = ::CoMarshalInterThreadInterfaceInStream(
               __uuidof( ICodexArchive ),
               poContainer->m_poICodexInternal, & poICodexStream );
  if( g_hRes != S_OK
        ||
      ! poICodexStream )
  {
    // not a fatal error:
    g_fThreadCanExits = FALSE;
    VERIFY( ::SetEvent( poCall->m_hCompleted ) );
    return;
  }

  poCall->o_poMarshalledCodexInterface = poICodexStream;
  poCall->m_fOk = TRUE;
  g_fThreadCanExits = FALSE;
  VERIFY( ::SetEvent( poCall->m_hCompleted ) );
}

// api object returned to user. Should support multithreading.
// Class is a kind of a smartpointer.
class CCodexArchiveApi :
  public ICodexArchiveApi
{
public:

  CCodexArchiveApi( void ) :
    m_poCached( 0 )
  {
  }

  // only can be called from ICodexArchiveApi::~ICodexArchiveApi().
  virtual ~CCodexArchiveApi()
  {
    // release access to cache object

    if( m_poCached )
    {
      CAveAutoGuard oGuard( & g_oAccessGuard );
      if( VERIFY( g_hManThread ) )
      {

        // APC call to Release
        SReleaseArchiveCall oCall;
        oCall.i_poCached = m_poCached;
        VERIFY( oCall.Call( g_hManThread, ReleaseArchiveApc ) );
      }

      m_poCached = 0;
    }
  }

  // get access to the codex interface. Transforms IStream
  // to the current thread interface.
  // User should release given interface pointer.
  virtual BOOL GetICodex( ICodexArchive** o_ppoICodex )
  {
    ENSURE( m_poCached );

    SMarshalArchiveCall oCall;
    oCall.i_poCached = m_poCached;
    {
      CAveAutoGuard oGuard( & g_oAccessGuard );
      ENSURE( oCall.Call( g_hManThread, MarshalArchiveApc ) );
    }

    ENSURE( oCall.o_poMarshalledCodexInterface );

    ICodexArchive* poICodex = 0;
    HRESULT hRes = ::CoUnmarshalInterface(
      oCall.o_poMarshalledCodexInterface, __uuidof( ICodexArchive ),
      (void**) & poICodex );

    oCall.o_poMarshalledCodexInterface->Release();

    ENSURE( S_OK == hRes );
    ENSURE( poICodex );

    * o_ppoICodex = poICodex;

    return TRUE;
  }


  SCachedContainer* m_poCached;
};


struct SAccessArchiveCall :
  public SApcCall
{
  BSTR i_bstrArchiveFilePath;
  BOOL i_fnAdminAccess;
  ICodexArchiveApi** o_ppoArchiveApi;
  BSTR* o_pbstrError;
};

BOOL __stdcall IsContainerNamesEquals(
    SCachedContainer* i_poCurrCompared,
    SCachedContainer* i_poComparedWith,
    BOOL* o_pfnIsEquals )
{
  ENSURE( i_poCurrCompared && i_poComparedWith && o_pfnIsEquals );

  * o_pfnIsEquals = FALSE;

  UINT32 nLen1 = AVE_BSTR_LEN( i_poCurrCompared->m_bstrFilePath );
  UINT32 nLen2 = AVE_BSTR_LEN( i_poComparedWith->m_bstrFilePath );

  if( nLen1 == nLen2 )
  {
    if( 0 == ::wcsncmp( i_poCurrCompared->m_bstrFilePath,
                 i_poComparedWith->m_bstrFilePath, nLen1 ) )
    {
      * o_pfnIsEquals = TRUE;
    }
  }

  return TRUE;
}

static void CALLBACK AccessArchiveApc( ULONG_PTR i_pParam )
{
  g_hRes = E_UNEXPECTED;
  g_fThreadCanExits = TRUE;

  SAccessArchiveCall* poCall
    = static_cast< SAccessArchiveCall* >(
        static_cast< SApcCall* >( (void*) i_pParam ) );
  if( ! VERIFY( poCall ) || ! VERIFY( poCall->m_hCompleted ) )
  {
    return;
  }

  poCall->m_fOk = FALSE;
  * poCall->o_ppoArchiveApi = 0;
  * poCall->o_pbstrError = 0;

  VERIFY( DisableDllUnloadTimer() );

  // Find existing object by filepath:

  SCachedContainer oCompareWith;
  oCompareWith.m_bstrFilePath = poCall->i_bstrArchiveFilePath;

  SCachedContainer* poExisting = 0;

  // Find in the admin list:
  if( ! VERIFY( g_loAdminCache.Find( & oCompareWith,
    IsContainerNamesEquals, & poExisting ) ) )
  {
    VERIFY( ::SetEvent( poCall->m_hCompleted ) );
    return;
  }

  if( ! poExisting )
  {
    // if object is not found in the admin list,
    // try to find it in the normal list:

    if( ! VERIFY( g_loNormalCache.Find( & oCompareWith,
      IsContainerNamesEquals, & poExisting ) ) )
    {
      VERIFY( ::SetEvent( poCall->m_hCompleted ) );
      return;
    }
  }

  if( poExisting )
  {
    // Found.

    // Create returned container
    CCodexArchiveApi* poReturnedContainer
      = new CCodexArchiveApi();
    if( ! VERIFY( poReturnedContainer ) )
    {
      // not a fatal error:
      g_fThreadCanExits = FALSE;
      g_hRes = E_OUTOFMEMORY;
      VERIFY( EnableDllUnloadTimerIfEmpty() );
      VERIFY( ::SetEvent( poCall->m_hCompleted ) );
      return;
    }

    // If old ref count was 1, reset autorelease timers.
    if( 1 == poExisting->m_nRefCount )
    {
      poCall->m_fOk = VERIFY(
        ::CancelWaitableTimer( poExisting->m_hAutoreleaseTimer ) );
    }

    // If exists, increment ref counter
    ++ poExisting->m_nRefCount;

    poReturnedContainer->m_poCached = poExisting;

    * poCall->o_ppoArchiveApi = poReturnedContainer;
    * poCall->o_pbstrError = 0;
  }
  else
  {
    // Not found

    // If not exists, try create a new:

    SCachedContainer* poCreated = 0;

    if( poCall->i_fnAdminAccess )
    {
      if( ! VERIFY( g_loAdminCache.Create( & poCreated ) ) )
      {
        // not a fatal error:
        g_fThreadCanExits = FALSE;
        g_hRes = E_OUTOFMEMORY;
        VERIFY( EnableDllUnloadTimerIfEmpty() );
        VERIFY( ::SetEvent( poCall->m_hCompleted ) );
        return;
      }
    }
    else
    {
      if( ! VERIFY( g_loNormalCache.Create( & poCreated ) ) )
      {
        // not a fatal error:
        g_fThreadCanExits = FALSE;
        g_hRes = E_OUTOFMEMORY;
        VERIFY( EnableDllUnloadTimerIfEmpty() );
        VERIFY( ::SetEvent( poCall->m_hCompleted ) );
        return;
      }
    }

    if( ! VERIFY( poCreated ) )
    {
      // not a fatal error:
      g_fThreadCanExits = FALSE;
      g_hRes = E_OUTOFMEMORY;
      VERIFY( EnableDllUnloadTimerIfEmpty() );
      VERIFY( ::SetEvent( poCall->m_hCompleted ) );
      return;
    }

    poCreated->m_hAutoreleaseTimer
      = ::CreateWaitableTimerW( 0, TRUE, 0 );
    if( ! VERIFY( poCreated->m_hAutoreleaseTimer ) )
    {
      // not a fatal error:
      g_fThreadCanExits = FALSE;
      g_hRes = E_OUTOFMEMORY;
      delete poCreated;
      VERIFY( EnableDllUnloadTimerIfEmpty() );
      VERIFY( ::SetEvent( poCall->m_hCompleted ) );
    }

    poCreated->m_bstrFilePath
      = ::SysAllocStringLen( poCall->i_bstrArchiveFilePath,
            AVE_BSTR_LEN( poCall->i_bstrArchiveFilePath ) );
    if( ! VERIFY( poCreated->m_bstrFilePath ) )
    {
      // not a fatal error:
      g_fThreadCanExits = FALSE;
      g_hRes = E_OUTOFMEMORY;
      VERIFY( ::CloseHandle( poCreated->m_hAutoreleaseTimer ) );
      delete poCreated;
      VERIFY( EnableDllUnloadTimerIfEmpty() );
      VERIFY( ::SetEvent( poCall->m_hCompleted ) );
    }

    // Create returned container
    CCodexArchiveApi* poReturnedContainer
      = new CCodexArchiveApi();
    if( ! VERIFY( poReturnedContainer ) )
    {
      // not a fatal error:
      g_fThreadCanExits = FALSE;
      g_hRes = E_OUTOFMEMORY;
      ::SysFreeString( poCreated->m_bstrFilePath );
      VERIFY( ::CloseHandle( poCreated->m_hAutoreleaseTimer ) );
      delete poCreated;
      VERIFY( EnableDllUnloadTimerIfEmpty() );
      VERIFY( ::SetEvent( poCall->m_hCompleted ) );
      return;
    }

    ICodexArchive* poICodex = 0;
    if( ! VERIFY( CreateCodexInterface( poCall->i_fnAdminAccess,
      & poICodex ) )
        ||
        ! poICodex )
    {
      // not a fatal error:
      g_fThreadCanExits = FALSE;
      // hresult has been set in the CreateCodexInterface() call.
      ::SysFreeString( poCreated->m_bstrFilePath );
      VERIFY( ::CloseHandle( poCreated->m_hAutoreleaseTimer ) );
      delete poCreated;
      delete poReturnedContainer;
      VERIFY( EnableDllUnloadTimerIfEmpty() );
      VERIFY( ::SetEvent( poCall->m_hCompleted ) );
      return;
    }

    if( S_OK != poICodex->OpenArchive( poCreated->m_bstrFilePath,
      poCall->o_pbstrError ) )
    {
      // not a fatal error:
      g_fThreadCanExits = FALSE;
      g_hRes = E_OUTOFMEMORY;
      poICodex->Release();
      ::SysFreeString( poCreated->m_bstrFilePath );
      VERIFY( ::CloseHandle( poCreated->m_hAutoreleaseTimer ) );
      delete poCreated;
      delete poReturnedContainer;
      VERIFY( EnableDllUnloadTimerIfEmpty() );
      VERIFY( ::SetEvent( poCall->m_hCompleted ) );
      return;
    }

    if( poCall->i_fnAdminAccess )
    {
      if( ! VERIFY( g_loAdminCache.Insert( poCreated ) ) )
      {
        // fatal error:
        poICodex->CloseArchive();
        poICodex->Release();
        ::SysFreeString( poCreated->m_bstrFilePath );
        VERIFY( ::CloseHandle( poCreated->m_hAutoreleaseTimer ) );
        delete poCreated;
        delete poReturnedContainer;
        VERIFY( ::SetEvent( poCall->m_hCompleted ) );
        return;
      }
    }
    else
    {
      if( ! VERIFY( g_loNormalCache.Insert( poCreated ) ) )
      {
        // fatal error:
        poICodex->CloseArchive();
        poICodex->Release();
        ::SysFreeString( poCreated->m_bstrFilePath );
        VERIFY( ::CloseHandle( poCreated->m_hAutoreleaseTimer ) );
        delete poCreated;
        delete poReturnedContainer;
        VERIFY( ::SetEvent( poCall->m_hCompleted ) );
        return;
      }
    }

    poICodex->AddRef();

    poCreated->m_poICodexInternal = poICodex;

    poCreated->m_fnAdminAccess = poCall->i_fnAdminAccess;

    poCreated->m_nRefCount = 2;
    poReturnedContainer->m_poCached = poCreated;

    * poCall->o_ppoArchiveApi = poReturnedContainer;
  }

  poCall->m_fOk = TRUE;
  g_fThreadCanExits = FALSE;
  g_hRes = S_OK;

  VERIFY( ::SetEvent( poCall->m_hCompleted ) );
}

AccessArchiveExportedFuncType AccessArchive;

// Get access to given archive file
BOOL __stdcall AccessArchive(
  __in  BSTR i_bstrArchiveFilePath,
  __in  BOOL i_fnAdminAccess,
  __out ICodexArchiveApi** o_ppoArchiveApi,
  __out BSTR* o_pbstrError )
{
  CAveAutoGuard oGuard( & g_oAccessGuard );

  if( ! g_hManThread )
  {
    ENSURE( InitManagerThread() );
    ENSURE( g_hManThread );
  }

  SAccessArchiveCall oCall;

  oCall.i_bstrArchiveFilePath = i_bstrArchiveFilePath;
  oCall.i_fnAdminAccess = i_fnAdminAccess;
  oCall.o_ppoArchiveApi = o_ppoArchiveApi;
  oCall.o_pbstrError = o_pbstrError;

  oCall.Call( g_hManThread, AccessArchiveApc );

  return oCall.m_fOk;
}

extern "C"
BOOL WINAPI
DllMain( HINSTANCE i_hInst, DWORD i_nReason, void* )
{
  switch( i_nReason )
  {
    break; case DLL_PROCESS_ATTACH:
    {
      ENSURE( ::InitializeCriticalSectionAndSpinCount(
                  & g_oAccessGuard, 1024 ) );

      g_hInst = i_hInst;
    }

    break; case DLL_PROCESS_DETACH:
    {
      ::DeleteCriticalSection( & g_oAccessGuard );
    }
  }

  return TRUE;
}
