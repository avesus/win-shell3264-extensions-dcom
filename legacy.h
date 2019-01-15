class CCodexSrvCache
{
  

  struct SAutoreleaseInfo
  {
    SCachedArchiveObject* m_poContainer;
    CCodexSrvCache* m_poMgr;
    BOOL m_fnAutoreleasePool;
  };

public:

  // o_pbstrError - pointer to BSTR with error information,
  // if archive file cannot be opened. It should be freed by
  // SysFreeString() if returned not null.
  BOOL AccessArchive(
    __in  BSTR i_bstrArchiveFilePath,
    __in  BOOL i_fnTemporary,
    __in  BOOL i_fnAdminAccess,
    __out ICodexArchive** o_ppoCodexApiForCurrentThread,
    __out ICodexArchive** o_ppoOriginalCodexApi,
    __out BSTR* o_pbstrError )
  {
    CAveAutoGuard oGuard( & m_oAccessGuard );

    ENSURE( o_ppoCodexApiForCurrentThread && o_ppoOriginalCodexApi
      && o_pbstrError && i_bstrArchiveFilePath );

    * o_ppoCodexApiForCurrentThread = 0;
    * o_ppoOriginalCodexApi = 0;
    * o_pbstrError = 0;

    if( ! m_fnFirstAccessed )
    {
      WCHAR awzPath[ 1024 ] = { 0 };
      HANDLE hCurrProcess = ::GetCurrentProcess();
      ENSURE( ::GetModuleFileNameExW( hCurrProcess, g_hInst, awzPath, ALEN( awzPath ) ) );
      ::CloseHandle( hCurrProcess );

      // increment load counter
      ::LoadLibraryW( awzPath );

      m_fnFirstAccessed = TRUE;
    }

    SCachedArchiveObject* poCached = 0;
    ENSURE( FindArchiveByName( i_bstrArchiveFilePath, i_fnAdminAccess,
      & poCached ) );

    if( poCached )
    {
      if( i_fnAdminAccess )
      {
        if( ! poCached->m_fnAdminAccess )
        {
          // if an object was found, but it has no admin rights,
          // and client requests them, such object will be
          // moved to the non-admin release list,
          // and the new admin object will be created.

          ENSURE( MoveToReleasePool( poCached ) );
          
          // create admin object

          ENSURE( CreateObject( i_bstrArchiveFilePath,
            i_fnTemporary, i_fnAdminAccess,
            & poCached, o_pbstrError ) );

          if( ! poCached )
          {
            // opening of the object failed

            return TRUE;
          }
        }
      }

      //::MessageBoxW( 0, L"Existing archive requested", L"Archive Folders", 0 );
    }

    if( ! poCached )
    {
      //::MessageBoxW( 0, L"Archive creation requested", L"Archive Folders", 0 );

      ENSURE( CreateObject( i_bstrArchiveFilePath,
            i_fnTemporary, i_fnAdminAccess,
            & poCached, o_pbstrError ) );
      if( ! poCached )
      {
        // opening of the object failed

        return TRUE;
      }
    }

    // return existing object from pool
    ENSURE( poCached->m_poObject );

    //LONG nRefCount = 
    //poCached->m_poObject->AddRef();
    /*
    WCHAR awzStr[ 1024 ] = { 0 };
    ::wsprintf( awzStr, L"AddRef(): Resulting ref count: %d", nRefCount );
    ::MessageBoxW( 0, awzStr, L"Archive Folders", 0 );*/

    ICodexArchive* poICodex = poCached->m_poObject;

    ENSURE( QueryCodexThreadInterfaceWithApc( & poICodex ) );
    if( poICodex )
    {
      poICodex->AddRef();
      * o_ppoCodexApi = poICodex;
    }

    ++ poCached->m_nCounter;

    // set timeout to 30 days
    ENSURE( CTimeUtils::SetWaitableTimerMsTimeout(
      poCached->m_hReleaseTimer, TIMEOUT_30_DAYS ) );

    //++ g_nRefCounter;

    return TRUE;
  }

  // If i_fnImmediate == TRUE, archive will be closed
  // immediately, without waiting the timeout,
  // if it has no any references to it.
  // This flag is used for handling delete notifications.
  BOOL ReleaseArchive( ICodexArchive* i_poArchiveApi,
    BOOL i_fnImmediate )
  {
    CAveAutoGuard oGuard( & m_oAccessGuard );

    BOOL fnAutoreleasePool = FALSE;

    SCachedArchiveObject* poObj = 0;

    ENSURE( FindObject( i_poArchiveApi, & poObj, & fnAutoreleasePool ) );
    ENSURE( poObj && poObj->m_poObject == i_poArchiveApi );

    //-- g_nRefCounter;

    //LONG nRefCount = 
    i_poArchiveApi->Release();
      /*
    WCHAR awzStr[ 1024 ] = { 0 };
    ::wsprintf( awzStr, L"Release(): Resulting ref count %d", nRefCount );
    ::MessageBoxW( 0, awzStr, L"Archive Folders", 0 );
    */

    -- poObj->m_nCounter;

    if( 1 == poObj->m_nCounter )
    {
      if( i_fnImmediate || ! poObj->m_hReleaseTimer )
      {
        if( poObj->m_hReleaseThread && poObj->m_hReleaseTimer )
        {
          VERIFY( CTimeUtils::SetWaitableTimerMsTimeout(
            poObj->m_hReleaseTimer, 1 ) );
          ::WaitForSingleObject( poObj->m_hReleaseThread, INFINITE );

          ::CloseHandle( poObj->m_hReleaseThread );
          poObj->m_hReleaseThread = 0;
        }

        //::MessageBoxW( 0, L"Immediate remove requested", L"Archive Folders", 0 );

        ENSURE( RemoveObject( poObj, fnAutoreleasePool ) );
        delete poObj;
      }
      else
      {
        // run autorelease timer

        //::MessageBoxW( 0, L"Autorelease timer started", L"Archive Folders", 0 );

        if( poObj->m_hReleaseTimer )
        {
          VERIFY( CTimeUtils::SetWaitableTimerMsTimeout(
            poObj->m_hReleaseTimer, TIMEOUT_30_DAYS ) );
        }

        if( ! poObj->m_hReleaseThread )
        {
          SAutoreleaseInfo* poInfo = new SAutoreleaseInfo;
          if( ! VERIFY( poInfo ) )
          {
            ENSURE( RemoveObject( poObj, fnAutoreleasePool ) );
            delete poObj;
            return FALSE;
          }

          poInfo->m_poMgr = this;
          poInfo->m_poContainer = poObj;
          poInfo->m_fnAutoreleasePool = fnAutoreleasePool;

          poObj->m_hReleaseThread = (HANDLE)::_beginthreadex( 0, 0,
            AutoreleaseThread, (void*)poInfo, 0, 0 );
        }

        if( ! poObj->m_hReleaseThread )
        {
          ENSURE( RemoveObject( poObj, fnAutoreleasePool ) );
          delete poObj;
          return FALSE;
        }

        if( ! VERIFY( CTimeUtils::SetWaitableTimerMsTimeout(
          poObj->m_hReleaseTimer,
          CODEX_SRV_AUTO_UNLOAD_TIMEOUT ) ) )
        {
          ENSURE( RemoveObject( poObj, fnAutoreleasePool ) );
          delete poObj;
          return FALSE;
        }
      }
    }

    return TRUE;
  }

  ~CCodexSrvCache()
  {
    //::MessageBoxW( 0, L"Unloading requested", L"Archive folders", 0 );

    if( m_poFirst )
    {
      delete m_poFirst;
      m_poFirst = 0;
    }

    if( m_poFirstAutoRelease )
    {
      delete m_poFirstAutoRelease;
      m_poFirstAutoRelease = 0;
    }

    if( m_hCodexAccessThread )
    {
      ::SetEvent( m_hShutdown );
      ::WaitForSingleObject( m_hCodexAccessThread, INFINITE );
      ::CloseHandle( m_hCodexAccessThread );
      m_hCodexAccessThread = 0;
    }

    ::CloseHandle( m_hShutdown );
    ::CloseHandle( m_hCodexAccessThreadInitialized );

    ::DeleteCriticalSection( & m_oAccessGuard );

    if( m_poInstanceExplorer )
    {
      m_poInstanceExplorer->Release();
      m_poInstanceExplorer = 0;
    }
  }

  CCodexSrvCache() :
    m_poFirst( 0 ),
    m_poLast( 0 ),
    m_poFirstAutoRelease( 0 ),
    m_poLastAutoRelease( 0 ),
    m_hShutdown( ::CreateEvent( 0, TRUE, FALSE, 0 ) ),
    m_hCodexAccessThreadInitialized( ::CreateEvent( 0, TRUE, FALSE, 0 ) ),
    m_hCodexAccessThread( 0 ),
    m_nCodexAccessThreadId( 0 ),
    m_fnFirstAccessed( FALSE ),
    m_poInstanceExplorer( 0 )
  {
    //::MessageBoxW( 0, L"CodexSrv cache manager created", L"Archive Folders", 0 );

    ::SHGetInstanceExplorer( & m_poInstanceExplorer );
    

    ::InitializeCriticalSection( & m_oAccessGuard );
  }

  // only for use by internal structures!
  BOOL ReleaseCodexArchiveComServerWithApc( ICodexArchive* i_poICodex )
  {
    ENSURE( i_poICodex );

    ENSURE( m_hCodexAccessThread );

    if( ! QueueUserAPC(
      CodexReleaseApc, m_hCodexAccessThread, (ULONG_PTR) i_poICodex ) )
    {
      return FALSE;
    }

    return TRUE;
  }

  BOOL IsCurrentCodexAccessThread( void )
  {
    UINT32 nKillingThread = ::GetCurrentThreadId();
    BOOL fnYes = nKillingThread == m_nCodexAccessThreadId;
    return fnYes;
  }

private:

  BOOL m_fnFirstAccessed;

  IUnknown* m_poInstanceExplorer;

  CRITICAL_SECTION m_oAccessGuard;

  SCachedArchiveObject* m_poFirst;
  SCachedArchiveObject* m_poLast;

  SCachedArchiveObject* m_poFirstAutoRelease;
  SCachedArchiveObject* m_poLastAutoRelease;

  HANDLE m_hCodexAccessThread;
  UINT32 m_nCodexAccessThreadId;
  HANDLE m_hShutdown;
  HANDLE m_hCodexAccessThreadInitialized;

  struct SCodexLoadQuery
  {
    ICodexArchive* m_poICodex;
    BOOL m_fnAdminAccess;
    HANDLE m_hCompletionEvent;
  };

  struct SCodexQueryThreadInterface
  {
    ICodexArchive* m_poIOriginalCodex;
    IStream* m_poIStream;
    HANDLE m_hCompletionEvent;
  };

  static void CALLBACK CodexQueryThreadInterfaceApc( ULONG_PTR i_pParam )
  {
    SCodexQueryThreadInterface* poQuery
      = (SCodexQueryThreadInterface*)i_pParam;

    if( ! VERIFY( poQuery ) )
    {
      return;
    }

    if( ! VERIFY( poQuery->m_hCompletionEvent ) )
    {
      return;
    }

    IStream* poICodexStream = 0;
    if( S_OK == ::CoMarshalInterThreadInterfaceInStream(
      __uuidof( ICodexArchive ),
      poQuery->m_poIOriginalCodex,
      & poICodexStream ) )
    {
      if( poICodexStream )
      {
        poQuery->m_poIStream = poICodexStream;
      }
    }

    ::SetEvent( poQuery->m_hCompletionEvent );
  }

  static void CALLBACK CodexLoadApc( ULONG_PTR i_pParam )
  {
    SCodexLoadQuery* poQuery = (SCodexLoadQuery*)i_pParam;

    if( ! VERIFY( poQuery ) )
    {
      return;
    }

    if( ! VERIFY( poQuery->m_hCompletionEvent ) )
    {
      return;
    }

    ICodexArchive* poICodex = 0;

    VERIFY( LoadCodexArchiveComServer( poQuery->m_fnAdminAccess,
      & poICodex ) );

    if( poICodex )
    {
      poQuery->m_poICodex = poICodex;
    }

    ::SetEvent( poQuery->m_hCompletionEvent );
  }

  static void CALLBACK CodexReleaseApc( ULONG_PTR i_pParam )
  {
    ICodexArchive* poICodex = (ICodexArchive*)i_pParam;

    if( ! VERIFY( poICodex ) )
    {
      return;
    }

    VERIFY( ! poICodex->Release() );
  }

  BOOL QueryCodexThreadInterfaceWithApc( ICodexArchive** io_ppoICodex )
  {
    ENSURE( io_ppoICodex && * io_ppoICodex );

    ENSURE( m_hCodexAccessThread );

    SCodexQueryThreadInterface oQuery = { 0 };
    oQuery.m_poIOriginalCodex = * io_ppoICodex;
    oQuery.m_hCompletionEvent = ::CreateEventW( 0, TRUE, FALSE, 0 );
    ENSURE( oQuery.m_hCompletionEvent );

    if( WAIT_OBJECT_0 == ::WaitForSingleObject( oQuery.m_hCompletionEvent, 300000 ) )
    {
      if( ! oQuery.m_poIStream )
      {
        ::CloseHandle( oQuery.m_hCompletionEvent );
        return FALSE;
      }

      if( S_OK != ::CoGetInterfaceAndReleaseStream(
          oQuery.m_poIStream,
          __uuidof( ICodexArchive ),
          (void**) io_ppoICodex ) )
      {
        ::CloseHandle( oQuery.m_hCompletionEvent );
        return FALSE;
      }
    }

    ::CloseHandle( oQuery.m_hCompletionEvent );

    return TRUE;
  }

  BOOL LoadCodexArchiveComServerWithApc( BOOL i_fnElevated,
                                ICodexArchive** o_ppoICodex )
  {
    ENSURE( o_ppoICodex );

    if( ! m_hCodexAccessThread )
    {
      m_hCodexAccessThread = (HANDLE) ::_beginthreadex(
        0, 0, CodexAccessThread, (void*)this, 0, & m_nCodexAccessThreadId );
      ENSURE( m_hCodexAccessThread );

      ::WaitForSingleObject( m_hCodexAccessThreadInitialized, INFINITE );
    }

    SCodexLoadQuery oQuery = {
      0,
      i_fnElevated,
      ::CreateEventW( 0, TRUE, FALSE, 0 ) };
    ENSURE( oQuery.m_hCompletionEvent );

    if( ! VERIFY( QueueUserAPC(
      CodexLoadApc, m_hCodexAccessThread, (ULONG_PTR) & oQuery ) ) )
    {
      ::CloseHandle( oQuery.m_hCompletionEvent );
      return FALSE;
    }

    if( WAIT_OBJECT_0 == ::WaitForSingleObject( oQuery.m_hCompletionEvent, 300000 ) )
    {
      if( ! oQuery.m_poICodex )
      {
        ::CloseHandle( oQuery.m_hCompletionEvent );
        return FALSE;
      }

      * o_ppoICodex = oQuery.m_poICodex;
    }

    ::CloseHandle( oQuery.m_hCompletionEvent );

    return TRUE;
  }


  static UINT32 CALLBACK CodexAccessThread( void* i_pParam )
  {
    CCodexSrvCache* poCache = (CCodexSrvCache*)i_pParam;

    ENSURE( S_OK == CoInitializeEx( 0, COINIT_MULTITHREADED ) );

    ::SetEvent( poCache->m_hCodexAccessThreadInitialized );

    for(;;)
    {
      UINT32 nRet = ::WaitForSingleObjectEx(
        poCache->m_hShutdown, INFINITE, TRUE );

      // enter critsect

      // check for pending

      if( WAIT_OBJECT_0 == nRet )
      {
        break;
      }
      else if( WAIT_IO_COMPLETION == nRet )
      {
        continue;
      }
    }

    ::CoUninitialize();

    ::CloseHandle( poCache->m_hCodexAccessThread );
    poCache->m_hCodexAccessThread = 0;

    return S_OK;
  }

  static BOOL LoadCodexArchiveComServer( BOOL i_fnElevated,
                                ICodexArchive** o_ppoICodex )
  {
    ENSURE( o_ppoICodex );

    * o_ppoICodex = 0;

    IClassFactory* poIClassFactory = 0;
    HRESULT hRes = E_UNEXPECTED;

    if( i_fnElevated )
    {
      BIND_OPTS3 bindOptions;
      ::memset( & bindOptions, 0, sizeof( bindOptions ) );
      bindOptions.cbStruct = sizeof( bindOptions );
      bindOptions.hwnd = ::GetForegroundWindow();
      bindOptions.dwClassContext
        = CLSCTX_LOCAL_SERVER | CLSCTX_ACTIVATE_32_BIT_SERVER;

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
		    CLSCTX_LOCAL_SERVER | CLSCTX_ACTIVATE_32_BIT_SERVER,
		    0,
		    IID_IClassFactory, (void**) & poIClassFactory );
	    ENSURE( S_OK == hRes );
    }

    ICodexArchive* poICodex = 0;
	  hRes = poIClassFactory->CreateInstance(
      0, __uuidof( ICodexArchive ),
		  (void**) & poICodex );

    ENSURE( S_OK == hRes );

    poIClassFactory->Release();
    poIClassFactory = 0;

    * o_ppoICodex = poICodex;

    return TRUE;
  }

  static UINT32 CALLBACK AutoreleaseThread( void* i_pParam )
  {
    ENSURE( S_OK == CoInitializeEx( 0, COINIT_APARTMENTTHREADED ) );

    SAutoreleaseInfo* poInfo = (SAutoreleaseInfo*)i_pParam;

    ENSURE( poInfo );

    ENSURE( poInfo->m_poContainer && poInfo->m_poMgr );

    HANDLE hReleaseTimer = poInfo->m_poContainer->m_hReleaseTimer;

    ::WaitForSingleObject( hReleaseTimer, INFINITE );

    //::MessageBoxW( 0, L"Autorelease timer timeouted", L"Archive Folders", 0 );

    BOOL fnDoFreeLibrary = FALSE;

    {
      CAveAutoGuard oGuard( & poInfo->m_poMgr->m_oAccessGuard );

      if( 1 == poInfo->m_poContainer->m_nCounter )
      {
        ::CloseHandle( poInfo->m_poContainer->m_hReleaseThread );
        poInfo->m_poContainer->m_hReleaseThread = 0;

        ENSURE( poInfo->m_poMgr->RemoveObject(
          poInfo->m_poContainer, poInfo->m_fnAutoreleasePool ) );
        delete poInfo->m_poContainer;
      }
    
      if( ! poInfo->m_poMgr->m_poFirst
          &&
          ! poInfo->m_poMgr->m_poFirstAutoRelease
          &&
          ! g_nRefCounter )
      {
        fnDoFreeLibrary = TRUE;
      }
    }

    if( fnDoFreeLibrary )
    {
      delete poInfo->m_poMgr;
      g_poCodexCache = 0;

      ::CoUninitialize();
      ::FreeLibraryAndExitThread( g_hInst, S_OK );
    }
    else
    {
      delete poInfo;
      ::CoUninitialize();
    }

    return S_OK;
  }

  BOOL AddObject( SCachedArchiveObject* i_poObject, BOOL i_fnAutoreleasePool )
  {
    if( i_fnAutoreleasePool )
    {
      if( m_poLastAutoRelease )
      {
        ENSURE( i_poObject->LinkAfter( m_poLastAutoRelease ) );
      }
      else
      {
        m_poFirstAutoRelease = i_poObject;
      }

      m_poLastAutoRelease = i_poObject;
    }
    else
    {
      if( m_poLast )
      {
        ENSURE( i_poObject->LinkAfter( m_poLast ) );
      }
      else
      {
        m_poFirst = i_poObject;
      }

      m_poLast = i_poObject;
    }

    return TRUE;
  }

  BOOL RemoveObject( SCachedArchiveObject* i_poObject, BOOL i_fnAutoreleasePool )
  {
    if( ! i_fnAutoreleasePool )
    {
      if( m_poLast == i_poObject )
      {
        if( m_poLast == m_poFirst )
        {
          m_poFirst = m_poLast = 0;
        }
        else
        {
          m_poLast = m_poLast->m_poPrev;
        }
      }
    }
    else
    {
      if( m_poLastAutoRelease == i_poObject )
      {
        if( m_poLastAutoRelease == m_poFirstAutoRelease )
        {
          m_poFirstAutoRelease = m_poLastAutoRelease = 0;
        }
        else
        {
          m_poLastAutoRelease = m_poLastAutoRelease->m_poPrev;
        }
      }
    }

    ENSURE( i_poObject->Unlink() );

    return TRUE;
  }

  BOOL FindObject( ICodexArchive* i_poArchiveApi,
    SCachedArchiveObject** o_ppoObject,
    BOOL* o_pfnAutoreleasePool )
  {
    ENSURE( i_poArchiveApi && o_ppoObject && o_pfnAutoreleasePool );

    * o_ppoObject = 0;

    for( SCachedArchiveObject* poObj = m_poFirstAutoRelease;
         poObj;
         poObj = poObj->m_poNext )
    {
      if( poObj->m_poObject == i_poArchiveApi )
      {
        * o_ppoObject = poObj;
        * o_pfnAutoreleasePool = TRUE;
        return TRUE;
      }
    }

    for( SCachedArchiveObject* poObj = m_poFirst;
         poObj;
         poObj = poObj->m_poNext )
    {
      if( poObj->m_poObject == i_poArchiveApi )
      {
        * o_ppoObject = poObj;
        * o_pfnAutoreleasePool = FALSE;
        return TRUE;
      }
    }

    return TRUE;
  }

  BOOL MoveToReleasePool( SCachedArchiveObject* i_poObject )
  {
    ENSURE( RemoveObject( i_poObject, FALSE ) );

    ENSURE( AddObject( i_poObject, TRUE ) );

    return TRUE;
  }

  BOOL CreateObject(
    __in  BSTR i_bstrArchiveFilePath,
    __in  BOOL i_fnTemporary,
    __in  BOOL i_fnAdminAccess,
    __out SCachedArchiveObject** o_ppoCreated,
    __out BSTR* o_pbstrError )
  {
    ENSURE( i_bstrArchiveFilePath && o_ppoCreated && o_pbstrError );

    * o_ppoCreated = 0;

    ICodexArchive* poICodex = 0;
    ENSURE( LoadCodexArchiveComServerWithApc( i_fnAdminAccess, & poICodex ) );
    if( ! poICodex )
    {
      return TRUE;
    }

    BSTR bstrArchiveFilePath = ::SysAllocStringLen(
      i_bstrArchiveFilePath,
      AVE_BSTR_LEN( i_bstrArchiveFilePath ) );

    HRESULT hRes = poICodex->OpenArchive( i_bstrArchiveFilePath,
      o_pbstrError );

    ::SysFreeString( bstrArchiveFilePath );
    if( hRes != S_OK )
    {
      ::MessageBoxW( 0, L"Error opening archive", L"CodexMan", 0 );
      //poICodex->Release();
      ENSURE( ReleaseCodexArchiveComServerWithApc( poICodex ) );
      return TRUE;
    }

    SCachedArchiveObject* poCreated = new SCachedArchiveObject();
    if( ! VERIFY( poCreated ) )
    {
      ::MessageBoxW( 0, L"Error allocating memory", L"CodexMan", 0 );
      //poICodex->Release();
      ENSURE( ReleaseCodexArchiveComServerWithApc( poICodex ) );
      return FALSE;
    }

    poCreated->m_poMan = this;

    poCreated->m_bstrArchiveFilePath = ::SysAllocStringLen(
      i_bstrArchiveFilePath, AVE_BSTR_LEN( i_bstrArchiveFilePath ) );
    if( ! poCreated->m_bstrArchiveFilePath )
    {
      ::MessageBoxW( 0, L"Error allocating memory", L"CodexMan", 0 );

      delete poCreated;
      //poICodex->Release();
      ENSURE( ReleaseCodexArchiveComServerWithApc( poICodex ) );
      return FALSE;
    }

    poCreated->m_fnAdminAccess = i_fnAdminAccess;
    poCreated->m_fnDeleteOnClose = i_fnTemporary;

    if( m_poLast )
    {
      if( ! VERIFY( poCreated->LinkAfter( m_poLast ) ) )
      {
        ::MessageBoxW( 0, L"Error linking after", L"CodexMan", 0 );

        delete poCreated;
        //poICodex->Release();
        ENSURE( ReleaseCodexArchiveComServerWithApc( poICodex ) );
        return FALSE;
      }

    }
    else
    {
      m_poFirst = poCreated;
    }

    m_poLast = poCreated;

    poCreated->m_poObject = poICodexOriginal;

    * o_ppoCreated = poCreated;

    return TRUE;
  }

  // i_fnOnlyGenericPool - find object for admin access
  BOOL FindArchiveByName(
    __in  BSTR i_bstrArchiveFilePath,
    __in  BOOL i_fnOnlyGenericPool,
    __out SCachedArchiveObject** o_ppoFound )
  {
    ENSURE( i_bstrArchiveFilePath && o_ppoFound );
    * o_ppoFound = 0;

    UINT32 nStrLen = AVE_BSTR_LEN( i_bstrArchiveFilePath );

    // check for the correct format!
    ENSURE( nStrLen < 1024 );

    for( SCachedArchiveObject* poObj = m_poFirst;
         poObj;
         poObj = poObj->m_poNext )
    {
      if( AVE_BSTR_LEN( poObj->m_bstrArchiveFilePath ) == nStrLen )
      {
        if( 0 == ::wcsncmp( i_bstrArchiveFilePath,
          poObj->m_bstrArchiveFilePath, nStrLen ) )
        {
          * o_ppoFound = poObj;
          return TRUE;
        }
      }
    }

    if( ! i_fnOnlyGenericPool )
    {
      for( SCachedArchiveObject* poObj = m_poFirstAutoRelease;
           poObj;
           poObj = poObj->m_poNext )
      {
        if( AVE_BSTR_LEN( poObj->m_bstrArchiveFilePath ) == nStrLen )
        {
          if( 0 == ::wcsncmp( i_bstrArchiveFilePath,
            poObj->m_bstrArchiveFilePath, nStrLen ) )
          {
            * o_ppoFound = poObj;
            return TRUE;
          }
        }
      }
    }

    return TRUE;
  }
};


BOOL __stdcall OpenCodex(
    __in  BSTR i_bstrArchiveFilePath,
    __in  BOOL i_fnTemporary,
    __in  BOOL i_fnAdminAccess,
    __out ICodexArchive** o_ppoCodexApi,
    __out ICodexArchive** o_ppoOriginalCodexApi,
    __out BSTR* o_pbstrError )
{
  CAveAutoGuard oGuard( & g_oAccessGuard );

  if( ! g_poCodexCache )
  {
    g_poCodexCache = new CCodexSrvCache();
    // delete will be automatic,
    // or after timeout, or if AccessArchive returns no
    ENSURE( g_poCodexCache );
  }
/*
  ++ g_nAccessCount;

  WCHAR awzText[ 1024 ] = { 0 };
  ::wsprintf( awzText, L"\r\n%d Accessed\r\n", g_nAccessCount );
  ::OutputDebugStringW( awzText );
*/
  return g_poCodexCache->AccessArchive( i_bstrArchiveFilePath,
    i_fnTemporary, i_fnAdminAccess, o_ppoCodexApi,
    o_ppoOriginalCodexApi, o_pbstrError );
}

BOOL __stdcall CloseCodex( ICodexArchive* i_poArchiveApi,
    BOOL i_fnImmediate )
{
  CAveAutoGuard oGuard( & g_oAccessGuard );
  ENSURE( g_poCodexCache );

/*
  -- g_nAccessCount;

  WCHAR awzText[ 1024 ] = { 0 };
  ::wsprintf( awzText, L"\r\n%d Released\r\n", g_nAccessCount );

  ::OutputDebugStringW( awzText );
*/
  return g_poCodexCache->ReleaseArchive( i_poArchiveApi, i_fnImmediate );
}



# if 0

class IAccessImpl
{
public:

  virtual ~ICacheImpl() { }

  virtual BOOL Find()
};

class IAccessContext
{
public:

  virtual ~IAccessContext() { }
};

class ICachedContainer
{
public:

  virtual ~ICachedContainer() { }
};

template< class TAccessImpl >
class CCachedAccessManager
{
public:

  // to call through exported dll function
  static BOOL CachedAccess( IAccessContext* i_poCtxt,
                      ICachedContainer** o_ppoContainer )
  {
    * o_ppoContainer = 0;

    // APC send query message and wait response

    // Enter into dll-global crit. section
    CAveAutoGuard oGuard( & g_oAccessGuard );



    ICachedContainer* poContainer = 0;
    ENSURE( Find( i_poCtxt, & poContainer ) );
    if( ! poContainer )
    {
      ENSURE( Create( i_poCtxt, & poContainer ) );
      if( poContainer )
      {
        // add to tracking

        * o_ppoContainer = poContainer;
      }
    }
    else
    {
      ENSURE( Access( poContainer ) );
      // update tracking
      * o_ppoContainer = poContainer;
    }

    return TRUE;
  }

  // to call from DllMain()
  static BOOL OnLoadLibrary( HINSTANCE i_hDllInst )
  {
    return TRUE;
  }

  // to call from DllMain()
  static BOOL OnFreeLibrary( void )
  {
    return TRUE;
  }

private:

  CCachedAccessManager() :
     m_poAccessImpl( 0 )
  {
  }

  ~CCachedAccessManager()
  {
  }

  BOOL Init( void )
  {
    // create access impl
    return TRUE;
  }

  BOOL Create( IAccessContext* i_poCtxt,
               ICachedContainer** o_ppoContainer )
  {
    return TRUE;
  }

  BOOL Destroy( ICachedContainer* i_poContainer )
  {
    return TRUE;
  }

  BOOL Access( ICachedContainer* i_poContainer )
  {
    return TRUE;
  }

  BOOL Find( IAccessContext* i_poCtxt,
             ICachedContainer** o_ppoContainer )
  {
    return TRUE;
  }


  BOOL WorkingThread( void )
  {

  }

  IAccessImpl* m_poAccessImpl;
};

# endif

class ICachedArchiveObject
{
public:

  virtual BOOL GetICodex( __out ICodexArchive** o_ppoICodex ) = 0;

  virtual UINT32 Release( void ) = 0;

  virtual ~ICachedArchiveObject() { }
};

// manager interface for cached containers.
class ICodexSrvCache :
  public IApcQueued
{
public:

  // release codex instance identified by its stream object.
  // Method implementation should support multithreaded access.
  virtual BOOL ReleaseCached( ICachedArchiveObject* i_poCacheToRelease ) = 0;

  virtual ~ICodexSrvCache() { }
};


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

// methods
private:

  // only can be called from ICodexArchiveApi::~ICodexArchiveApi().
  virtual ~CCodexArchiveApi()
  {
    // release access to cache object

    if( VERIFY( m_poCached ) )
    {
      VERIFY( m_poCached->Release() );
      m_poCached = 0;
    }
  }

  // get access to the codex interface. Transforms IStream
  // to the current thread interface.
  // User should release given interface pointer.
  virtual BOOL GetICodex( ICodexArchive** o_ppoICodex )
  {
    ENSURE( m_poCached );

    return m_poCached->GetICodex( o_ppoICodex );
  }

// fields
public:

  ICachedArchiveObject* m_poCached;
};

class CCachedArchiveObject :
  public ICachedArchiveObject
{
public:

  CCachedArchiveObject( ICodexSrvCache* i_poHost ) :
    m_poHost( i_poHost ),
    m_nRefCount( 0 ),
    m_poIStream( 0 ),
    m_hTimer( ::CreateWaitableTimerW( 0, TRUE, 0 ) ),
    m_poIStream( 0 )
  {
  }

  virtual ~CCachedArchiveObject( void )
  {
    if( m_hTimer )
    {
      VERIFY( ::CloseHandle( m_hTimer ) );
      m_hTimer = 0;
    }
  }

  BOOL Init( void )
  {
    ENSURE( m_hTimer && m_poHost );

    return TRUE;
  }

  BOOL Free( void )
  {
    

    return TRUE;
  }

  // Нужен ли?
  BOOL GetHost( ICodexSrvCache** o_ppoHost )
  {
    ENSURE( o_ppoHost );

    * o_ppoHost = m_poHost;

    return TRUE;
  }

  virtual BOOL GetICodex( ICodexArchive** o_ppoICodex )
  {
    ENSURE( o_ppoICodex );

    * o_ppoICodex = 0;

    if( m_poIStream )
    {
      ICodexArchive* poICodex = 0;
      if( S_OK == ::CoUnmarshalInterface(
            m_poIStream,
            __uuidof( ICodexArchive ),
            (void**) & poICodex ) )
      {
        * o_ppoICodex = poICodex;
      }
    }

    return TRUE;
  }

  virtual UINT32 Release( void )
  {
    UINT32 nRet = ::InterlockedDecrement( (LONG*) & m_nRefCount );

    if( ! nRet )
    {
      struct SReleaseCacheCall :  public CApcCall
      {
        SReleaseCacheCall( ICodexSrvCache* i_poHost,
          CCachedArchiveObject* i_poCached ) :
          CApcCall( i_poHost ),
          m_poCached( i_poCached ),
          m_poHost( i_poHost )
        {
        }

        virtual ~SReleaseCacheCall() { }

        CCachedArchiveObject* m_poCached;
        ICodexSrvCache* m_poHost;

      private:

        virtual BOOL Invoke( void )
        {
          ENSURE( m_poHost && m_poCached );

          return VERIFY( m_poHost->ReleaseCached( m_poCached ) );
        }
      }
        oCall( m_poHost, this );

      VERIFY( oCall.QueueApcAndWaitCompletion() );

      return 0;
    }
    else
    {
      return nRet;
    }
  }

private:

  ICodexSrvCache* m_poHost;

  volatile UINT32 m_nRefCount;

  IStream* m_poIStream;

  HANDLE m_hTimer;
};


class CCodexSrvCache :
  public ICodexSrvCache
{
public:

  CCodexSrvCache() :
    m_hDll( 0 ),
    m_poInstanceExplorer( 0 ),
    m_hThread( 0 )
  {
  }

  virtual ~CCodexSrvCache()
  {
    if( m_poInstanceExplorer )
    {
      m_poInstanceExplorer->Release();
      m_poInstanceExplorer = 0;
    }
  }

  // Get a codex instance by filepath
  // Can be called from ANY THREAD !!!
  // This method sends a message into internal thread
  // by use of APC and waits its completion.
  // Called within the global critical section guard,
  // because in time may be unloading process.
  BOOL AccessArchive(
    __in  BSTR i_bstrArchiveFilePath,
    __in  BOOL i_fnAdminAccess,
    __out ICodexArchiveApi** o_ppoArchiveApi,
    __out BSTR* o_pbstrError )
  {
    ENSURE( i_bstrArchiveFilePath && i_bstrArchiveFilePath[ 0 ]
            && o_ppoArchiveApi && o_pbstrError );

    SAccessArchiveApcCall oCall( this );

    oCall.m_bstrArchiveFilePath = i_bstrArchiveFilePath;
    oCall.m_fnAdminAccess = i_fnAdminAccess;

    ENSURE( oCall.QueueApcAndWaitCompletion() );

    * o_ppoArchiveApi = oCall.m_poArchiveApi;
    * o_pbstrError = oCall.m_bstrError;

    return TRUE;
  }

  // does initializing control objects within current thread context,
  // then makes APC into created thread that makes all subsequent init.
  BOOL Init( void )
  {
    IUnknown* poIExplorer = 0;
    if( S_OK == ::SHGetInstanceExplorer( & poIExplorer ) )
    {
      m_poInstanceExplorer = poIExplorer;
    }

    HANDLE hThread = (HANDLE) ::_beginthreadex( 0,
      0, InternalWorkingThread_static, (void*) this, 0, 0 );
    ENSURE( hThread );

    m_hThread = hThread;

    return TRUE;
  }

private:

  struct SAccessArchiveApcCall : public CApcCall
  {
    SAccessArchiveApcCall( IApcQueued* i_poHost ) :
      CApcCall( i_poHost ),
      m_bstrArchiveFilePath( 0 ),
      m_bstrError( 0 ),
      m_poArchiveApi( 0 ),
      m_fnAdminAccess( FALSE ),
      m_poHost( static_cast< CCodexSrvCache* >( i_poHost ) )
    {
    }

    virtual ~SAccessArchiveApcCall() { }

    BSTR m_bstrArchiveFilePath;
    BOOL m_fnAdminAccess;
    ICodexArchiveApi* m_poArchiveApi;
    BSTR m_bstrError;

    CCodexSrvCache* m_poHost;

  private:

    virtual BOOL Invoke( void )
    {
      return m_poHost->AccessArchive( this );
    }
  };

  // called within internal thread context
  BOOL AccessArchive( SAccessArchiveApcCall* i_poCall )
  {

    return TRUE;
  }

  // called within internal thread context
  virtual BOOL ReleaseCached( CCachedArchiveObject* i_poCached )
  {
    return TRUE;
  }

  static void CALLBACK InternalThreadApc( ULONG_PTR i_pParam )
  {
    CApcCall* poCall = static_cast< CApcCall* >( (void*)i_pParam );
    if( VERIFY( poCall ) )
    {
      poCall->Process();
    }
  }

  virtual BOOL QueueApc( CApcCall* i_poCall )
  {
    ENSURE( i_poCall && m_hThread );

    ENSURE( ::QueueUserAPC(
      InternalThreadApc, m_hThread, (ULONG_PTR) i_poCall ) );

    return TRUE;
  }

  BOOL InternalWorkingThread( void )
  {

    // global lock

    HMODULE hDll = m_hDll;
    delete this;

    ::FreeLibraryAndExitThread( hDll, S_OK );

    return TRUE;
  }

  static UINT32 CALLBACK InternalWorkingThread_static( void* i_pParam )
  {
    CCodexSrvCache* poThis = static_cast< CCodexSrvCache* >( i_pParam );
    ENSURE( poThis );

    return poThis->InternalWorkingThread() ? S_OK : E_UNEXPECTED;
  }

  CAveLinkedList< CCachedArchiveObject > m_lpoNonAdminObjects;
  CAveLinkedList< CCachedArchiveObject > m_lpoAdminObjects;

  HMODULE m_hDll;
  IUnknown* m_poInstanceExplorer;

  HANDLE m_hThread;
};

# if 0
class CCodexSrvCache* g_poCodexCache = 0;



//UINT32 g_nAccessCount = 0;


class CCachedArchiveObject :
  public ICachedArchiveObject
{
public:

  CCachedArchiveObject( void ) :
    m_poNext( 0 ),
    m_poPrev( 0 ),
    m_bstrArchiveFilePath( 0 ),
    m_poObject( 0 ),
    m_fnAdminAccess( FALSE ),
    m_nCounter( 1 ),
    m_poMan( 0 )
  {
  }

private:

  virtual ~CCachedArchiveObject()
  {
    VERIFY( ! m_poMan );
    VERIFY( ! m_nCounter );
    VERIFY( ! m_poObject );
    VERFIY( ! m_poPrev );
    VERFIY( ! m_poNext );
    VERIFY( ! m_poPackedObject );
    VERIFY( ! m_bstrArchiveFilePath );
  }

  // get access to the codex interface. Transforms IPersist
  // to the current thread interface.
  // User should release given interface pointer.
  //
  // Can be called from ANY THREAD !!!
  virtual BOOL GetICodex( __out ICodexArchive** o_ppoICodex )
  {
    ENSURE( m_poPackedObject );

    if( S_OK ==
          ::CoUnmarshalInterface(
            m_poPackedObject,
            __uuidof( ICodexArchive ), (void**) o_ppoICodex ) )
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }

  // Can be called from ANY THREAD !!!
  virtual UINT32 Release( void )
  {
    ENSURE( m_poMan );

    return m_poMan->ReleaseCache( this );
  }

  // Only may link unlinked object!!!
  BOOL LinkAfter( CCachedArchiveObject* i_poPrev )
  {
    ENSURE( i_poPrev && ! m_poPrev && ! m_poNext );

    if( i_poPrev->m_poNext )
    {
      m_poNext = i_poPrev->m_poNext;

      m_poNext->m_poPrev = this;
    }

    i_poPrev->m_poNext = this;
    m_poPrev = i_poPrev;

    return TRUE;
  }

  BOOL Unlink( void )
  {
    if( m_poPrev )
    {
      m_poPrev->m_poNext = m_poNext;
    }

    if( m_poNext )
    {
      // unlink
      m_poNext->m_poPrev = m_poPrev;
    }

    m_poPrev = 0;
    m_poNext = 0;

    return TRUE;
  }

private:

  // list support
  CCachedArchiveObject* m_poNext;
  CCachedArchiveObject* m_poPrev;

  // full path to the archive file.
  BSTR m_bstrArchiveFilePath;

  // arhive access interface for any operations on it.
  ICodexArchive* m_poObject;
  // stream object for marshalling to other threads
  IStream* m_poPackedObject;

  // manager interface for Release() implementation.
  ICodexSrvCache* m_poMan;

  // type of object
  BOOL m_fnAdminAccess;

  // Ref counter of this object.
  // When count == 1, autorelease timer will started.
  UINT32 m_nCounter;

  // Time, when this object should be released.
  UINT64 m_qTimeToRelease;
};



class CCodexIfaceFactory
{
public:

  CCodexIfaceFactory()
  {
  }

  ~CCodexIfaceFactory()
  {
  }

  // Create new interface instance in current thread.
  BOOL Create( BOOL i_fnAdmin, ICodexArchive** o_ppoICodex )
  {
    return TRUE;
  }

private:

};

class CCachedObjectFactory
{
public:

  CCachedObjectFactory()
  {
  }

  ~CCachedObjectFactory()
  {
  }

  BOOL CreateContainer( CCachedArchiveObject** o_ppoContainer )
  {
    return TRUE;
  }

private:

};

class CCacheList
{
public:

  CCacheList() :
    m_poFirst( 0 ),
    m_poLast( 0 )
  {
  }

  ~CCacheList()
  {
  }

private:

  CCachedArchiveObject* m_poFirst;
  CCachedArchiveObject* m_poLast;
};

class CCodexSrvCache :
  public ICodexSrvCache
{
  typedef CCodexSrvCache TSelf;

public:

  CCodexSrvCache() :
    m_hInitialized( 0 ),
    m_fnInitialized( FALSE ),
    m_nNextTimeout( INFINITE )
  {
  }

  virtual ~CCodexSrvCache()
  {
    VERIFY( ! m_hInitialized );
    VERIFY( ! m_fnInitialized );
  }

  // Get a codex instance by filepath
  // Can be called from ANY THREAD !!!
  // This method sends a message into internal thread
  // by use of APC and waits its completion.
  // Called within the global critical section guard,
  // because in time may be unloading process.
  BOOL AccessArchive(
    __in  BSTR i_bstrArchiveFilePath,
    __in  BOOL i_fnAdminAccess,
    __out ICodexArchiveApi** o_ppoArchiveApi,
    __out BSTR* o_pbstrError )
  {
    ENSURE( i_bstrArchiveFilePath && i_bstrArchiveFilePath[ 0 ]
            && o_ppoArchiveApi && o_pbstrError );

    if( ! m_fnInitialized )
    {
      ENSURE( Init() );
    }



    return TRUE;
  }

  // Release codex instance identified by its stream object.
  // Can be called from ANY THREAD !!!
  // This method sends a message into internal thread
  // by use of APC and waits its completion.
  virtual BOOL ReleaseCache( ICachedArchiveObject* i_poCacheToRelease )
  {
    return TRUE;
  }

private:

  BOOL Init( void )
  {
    ENSURE( ! m_fnInitialized );

    m_hInitialized = ::CreateEventW( 0, TRUE, FALSE, 0 );
    ENSURE( m_hInitialized );

    HANDLE hThread = (HANDLE)::_beginthreadex(
      0, 0, InternalApcThread_Static, (void*) this, 0, 0 );
    if( ! VERIFY( hThread ) )
    {
      ENSURE( ::CloseHandle( m_hInitialized ) );
      return FALSE;
    }

    if( ! VERIFY( WAIT_OBJECT_0
      == ::WaitForSingleObject( m_hInitialized, 30000 ) ) )
    {
      ::WaitForSingleObject( hThread, INFINITE );
      ENSURE( ::CloseHandle( hThread ) );
      ENSURE( ::CloseHandle( m_hInitialized ) );
      return FALSE;
    }

    ENSURE( m_fnInitialized );

    ENSURE( ::CloseHandle( m_hInitialized ) );
    ENSURE( ::CloseHandle( hThread ) );
    m_hInitialized = 0;

    return TRUE;
  }

  // can be called only if there is no active object instances
  // and only within global critical section barrier.
  BOOL Free( void )
  {
    ENSURE( ! m_poFirst
            &&
            ! m_poFirstAutoRelease
            &&
            ! m_poFirstAdmin
            &&
            ! m_poFirstAdminAutoRelease );

    m_fnInitialized = FALSE;
    return TRUE;
  }

  BOOL ReleaseTimeoutedObjects( UINT32* o_pnNextTimeout )
  {
    return TRUE;
  }

  struct SCodexLoadQuery
  {
    // in
    BSTR m_bstrFilePath;
    BOOL m_fnAdminAccess;
    HANDLE m_hCompletionEvent;

    // out
    CCachedArchiveObject* m_poCreatedCached;
    BSTR* m_pbstrErr;
  };

  static void CALLBACK CodexLoadApc( ULONG_PTR i_pParam )
  {
    SCodexLoadQuery* poQuery = (SCodexLoadQuery*)i_pParam;

    if( ! VERIFY( poQuery ) )
    {
      return;
    }

    if( ! VERIFY( poQuery->m_hCompletionEvent ) )
    {
      return;
    }

    ICodexArchive* poICodex = 0;

    VERIFY( LoadCodexArchiveComServer(
      poQuery->m_fnAdminAccess, & poICodex ) );

    if( poICodex )
    {
      IStream* poICodexStream = 0;
      if( S_OK == ::CoMarshalInterThreadInterfaceInStream(
        __uuidof( ICodexArchive ),
        poICodex, & poICodexStream ) )
      {
        if( VERIFY( poICodexStream ) )
        {
          CCachedArchiveObject* poCached = new CCachedArchiveObject();
          if( VERIFY( poCached ) )
          {
            poCached->m_bstrArchiveFilePath
              = ::SysAllocStringLen( poQuery->m_bstrFilePath,
                  AVE_BSTR_LEN( poQuery->m_bstrFilePath ) );
            if( VERIFY( poQuery->m_bstrFilePath ) )
            {
              poCached->m_poMan = this;

              poCached->m_poPackedObject = poICodexStream;

              poICodex->OpenArchive( poQuery->m_bstrFilePath, 

              VERIFY( ::SetEvent( poQuery->m_hCompletionEvent ) );
              return;

              ::SysFreeString( poQuery->m_bstrFilePath );
              poQuery->m_bstrFilePath = 0;
              poCached->m_poMan = 0;
              poCached->m_poPackedObject = 0;
            }

            delete poCached;
          }

          poICodexStream->Release();
        }
      }

      poICodex->Release();
    }

    VERIFY( ::SetEvent( poQuery->m_hCompletionEvent ) );
  }

  // After exit from this method, the object shoudn't be exists!
  BOOL InternalApcThread( void )
  {
    ENSURE( m_hInitialized );

    ENSURE( S_OK == CoInitializeEx( 0, COINIT_APARTMENTTHREADED ) );

    ::SHGetInstanceExplorer( & m_poInstanceExplorer );

    WCHAR awzPath[ 1024 ] = { 0 };
    HANDLE hCurrProcess = ::GetCurrentProcess();
    ENSURE( ::GetModuleFileNameExW( hCurrProcess,
      g_hInst, awzPath, ALEN( awzPath ) ) );
    VERIFY( ::CloseHandle( hCurrProcess ) );

    // increment load counter
    VERIFY( ::LoadLibraryW( awzPath ) );

    m_fnInitialized = TRUE;

    ENSURE( ::SetEvent( m_hInitialized ) );

    // wait for commands
    for(;;)
    {
      UINT32 nObj = ::SleepEx( m_nNextTimeout, TRUE );
      if( 0 == nObj )
      { // timeout
        {
          CAveAutoGuard oGuard( & g_oAccessGuard );

          VERIFY( ReleaseTimeoutedObjects( & nNextTimeout ) );

          if( ! m_poFirst
              &&
              ! m_poFirstAutoRelease
              &&
              ! m_poFirstAdmin
              &&
              ! m_poFirstAdminAutoRelease )
          {
            ENSURE( Free() );
            return TRUE;
          }
        }
      }
      else if( WAIT_IO_COMPLETION == nObj )
      {
        continue;
      }
      else
      {
        break;
      }
    }

    {
      CAveAutoGuard oGuard( & g_oAccessGuard );
      ENSURE( Free() );
    }

    return FALSE;
  }

  static UINT32 CALLBACK InternalApcThread_Static( void* i_pParam )
  {
    if( ! i_pParam )
    {
      return E_INVALIDARG;
    }

    TSelf* poMan = (TSelf*) i_pParam;

    HRESULT hRet = VERIFY( poMan->InternalApcThread() )
      ? S_OK : E_UNEXPECTED;

    {
      CAveAutoGuard oGuard( & g_oAccessGuard );

      VERIFY( ! poMan->m_fnInitialized );
    }

    ::FreeLibraryAndExitThread( g_hInst, hRet );
  }

private:

  BOOL m_fnInitialized;
  HANDLE m_hInitialized;

  // timeout for nearest release operation.
  // accessed only within internal thread.
  UINT32 m_nNextTimeout;
};

CCodexSrvCache g_oCodexCache;

# endif

CCodexSrvCache* g_poCodexCache = 0;

