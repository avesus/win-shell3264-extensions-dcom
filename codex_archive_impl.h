# include "codex_srv.h"

# include "parse_tree.h"

# include "commatext_parser.h"

/*
HRESULT
CodexSrv::ICodexArchive_Init( WCHAR* i_pwzPath )
{
  ::MessageBoxW( 0, i_pwzPath, L"CodexSrv", 0 );
  return S_OK;
}
*/

/*
HRESULT
CodexSrv::ICodexArchive_ListItems( 
  __in BSTR i_swArchiveFilePath,
  __in BSTR i_swSubfolderPath,
  __in EListItems i_eWhatToList,
  __out SAFEARRAY** o_plFolders,
  __out SAFEARRAY** o_plFiles,
  __out SAFEARRAY** o_plFileSizes )

{

  * o_plFolders = ::SafeArrayCreateVector( VT_BSTR, 0, 1 ); 

  BSTR* pswElement = 0;
  ::SafeArrayAccessData( * o_plFolders, (void**) & pswElement );
  pswElement[ 0 ] = i_swArchiveFilePath;
  ::SafeArrayUnaccessData( * o_plFolders );

  if( o_plFileSizes )
  {
    * o_plFileSizes = ::SafeArrayCreateVector( VT_UI8, 0, 1 ); 

    UINT64* pqElement = 0;
    ::SafeArrayAccessData( * o_plFileSizes, (void**) & pqElement );
    pqElement[ 0 ] = 7164872534865LL;
    ::SafeArrayUnaccessData( * o_plFileSizes );
  }

  

/*  size_t nLen = ::wcslen( i_pwzArchiveFilePath );
  size_t nSize = (nLen + 1) * sizeof( WCHAR );

  ::MessageBoxW( 0, i_pwzArchiveFilePath, L"CodexSrv::ListItems", 0 );

  //if( * o_pnFoldersListSize )
  //{
  * o_pnFoldersListSize = (ULONG)nSize;
  * o_pFoldersList = (BYTE*)::CoTaskMemAlloc( nSize );
  ::wcscpy( (WCHAR*)(* o_pFoldersList), i_pwzArchiveFilePath );
  
  //}

  //if( * o_pnFilesListSize )
  //{
  * o_pnFilesListSize = (ULONG)nSize;
  * o_pFilesList = (BYTE*)::CoTaskMemAlloc( nSize );
  ::wcscpy( (WCHAR*)(* o_pFilesList), i_pwzArchiveFilePath );

  
  //}

  

  return S_OK;
}
*/


HRESULT
CodexSrv::ICodexArchive_OpenArchive(
  __in  BSTR i_bstrArchiveFilePath,
  __out BSTR* o_pbstrErrorText )
{
  if( m_fnOpened )
  {
    return S_OK;
  }

  if( o_pbstrErrorText )
  {
    * o_pbstrErrorText = 0;
  }

  if( ! i_bstrArchiveFilePath || ! i_bstrArchiveFilePath[ 0 ] )
  {
    return E_INVALIDARG;
  }

  UINT32 nPathLen = ::SysStringLen( i_bstrArchiveFilePath );
  if( ! nPathLen )
  {
    return E_INVALIDARG;
  }

  CAutoArray< CHAR > aczArchiveFilePath;
  if( ! VERIFY( aczArchiveFilePath.Alloc( nPathLen + 1 ) ) )
  {
    return E_OUTOFMEMORY;
  }

  CHAR* pczFileName = aczArchiveFilePath.Ptr();
  if( ! pczFileName )
  {
    return E_UNEXPECTED;
  }

  if( ! ::WideCharToMultiByte( CP_THREAD_ACP, WC_NO_BEST_FIT_CHARS,
    i_bstrArchiveFilePath, nPathLen,
    pczFileName, nPathLen + 1, 0, 0 ) )
  {
    return ::GetLastError();
  }

  if( ! pczFileName[ 0 ] )
  {
    return E_INVALIDARG;
  }

  // zero last char!!!
  pczFileName[ nPathLen ] = 0;

  // required buffer length for file names inside archive
  INT32 nItems = 0;
  // required buffer length for password protected file names inside archive
  INT32 nPwdItems = 0;
  // required buffer length for dates and times of files inside archive
  INT32 nDateTimes = 0;
  // required buffer length for sizes of files inside archive
  INT32 nSizes = 0;
  // required buffer length for compressed sizes of files inside archive
  INT32 nCompSizes = 0;

  //::MessageBoxA( 0, pczFileName, "CodexSrv: OpenArchive", 0 );

  // should be controlled call,
  // and must be special thread that will does TerminateThread()
  // if any stack overloads has been detected!!!
  INT32 nRet = m_oCodexApiDll.QueryArchiveFieldLength(
    pczFileName,
    & nItems,
    & nPwdItems,
    & nDateTimes,
    & nSizes,
    & nCompSizes );

  if( 0 == nRet )
  {
    ENSURE( m_aczItems.Alloc( nItems + 1 ) );

    
    ENSURE( m_aczPwdItems.Alloc( nPwdItems + 1 ) );

    
    ENSURE( m_aczDateTimes.Alloc( nDateTimes + 1 ) );

    
    ENSURE( m_aczSizes.Alloc( nSizes + 1 ) );

    ENSURE( m_aczCompSizes.Alloc( nCompSizes + 1 ) );

    m_oCodexApiDll.QueryArchiveEx( m_aczItems.Ptr(), m_aczPwdItems.Ptr(),
      0, m_aczSizes.Ptr(), m_aczCompSizes.Ptr(), 0 );

    if( m_aczItems.Ptr() && m_aczItems.Ptr()[ 0 ] )
    {
      // parse & alloc folders & files tree

      CParseTree<CHAR>::SParsedFolderPath oParsedPath;

      const CHAR* pczCommaTxt = m_aczItems.Ptr();
      UINT32 nRestLen = (UINT32)m_aczItems.Size();

      while( pczCommaTxt && nRestLen )
      {
        const CHAR* pczFoundFileName = 0;
        UINT32 nFileNameLen = 0;

        ENSURE( ExtractFilePath( & pczCommaTxt, & nRestLen,
          & oParsedPath, & pczFoundFileName,
          & nFileNameLen ) );

        if( pczFoundFileName && nFileNameLen
            && pczFoundFileName[ 0 ] )
        {
          ENSURE( m_oFilesTree.AddFile( & oParsedPath, pczFoundFileName,
            nFileNameLen, 0, 0 ) );
        }
      }

      // return S_OK later
    }
  }
  else
  {
    CAutoArray< CHAR > aczErr;

    if( nRet )
    {
      ENSURE( aczErr.Alloc( nRet + 1 ) );
    }
    else
    {
      ENSURE( aczErr.Alloc( 1024 ) );
    }

    CHAR* pczErr = aczErr.Ptr();

    m_oCodexApiDll.QueryArchiveEx( 0, 0, 0, 0, 0, pczErr );

    if( pczErr && pczErr[ 0 ] )
    {
      BSTR bstrErr = ::SysAllocStringLen( 0, aczErr.Size() - 1 );

      if( ! ::MultiByteToWideChar( CP_THREAD_ACP, MB_ERR_INVALID_CHARS,
        pczErr, aczErr.Size() - 1, bstrErr, aczErr.Size() ) )
      {
        UINT32 nErr = ::GetLastError();
        ::SysFreeString( bstrErr );
        
        return nErr;
      }

      * o_pbstrErrorText = bstrErr;
    }

    return S_FALSE;
  }

  m_fnOpened = TRUE;

  return S_OK;
}

HRESULT
CodexSrv::ICodexArchive_ListFiles( 
    __in  BSTR i_bstrInFolder,
    __out SAFEARRAY** o_plbstrFiles,
    __out SAFEARRAY** o_plqFilesSizes,
    __out SAFEARRAY** o_plqFilesCompSizes,
    __out SAFEARRAY** o_plfPwdProtectedFlags)
{
  if( ! m_fnOpened )
  {
    return S_FALSE;
  }

  if( ! i_bstrInFolder || ! i_bstrInFolder[ 0 ] )
  {
    return E_INVALIDARG;
  }

  UINT32 nPathLen = ::SysStringLen( i_bstrInFolder );
  if( ! nPathLen )
  {
    return E_INVALIDARG;
  }

  CAutoArray< CHAR > aczInFolderPath;
  if( ! VERIFY( aczInFolderPath.Alloc( nPathLen + 1 ) ) )
  {
    return E_OUTOFMEMORY;
  }

  CHAR* pczInFolderPath = aczInFolderPath.Ptr();
  if( ! pczInFolderPath )
  {
    return E_UNEXPECTED;
  }

  if( ! ::WideCharToMultiByte( CP_THREAD_ACP, WC_NO_BEST_FIT_CHARS,
    i_bstrInFolder, nPathLen,
    pczInFolderPath, nPathLen + 1, 0, 0 ) )
  {
    return ::GetLastError();
  }

  if( ! pczInFolderPath[ 0 ] )
  {
    return E_INVALIDARG;
  }

  // zero last char!!!
  pczInFolderPath[ nPathLen ] = 0;


  CParseTree<CHAR>::SParsedFolderPath oParsedPath;

  // if not root path, then find it structure:
  if( 0 != ::strncmp( pczInFolderPath, "./", 2 ) )
  {
    CHAR aczTempPath[ MAX_PATH ] = { 0 };
    ::strcat( aczTempPath, "\"" );
    ::strcat( aczTempPath, pczInFolderPath );
    ::strcat( aczTempPath, "\\uuu.txt\"" );

    const CHAR* pczCommaTxt = aczTempPath;
    UINT32 nRestLen = ::strlen( pczCommaTxt );

    const CHAR* pczFoundFileName = 0;
    UINT32 nFileNameLen = 0;

    ENSURE( ExtractFilePath( & pczCommaTxt, & nRestLen,
      & oParsedPath, & pczFoundFileName, & nFileNameLen ) );

    if( nFileNameLen != 7 )
    {
      return S_FALSE;
    }

    if( 0 != ::strncmp( pczFoundFileName, "uuu.txt", 7 ) )
    {
      return S_FALSE;
    }
  }

  CParseTree<CHAR>::SFileItem* poFiles = 0;

  BOOL fnOk = FALSE;
  UINT32 nFiles = 0;
  ENSURE( m_oFilesTree.ListFiles( & oParsedPath, & poFiles, & nFiles,
    & fnOk ) );

  if( ! fnOk )
  {
    return S_FALSE;
  }

  if( ! nFiles )
  {
    * o_plbstrFiles = 0;
    return S_OK;
  }

  * o_plbstrFiles = ::SafeArrayCreateVector( VT_BSTR, 0, nFiles );
  if( ! VERIFY( * o_plbstrFiles ) )
  {
    return E_OUTOFMEMORY;
  }

  BSTR* abstrFiles = 0;
  ::SafeArrayAccessData( * o_plbstrFiles, (void**) & abstrFiles );

  UINT32 nFile = 0;

  while( poFiles )
  {
    abstrFiles[ nFile ] = ::SysAllocStringLen( 0, poFiles->m_nStrLen );

    if( ! ::MultiByteToWideChar( CP_THREAD_ACP, MB_ERR_INVALID_CHARS,
      poFiles->m_psFileName,
      poFiles->m_nStrLen,
      abstrFiles[ nFile ],
      poFiles->m_nStrLen + 1 ) )
    {
      UINT32 nErr = ::GetLastError();
      ::SysFreeString( abstrFiles[ nFile ] );

      ::SafeArrayUnaccessData( * o_plbstrFiles );
      
      return nErr;
    }


    poFiles = poFiles->m_poNext;
    ++ nFile;
  }

  ::SafeArrayUnaccessData( * o_plbstrFiles );

  return S_OK;
}

HRESULT
CodexSrv::ICodexArchive_ListFolders( 
    __in  BSTR i_bstrInFolder,
    __out SAFEARRAY** o_plbstrFolders )
{
  if( ! m_fnOpened )
  {
    return S_FALSE;
  }

  if( ! i_bstrInFolder || ! i_bstrInFolder[ 0 ] )
  {
    return E_INVALIDARG;
  }

  UINT32 nPathLen = ::SysStringLen( i_bstrInFolder );
  if( ! nPathLen )
  {
    return E_INVALIDARG;
  }

  CAutoArray< CHAR > aczInFolderPath;
  if( ! VERIFY( aczInFolderPath.Alloc( nPathLen + 1 ) ) )
  {
    return E_OUTOFMEMORY;
  }

  CHAR* pczInFolderPath = aczInFolderPath.Ptr();
  if( ! pczInFolderPath )
  {
    return E_UNEXPECTED;
  }

  if( ! ::WideCharToMultiByte( CP_THREAD_ACP, WC_NO_BEST_FIT_CHARS,
    i_bstrInFolder, nPathLen,
    pczInFolderPath, nPathLen + 1, 0, 0 ) )
  {
    return ::GetLastError();
  }

  if( ! pczInFolderPath[ 0 ] )
  {
    return E_INVALIDARG;
  }

  // zero last char!!!
  pczInFolderPath[ nPathLen ] = 0;


  CParseTree<CHAR>::SParsedFolderPath oParsedPath;

  // if not root path, then find it structure:
  if( 0 != ::strncmp( pczInFolderPath, "./", 2 ) )
  {
    //::MessageBoxW( 0, L"adasf", L"dsfgdsf", 0 );

    CHAR aczTempPath[ MAX_PATH ] = { 0 };
    ::strcat( aczTempPath, "\"" );
    ::strcat( aczTempPath, pczInFolderPath );
    ::strcat( aczTempPath, "\\uuu.txt\"" );

    const CHAR* pczCommaTxt = aczTempPath;
    UINT32 nRestLen = ::strlen( pczCommaTxt );

    const CHAR* pczFoundFileName = 0;
    UINT32 nFileNameLen = 0;

    ENSURE( ExtractFilePath( & pczCommaTxt, & nRestLen,
      & oParsedPath, & pczFoundFileName, & nFileNameLen ) );

    if( nFileNameLen != 7 )
    {
      return S_FALSE;
    }

    if( 0 != ::strncmp( pczFoundFileName, "uuu.txt", 7 ) )
    {
      return S_FALSE;
    }
  }

  CParseTree<CHAR>::SFolderItem* poFolders = 0;

  BOOL fnOk = FALSE;
  UINT32 nFolders = 0;
  ENSURE( m_oFilesTree.ListFolders( & oParsedPath, & poFolders,
    & nFolders,
    & fnOk ) );

  if( ! fnOk )
  {
    return S_FALSE;
  }

  if( ! nFolders )
  {
    * o_plbstrFolders = 0;
    return S_OK;
  }

  * o_plbstrFolders = ::SafeArrayCreateVector( VT_BSTR, 0, nFolders );
  if( ! VERIFY( * o_plbstrFolders ) )
  {
    return E_OUTOFMEMORY;
  }

  BSTR* abstrFolders = 0;
  ::SafeArrayAccessData( * o_plbstrFolders, (void**) & abstrFolders );

  UINT32 nFolder = 0;

  while( poFolders )
  {
    abstrFolders[ nFolder ]
      = ::SysAllocStringLen( 0, poFolders->m_nStrLen );

    if( ! ::MultiByteToWideChar( CP_THREAD_ACP, MB_ERR_INVALID_CHARS,
      poFolders->m_psFolderName,
      poFolders->m_nStrLen,
      abstrFolders[ nFolder ],
      poFolders->m_nStrLen + 1 ) )
    {
      UINT32 nErr = ::GetLastError();
      ::SysFreeString( abstrFolders[ nFolder ] );

      ::SafeArrayUnaccessData( * o_plbstrFolders );
      
      return nErr;
    }


    poFolders = poFolders->m_poNext;
    ++ nFolder;
  }

  ::SafeArrayUnaccessData( * o_plbstrFolders );

  return S_OK;
}

HRESULT
CodexSrv::ICodexArchive_CloseArchive( void )
{
  return S_OK;
}
