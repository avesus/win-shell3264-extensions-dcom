# define _WIN32_DCOM
# include <windows.h>

# include "./../avesus_commons/avedbg.h"
# include "./../avesus_commons/aveutils.h"
# include "./../avesus_commons/avecom.h"

/*
MIDL_INTERFACE("B87C7ED9-4387-46D9-B4EF-203B92FBBDE0")
ITest : public IUnknown
{
};
*/
/*

struct
__declspec( uuid( "{B87C7ED9-4387-46D9-B4EF-203B92FBBDE0}" ) )
__declspec( novtable )
ITest : public IUnknown
{
  virtual HRESULT __stdcall Test( long test ) = 0;
};

*/

/*
  BEGIN_INTERFACE_PART

  END_INTERFACE_PART

  BEGIN_INTERFACE_MAP
    INTERFACE_PART
*/

# include "./../avesus_commons/i_persist_folder.h"

class CMyCoClass :
  IMPLEMENTS( IUnknown ),
  IMPLEMENTS( IClassFactory ),
  IMPLEMENTS( IPersist ),
  IMPLEMENTS( IPersistFolder ),
  IMPLEMENTS( IPersistFolder2 ),
  public CRefCounted
{
public:

  CMyCoClass() : m_nInt( 0 )
  {
  }

  virtual ULONG
  IUnknown_AddRef( void )
  {
    ++ m_nInt;
    return CRefCounted_AddRef();
  }

  virtual ULONG
  IUnknown_Release( void )
  {
    return CRefCounted_Release();
  }

  virtual HRESULT
  IUnknown_QueryInterface( REFIID riid, void **ppvObject )
  {
    return S_OK;
  }

  virtual HRESULT
  IClassFactory_CreateInstance(
    IUnknown* pUnkOuter, REFIID riid, void **ppvObject )
  {
    ++ m_nInt;
    return S_OK;
  }
  
  virtual HRESULT
  IClassFactory_LockServer( BOOL fLock )
  {
    ++ m_nInt;
    return S_OK;
  }

  

  virtual HRESULT
  IPersist_GetClassID( CLSID *pClassID )
  {
    ++ m_nInt;
    return S_OK;
  }

  virtual HRESULT
  IPersistFolder_Initialize( __RPC__in PCIDLIST_ABSOLUTE pidl )
  {
    ++ m_nInt;
    return S_OK;
  }

  virtual HRESULT
  IPersistFolder2_GetCurFolder( PIDLIST_ABSOLUTE* o_ppoIdList )
  {
    ++ m_nInt;
    return S_OK;
  }

  int m_nInt;
};

# include "./../PureCOM/com/PureCOM.h"

# include "./../farukdemirel/32bit Delphi Shell Namespace Ext. Migration to 64bit VC++/codexsrv/com/codexsrv.h"


// # include <objbase.h>

# include "./../avesus_commons/avecomreg.h"

# include <shlobj.h>

# include "./../farukdemirel/customer_src/Codex/codexapi.h"

# include <cpl.h>

void foo( void )
{
  VERIFY( TRUE );
}

void bar( void )
{
  VERIFY( TRUE );
  return foo();
}

BOOL TryTest( void )
{

# if 0

//  int a = 1, b = 2, c = 3;
//  int i = (b | a) == c;

  //bar();

# if 0

  CMyCoClass* poMyClass = new CMyCoClass();
  IUnknown* poIUnknown = static_cast< IUnknown_wrap* >( poMyClass );
  IClassFactory* poICf
    = static_cast< IClassFactory_wrap* >( poMyClass );

  poMyClass->IUnknown_AddRef();
  poIUnknown->AddRef();
  poICf->AddRef();

  poICf->LockServer( 2 );
  poMyClass->IClassFactory_LockServer( 3 );

  IPersist* poIPersist = static_cast< IPersist_wrap* >( poMyClass );

  poIPersist->GetClassID( 0 );

  IPersistFolder* poIPersistFolder
    = static_cast< IPersistFolder_wrap* >( poMyClass );
  poIPersistFolder->Initialize( 0 );

  IPersistFolder2* poIPersistFolder2
    = static_cast< IPersistFolder2_wrap* >( poMyClass );
  poIPersistFolder2->GetCurFolder( 0 );

  poIUnknown->Release();
  poIUnknown->Release();
  poIUnknown->Release();

  // final release
  poIUnknown->Release();

  return TRUE;

# endif

/*
  IPersist* poIPersist = & oMyClass;
  poIPersist->GetClassID( 0 );
  */

//  ITest* pTest = new CTest();
//  pTest->Test( 1 );
/*
	//L"{1490ABEA-1D8F-43A8-9A16-93DD93448381}"
	if( VERIFY( RegisterShellFileExtFolder( __uuidof( TestCOM ),
		L"TestCOMComponentClass",
		L"C:\\me\\work\\my_solutions\\test_com_01\\debug\\PureCOM.dll",
		L".myxml", L"TestCOM opened myxml files",
		L"'MyXML' files, opened by TestCOM component.",
		L"%SystemRoot%\\system32\\mCodexAPI.dll,5",
		L"MyXML files Folder Menu" ) ) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
*/
# if 1

  //::MessageBoxW( 0, L"Please, attach!", L"Exposer", 0 );

/*
  HMODULE hCplDll = ::LoadLibraryW( L"mControlPanel.dll" );
  ENSURE( hCplDll );

  LONG (__stdcall* xCPlApplet)( HWND, UINT, LPARAM, LPARAM )
    = (LONG (APIENTRY*)( HWND, UINT, LPARAM, LPARAM ))
    ::GetProcAddress( hCplDll, "CPlApplet" );
  ENSURE( xCPlApplet );

  LONG nRet = xCPlApplet( 0, CPL_INIT, 0, 0 );
  ENSURE( TRUE == nRet );

  nRet = xCPlApplet( 0, CPL_GETCOUNT, 0, 0 );
  ENSURE( 1 == nRet );

  CPLINFO oInfo = { 0 };
  nRet = xCPlApplet( 0, CPL_INQUIRE, 0, (LPARAM) & oInfo );
  ENSURE( 0 == nRet );

  NEWCPLINFOA oNewInfo = { 0 };
  nRet = xCPlApplet( 0, CPL_NEWINQUIRE, 0, (LPARAM) & oNewInfo );
  ENSURE( 0 == nRet );

  //ICONINFO oIconInfo = { 0 };
  //ENSURE( ::GetIconInfo( oNewInfo.hIcon, & oIconInfo ) );

  HWND hWndDesktop = ::GetDesktopWindow();
  HDC hDc = ::GetDC( hWndDesktop );
  ::DrawIcon( hDc, 0, 0, oNewInfo.hIcon );

  nRet = xCPlApplet( 0, CPL_STOP, 0, (LPARAM) oInfo.lData );
  ENSURE( 0 == nRet );

  nRet = xCPlApplet( 0, CPL_EXIT, 0, 0 );
  //ENSURE( 0 == nRet );

  ::FreeLibrary( hCplDll );
  hCplDll = 0;
  xCPlApplet = 0;

  */

  HMODULE hDll = ::LoadLibraryW( L"mCodexAPI.dll" );
  ENSURE( hDll );

  LPLOADPLUGINS xLoadPlugIns = (LPLOADPLUGINS)
    ::GetProcAddress( hDll, "LoadPlugIns" );
  ENSURE( xLoadPlugIns );

  LPFREEPLUGINS xFreePlugIns = (LPFREEPLUGINS)
    ::GetProcAddress( hDll, "FreePlugIns" );
  ENSURE( xFreePlugIns );
/*
  LPGETEXTRACTABLEARCHIVESEX
    xGetExtractableArchivesEx = (LPGETEXTRACTABLEARCHIVESEX)
    ::GetProcAddress( hDll, "GetExtractableArchivesEx" );
  ENSURE( xGetExtractableArchivesEx );
*/

  LPQUERYARCHIVEFIELDLENGTH
    xQueryArchiveFieldLength = (LPQUERYARCHIVEFIELDLENGTH)
    ::GetProcAddress( hDll, "QueryArchiveFieldLength" );
  ENSURE( xQueryArchiveFieldLength );

  LPQUERYARCHIVEEX xQueryArchiveEx = (LPQUERYARCHIVEEX)
    ::GetProcAddress( hDll, "QueryArchiveEx" );
  ENSURE( xQueryArchiveEx );

  LPEXTRACTARCHIVE xExtractArchive = (LPEXTRACTARCHIVE)
    ::GetProcAddress( hDll, "ExtractArchive" );
  ENSURE( xExtractArchive );

  INT32 nPlugins = xLoadPlugIns();
/*
  INT32 nBufSize = xGetExtractableArchivesEx( 0 );
  CHAR* pczArchives = new CHAR[ nBufSize + 1 ];
  ENSURE( pczArchives );

  INT32 nRet = xGetExtractableArchivesEx( pczArchives );
  ENSURE( nBufSize == nRet );
*/

  const CHAR* pczFileName
    //= "C:\\archives\\Sims2SP7.iso";
    //= "C:\\Загрузки\\VGA_ATI_8.332.0.0_Vistax86.zip";
    = "C:\\Загрузки\\sample_w_pass.zip";

  //CHAR aczFileName[ 1024 ] = { 0 };

  /*::WideCharToMultiByte( CP_ACP, 0, pwzFileName, ::wcslen( pwzFileName ), aczFileName,
    ALEN( aczFileName ), 0, 0 );

  FILE* f = fopen( aczFileName, "r+" );
*/
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

  INT32 nRet = xQueryArchiveFieldLength( pczFileName,
    & nItems,
    & nPwdItems,
    & nDateTimes,
    & nSizes,
    & nCompSizes );

  if( 0 == nRet && nItems )
  {
    CAutoArray< CHAR > aczItems;
    ENSURE( aczItems.Alloc( nItems + 1 ) );

    CAutoArray< CHAR > aczPwdItems;
    ENSURE( aczPwdItems.Alloc( nPwdItems + 1 ) );

    CAutoArray< CHAR > aczDateTimes;
    ENSURE( aczDateTimes.Alloc( nDateTimes + 1 ) );

    CAutoArray< CHAR > aczSizes;
    ENSURE( aczSizes.Alloc( nSizes + 1 ) );

    CAutoArray< CHAR > aczCompSizes;
    ENSURE( aczCompSizes.Alloc( nCompSizes + 1 ) );

    xQueryArchiveEx( aczItems.Ptr(), aczPwdItems.Ptr(),
      aczDateTimes.Ptr(), aczSizes.Ptr(), aczCompSizes.Ptr(),
      0 );

    if( aczItems.Ptr() && aczItems.Ptr()[ 0 ] )
    {
      xExtractArchive( pczFileName,
        "C:\\Users\\I.Borisenko\\Desktop\\New Folder\\New Folder\\",
        aczItems.Ptr(),
        "Default" );
    }
  }
  else if( nRet )
  {
    CAutoArray< CHAR > aczErr;
    ENSURE( aczErr.Alloc( nRet + 1 ) );

    xQueryArchiveEx( 0, 0, 0, 0, 0, aczErr.Ptr() );

  }
  else
  {
    CHAR aczErr[ 1024 ] = { 0 };
    xQueryArchiveEx( 0, 0, 0, 0, 0, aczErr );
    ::printf( "%s", aczErr );
  }

  ::MessageBoxW( 0, L"Wait", L"Msg", 0 );

  xFreePlugIns();

  ::FreeLibrary( hDll );
  hDll = 0;

  return TRUE;

# endif

# endif

  ENSURE( S_OK == CoInitializeEx( 0, COINIT_APARTMENTTHREADED ) );
  {

  /*  HWND hwndDummy
      = ::CreateWindowW( L"STATIC", L"Dummy",
      WS_VISIBLE | WS_OVERLAPPEDWINDOW, 10, 10, 100, 100,
      HWND_DESKTOP, 0, ::GetModuleHandleW( 0 ), 0 );
    ENSURE( hwndDummy );
*/

    IClassFactory* poIClassFactory = 0;
/*
    BOOL (__stdcall* xIsUserAnAdmin)( void );

    HMODULE h
*/
    HRESULT hRes = E_UNEXPECTED;
/*
    if( ! ::IsUserAnAdmin() )
    {

      BIND_OPTS3 bindOptions;
      ::memset( & bindOptions, 0, sizeof( bindOptions ) );
      bindOptions.cbStruct = sizeof( bindOptions );
      bindOptions.hwnd = 0;
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

      for( UINT32 n = 0; n < 2; ++ n )
      {
        hRes = ::CoGetObject(
          awzElevationMonikerName, & bindOptions,
          __uuidof( IClassFactory ),
          reinterpret_cast< void** >( & poIClassFactory ) );
        ENSURE( S_OK == hRes );
      }
    }
    else
    {
      for( UINT32 n = 0; n < 2; ++ n )
      {
        
      }
    }
*/
    hRes = ::CoGetClassObject( __uuidof( CodexSrv ),
	    CLSCTX_LOCAL_SERVER | CLSCTX_ACTIVATE_32_BIT_SERVER,
	    0,
	    IID_IClassFactory, (void**) & poIClassFactory );

    ENSURE( S_OK == hRes );

    //::MessageBoxW( 0, L"Class Factory created!", L"Exposer", 0 );

    ICodexArchive* pITest = 0;
		hRes = poIClassFactory->CreateInstance(
      0, __uuidof( ICodexArchive ),
			(void**) & pITest );

    ENSURE( S_OK == hRes );

    //::MessageBoxW( 0, L"Object created!", L"Exposer", 0 );

    poIClassFactory->Release();
    poIClassFactory = 0;
/*
    long rv = 0;
    hRes = pITest->CPlApplet( 1, 2, 3, 4, & rv );
		ENSURE( S_OK == hRes );
    ENSURE( 777 == rv );

    ::MessageBoxW( 0, L"Object works!", L"Exposer", 0 );
	*/
    //= "C:\\archives\\Sims2SP7.iso";
    //= "C:\\Загрузки\\VGA_ATI_8.332.0.0_Vistax86.zip";
    //= "C:\\Загрузки\\sample_w_pass.zip";

    //WCHAR awzName[] = L"\0\0C:\\Загрузки\\sample_w_pass.zip";
    /*WCHAR awzName[] = 
      L"\0\0C:\\Users\\I.Borisenko\\Desktop\\3\\3\\example.zip";

    * ((UINT32*)awzName) = (ALEN( awzName ) - 3) * sizeof( * awzName );
*/
    //BSTR bstrFilePath = ::SysAllocString(     );

    AVE_BSTR_CONST( FilePath, L"C:\\Users\\I.Borisenko\\Desktop\\3\\3\\example.zip" );

    AVE_BSTR_CONST( Root, L"./" );
/*    WCHAR awzRoot[] = L"\0\0./";
    * ((UINT32*)awzRoot) = (ALEN( awzRoot ) - 3) * sizeof( * awzRoot );
*/

    BSTR bstrErrCode = 0;
    hRes = pITest->OpenArchive( bstrFilePath, & bstrErrCode );
    if( SUCCEEDED( hRes ) )
    {
      SAFEARRAY* plFilesList = 0;
      SAFEARRAY* plFileSizes = 0;
      SAFEARRAY* plFileCompSizes = 0;
      SAFEARRAY* plPwdProtFlags = 0;
      hRes = pITest->ListFiles( bstrRoot, & plFilesList, & plFileSizes,
        & plFileCompSizes, & plPwdProtFlags );

      if( plFilesList )
      {
        BSTR* pbstrElement = 0;
        ::SafeArrayAccessData( plFilesList, (void**) & pbstrElement );

        ::MessageBoxW( 0, pbstrElement[ 0 ], L"Archive Folders: returned files", 0 );

        ::SysFreeString( pbstrElement[ 0 ] );

        ::SafeArrayUnaccessData( plFilesList );

        ::SafeArrayDestroy( plFilesList );
      }
    }

		pITest->Release();
		pITest = 0;

# if 0

    COAUTHIDENTITY oIdentity = { 0 };
    oIdentity.Domain = (USHORT*)L"DEV";
    oIdentity.DomainLength = ::wcslen( L"DEV" );
    oIdentity.User = (USHORT*)L"I.Borisenko";
    oIdentity.UserLength = ::wcslen( L"I.Borisenko" );
    oIdentity.Password = (USHORT*)L"18648TheAeons";
    oIdentity.PasswordLength = ::wcslen( L"18648TheAeons" );
    oIdentity.Flags = 2;


    COAUTHINFO oAuthInfo = {
      RPC_C_AUTHN_GSS_SCHANNEL,
      //RPC_C_AUTHN_WINNT, //RPC_C_AUTHN_NONE,
      //RPC_C_AUTHZ_DCE, //RPC_C_AUTHZ_NONE,
      RPC_C_AUTHZ_DEFAULT,
      0,
      RPC_C_AUTHN_LEVEL_DEFAULT, //RPC_C_AUTHN_LEVEL_NONE,
      //RPC_C_IMP_LEVEL_IDENTIFY, //
      RPC_C_IMP_LEVEL_IMPERSONATE,
      & oIdentity,
      EOAC_NONE };

    COSERVERINFO oReqAccess = { 0 };
    oReqAccess.pAuthInfo = & oAuthInfo;
/*
		IClassFactory* pIClassFactory = 0;
		HRESULT hRes = ::CoGetClassObject( __uuidof( CodexSrv ),
      //CLSCTX_DISABLE_AAA |
			CLSCTX_LOCAL_SERVER | CLSCTX_ACTIVATE_32_BIT_SERVER,
			& oReqAccess,
      //0,
			IID_IClassFactory, (void**) & pIClassFactory );
*/
    //ROTFLAGS_ALLOWANYCLIENT

    MULTI_QI oQi = { 0 };
    oQi.pIID = & __uuidof( ICodexControlPanel );
    HRESULT hRes = ::CoCreateInstance( __uuidof( CodexSrv ), 0,
      CLSCTX_LOCAL_SERVER | CLSCTX_ACTIVATE_32_BIT_SERVER,
      * oQi.pIID,
      (void**)& oQi.pItf );

    if( S_OK == hRes )
    {
      ::MessageBoxW( 0, L"Created!!!!!!!!", L"Exposer", 0 );
//      return TRUE;
    }
    else
    {
      WCHAR awzMsg[ 256 ] = { 0 };
      ::wsprintfW( awzMsg, L"0x%X", hRes );
      ::MessageBoxW( 0, awzMsg, L"Exposer", 0 );
      return TRUE;
    }

		ENSURE( S_OK == hRes );
/*
    IClassFactory* pIClassFactory2 = 0;
		hRes = ::CoGetClassObject( __uuidof( CodexSrv ),
			CLSCTX_LOCAL_SERVER | CLSCTX_ACTIVATE_32_BIT_SERVER,
			0,
			IID_IClassFactory, (void**) & pIClassFactory2 );
		ENSURE( S_OK == hRes );

    ICodexControlPanel* pITest = 0;
		hRes = pIClassFactory->CreateInstance(
      0, __uuidof( ICodexControlPanel ),
			(void**) & pITest );

    ENSURE( S_OK == hRes );
*/


/*
		ITest* pITest = 0;
		hRes = pIClassFactory->CreateInstance( 0, __uuidof( ITest ),
			(void**) & pITest );
		ENSURE( S_OK == hRes );
*/
//		pIClassFactory->Release();
//		pIClassFactory = 0;

//    pIClassFactory2->Release();
//		pIClassFactory2 = 0;
/*
		::MessageBoxW( 0, L"Press", L"Instantiated", 0 );
*/

    long rv = 0;
    hRes = ((ICodexControlPanel*)(oQi.pItf))->CPlApplet( 1, 2, 3, 4, & rv );
		ENSURE( S_OK == hRes );
    ENSURE( 777 == rv );
	  
		oQi.pItf->Release();
		oQi.pItf = 0;

    ::MessageBoxW( 0, L"All ok!", L"Instantiated", 0 );

# endif

  }

  ::CoUninitialize();

  return TRUE;
}

void main( void )
{
  TryTest();
}





# if 0

BOOL RegisterShellFileExtFolder(
  const CLSID & i_poClsid,
	WCHAR* i_pwzShellClassName,
	WCHAR* i_pwzDllFullPath,
	WCHAR* i_pwzFileExtToAssoc,
	WCHAR* i_pwzEnglishDescriptionClassName,
	WCHAR* i_pwzFriendlyTypeName,
	WCHAR* i_pwzDefaultIconFullPath,
	WCHAR* i_pwzMenuHandlerTextName )
{
	ENSURE( i_pwzShellClassName && i_pwzDllFullPath
	     && i_pwzFileExtToAssoc );

	ENSURE( RegisterBaseComObject( i_poClsid, i_pwzDllFullPath,
		L"Apartment" ) );

	CRegistryGuid oClassGuid;
	ENSURE( oClassGuid.Build( i_poClsid ) );

	BOOL fnOk = TRUE;

	HKEY hkClsid = 0;
	if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
	  ::RegOpenKeyExW(
	    HKEY_CLASSES_ROOT, L"CLSID", 0, KEY_ENUMERATE_SUB_KEYS,
	    & hkClsid ) ) )
	{
		// Open CLSID GUID.

		HKEY hkRegisteredClassId = 0;
		if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
			::RegOpenKeyExW(
			  hkClsid, oClassGuid.Get(), 0,
				KEY_ALL_ACCESS, & hkRegisteredClassId ) ) )
		{
			// Add default value as class name.

			fnOk = fnOk && VERIFY( ERROR_SUCCESS == ::RegSetValueExW(
				hkRegisteredClassId, 0, 0,
				REG_SZ,
				(BYTE*)i_pwzShellClassName,
				(UINT32)( (::wcsnlen( i_pwzShellClassName, 256 ) + 1)
								 * sizeof( WCHAR ) ) ) );

			// Add implemented category link.
			{
				HKEY hkImplementedCategories = 0;
				DWORD nDisp = 0;
				if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
					::RegCreateKeyExW(
						hkRegisteredClassId, L"Implemented Categories", 0, 0, 0,
						KEY_ALL_ACCESS, 0,
						& hkImplementedCategories,
						& nDisp ) ) )
				{
					HKEY hkCatShellFolderExt = 0;
					DWORD nDisp = 0;
					if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
						::RegCreateKeyExW(
							hkImplementedCategories,
							L"{00021490-0000-0000-C000-000000000046}", 0, 0, 0,
							KEY_ALL_ACCESS, 0,
							& hkCatShellFolderExt,
							& nDisp ) ) )
					{
						fnOk = fnOk && VERIFY(
							ERROR_SUCCESS == ::RegCloseKey( hkCatShellFolderExt ) );
					}

					fnOk = fnOk && VERIFY(
						ERROR_SUCCESS == ::RegCloseKey( hkImplementedCategories ) );
				}
			}
			
			// Add ProgID.
			{
				HKEY hkProgID = 0;
				DWORD nDisp = 0;
				if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
					::RegCreateKeyExW(
						hkRegisteredClassId, L"ProgID", 0, 0, 0,
						KEY_ALL_ACCESS, 0,
						& hkProgID,
						& nDisp ) ) )
				{
					// Add default value as dll path

					fnOk = fnOk && VERIFY( ERROR_SUCCESS == ::RegSetValueExW(
						hkProgID, 0, 0,
						REG_SZ,
						(BYTE*)i_pwzShellClassName,
					  (UINT32)( (::wcsnlen( i_pwzShellClassName, 256 ) + 1)
					            * sizeof( WCHAR ) ) ) );

					fnOk = fnOk && VERIFY(
					  ERROR_SUCCESS == ::RegCloseKey( hkProgID ) );
				}
			}

			// Add ShellFolder.
			{
				HKEY hkShellFolder = 0;
				DWORD nDisp = 0;
				if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
					::RegCreateKeyExW(
						hkRegisteredClassId, L"ShellFolder", 0, 0, 0,
						KEY_ALL_ACCESS, 0,
						& hkShellFolder,
						& nDisp ) ) )
				{
					// Add Attributes value

					DWORD dwAttributes = 0x200001a0;
					fnOk = fnOk && VERIFY( ERROR_SUCCESS == ::RegSetValueExW(
						hkShellFolder, L"Attributes", 0,
						REG_DWORD,
						(BYTE*) & dwAttributes, sizeof( dwAttributes ) ) );

					// Add UseDropHandler entry

					fnOk = fnOk && VERIFY( ERROR_SUCCESS == ::RegSetValueExW(
						hkShellFolder, L"UseDropHandler", 0,
						REG_SZ,
						(BYTE*)0, 0 ) );

					fnOk = fnOk && VERIFY(
					  ERROR_SUCCESS == ::RegCloseKey( hkShellFolder ) );
				}
			}
			
			fnOk = fnOk && VERIFY(
				ERROR_SUCCESS == ::RegCloseKey( hkRegisteredClassId ) );
		}

		fnOk = fnOk && VERIFY( ERROR_SUCCESS == ::RegCloseKey( hkClsid ) );
	}

	// Add class (with classname)
	{
		HKEY hkClass = 0;
		DWORD nDisp = 0;
		if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
			::RegCreateKeyExW(
				HKEY_CLASSES_ROOT, i_pwzShellClassName, 0, 0, 0,
				KEY_ALL_ACCESS, 0,
				& hkClass,
				& nDisp ) ) )
		{
			// Add default value as english readable filetype class name

			fnOk = fnOk && VERIFY( ERROR_SUCCESS == ::RegSetValueExW(
				hkClass, 0, 0,
				REG_SZ,
				(BYTE*)i_pwzEnglishDescriptionClassName,
			  (UINT32)( (::wcsnlen( i_pwzEnglishDescriptionClassName, 256 ) + 1)
			            * sizeof( WCHAR ) ) ) );

			fnOk = fnOk && VERIFY( ERROR_SUCCESS == ::RegSetValueExW(
				hkClass, L"FriendlyTypeName", 0,
				REG_EXPAND_SZ,
				(BYTE*)i_pwzFriendlyTypeName,
			  (UINT32)( (::wcsnlen( i_pwzFriendlyTypeName, 256 ) + 1)
			            * sizeof( WCHAR ) ) ) );

      // !!! TODO: Add PerceivedType: Compressed !!!


			// Add CLSID entry

			{
				HKEY hkClsidAssoc = 0;
				DWORD nDisp = 0;
				if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
					::RegCreateKeyExW(
						hkClass, L"CLSID", 0, 0, 0,
						KEY_ALL_ACCESS, 0,
						& hkClsidAssoc,
						& nDisp ) ) )
				{
					// Add default value as class GUID

					fnOk = fnOk && VERIFY( ERROR_SUCCESS == ::RegSetValueExW(
						hkClsidAssoc, 0, 0,
						REG_SZ,
						(BYTE*)oClassGuid.Get(),
						(UINT32)( (::wcsnlen( oClassGuid.Get(), 256 ) + 1)
											* sizeof( WCHAR ) ) ) );

					fnOk = fnOk && VERIFY(
						ERROR_SUCCESS == ::RegCloseKey( hkClsidAssoc ) );
				}
			}

			// Add default icon entry

			{
				HKEY hkDefaultIcon = 0;
				DWORD nDisp = 0;
				if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
					::RegCreateKeyExW(
						hkClass, L"DefaultIcon", 0, 0, 0,
						KEY_ALL_ACCESS, 0,
						& hkDefaultIcon,
						& nDisp ) ) )
				{
					// Add default value as path to icon file (or dll)

					fnOk = fnOk && VERIFY( ERROR_SUCCESS == ::RegSetValueExW(
						hkDefaultIcon, 0, 0,
						REG_EXPAND_SZ,
						(BYTE*)i_pwzDefaultIconFullPath,
						(UINT32)( (::wcsnlen( i_pwzDefaultIconFullPath, 256 ) + 1)
											* sizeof( WCHAR ) ) ) );

					fnOk = fnOk && VERIFY(
						ERROR_SUCCESS == ::RegCloseKey( hkDefaultIcon ) );
				}
			}

			// Add Shell entry

			{
				HKEY hkShell = 0;
				DWORD nDisp = 0;
				if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
					::RegCreateKeyExW(
						hkClass, L"shell", 0, 0, 0,
						KEY_ALL_ACCESS, 0,
						& hkShell,
						& nDisp ) ) )
				{

					// Add Open entry

					{
						HKEY hkShellOpen = 0;
						DWORD nDisp = 0;
						if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
							::RegCreateKeyExW(
								hkShell, L"Open", 0, 0, 0,
								KEY_ALL_ACCESS, 0,
								& hkShellOpen,
								& nDisp ) ) )
						{
							// Add Command entry

							{
								HKEY hkShellOpenCommand = 0;
								DWORD nDisp = 0;
								if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
									::RegCreateKeyExW(
										hkShellOpen, L"Command", 0, 0, 0,
										KEY_ALL_ACCESS, 0,
										& hkShellOpenCommand,
										& nDisp ) ) )
								{
									// Add default value as special Explorer-open path.

									WCHAR awzSpecialExplorerOpen[]
									  = L"%SystemRoot%\\Explorer.exe /idlist,%I,%L";
									fnOk = fnOk && VERIFY( ERROR_SUCCESS == ::RegSetValueExW(
										hkShellOpenCommand, 0, 0,
										REG_EXPAND_SZ,
										(BYTE*)awzSpecialExplorerOpen,
										(UINT32)( (::wcsnlen( awzSpecialExplorerOpen,
										            256 ) + 1)
															* sizeof( WCHAR ) ) ) );

									// Add DelegateExecute GUID

									WCHAR awzDelegateExecuteGuid[]
									  = L"{11dbb47c-a525-400b-9e80-a54615a090c0}";
									fnOk = fnOk && VERIFY( ERROR_SUCCESS == ::RegSetValueExW(
										hkShellOpenCommand, L"DelegateExecute", 0,
										REG_SZ,
										(BYTE*)awzDelegateExecuteGuid,
										(UINT32)( (::wcsnlen( awzDelegateExecuteGuid,
										            256 ) + 1)
															* sizeof( WCHAR ) ) ) );

									fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
									  ::RegCloseKey( hkShellOpenCommand ) );
								}
							}

							fnOk = fnOk && VERIFY(
							  ERROR_SUCCESS == ::RegCloseKey( hkShellOpen ) );
						}
					}

					fnOk = fnOk && VERIFY(
						ERROR_SUCCESS == ::RegCloseKey( hkShell ) );
				}
			}


			
			// Add ShellEx entry

			{
				HKEY hkShellEx = 0;
				DWORD nDisp = 0;
				if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
					::RegCreateKeyExW(
						hkClass, L"ShellEx", 0, 0, 0,
						KEY_ALL_ACCESS, 0,
						& hkShellEx,
						& nDisp ) ) )
				{

          // !!! TODO: Analize what is
          // {00021500-0000-0000-C000-000000000046} GUID
          // it can be child of shellex entry
          // and has string value
          // {597C8D9B-4729-4F55-8E93-3299C95EC9E3}

					// Add ContextMenuHandlers entry

					{
						HKEY hkContextMenuHandlers = 0;
						DWORD nDisp = 0;
						if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
							::RegCreateKeyExW(
								hkShellEx, L"ContextMenuHandlers", 0, 0, 0,
								KEY_ALL_ACCESS, 0,
								& hkContextMenuHandlers,
								& nDisp ) ) )
						{
							// Add Menu handler GUID

							{
								HKEY hkMenuHandlerGuid = 0;
								DWORD nDisp = 0;
								if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
									::RegCreateKeyExW(
										hkContextMenuHandlers, oClassGuid.Get(), 0, 0, 0,
										KEY_ALL_ACCESS, 0,
										& hkMenuHandlerGuid,
										& nDisp ) ) )
								{
									// Add default value as handler name

									fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
										::RegSetValueExW(
										  hkMenuHandlerGuid, 0, 0,
										  REG_SZ,
										  (BYTE*)i_pwzMenuHandlerTextName,
										  (UINT32)( (::wcsnlen( i_pwzMenuHandlerTextName, 256 ) + 1)
															* sizeof( WCHAR ) ) ) );

									fnOk = fnOk && VERIFY(
										ERROR_SUCCESS == ::RegCloseKey( hkMenuHandlerGuid ) );
								}
							}

							fnOk = fnOk && VERIFY(
								ERROR_SUCCESS == ::RegCloseKey( hkContextMenuHandlers ) );
						}
					}

					

					// Add DropHandler entry

					{
						HKEY hkDropHandler = 0;
						DWORD nDisp = 0;
						if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
							::RegCreateKeyExW(
								hkShellEx, L"DropHandler", 0, 0, 0,
								KEY_ALL_ACCESS, 0,
								& hkDropHandler,
								& nDisp ) ) )
						{

							// Add default value as class GUID

							fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
								::RegSetValueExW(
								  hkDropHandler, 0, 0,
								  REG_SZ,
								  (BYTE*)oClassGuid.Get(),
								  (UINT32)( (::wcsnlen( oClassGuid.Get(), 256 ) + 1)
													* sizeof( WCHAR ) ) ) );

							fnOk = fnOk && VERIFY(
							  ERROR_SUCCESS == ::RegCloseKey( hkDropHandler ) );
						}
					}

					
          // TODO: Analize if this OPTIONAL !!!???
          // Add StorageHandler entry

					{
						HKEY hkStorageHandler = 0;
						DWORD nDisp = 0;
						if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
							::RegCreateKeyExW(
								hkShellEx, L"StorageHandler", 0, 0, 0,
								KEY_ALL_ACCESS, 0,
								& hkStorageHandler,
								& nDisp ) ) )
						{

							// Add default value as class GUID

							fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
								::RegSetValueExW(
								  hkStorageHandler, 0, 0,
								  REG_SZ,
								  (BYTE*)oClassGuid.Get(),
								  (UINT32)( (::wcsnlen( oClassGuid.Get(), 256 ) + 1)
													* sizeof( WCHAR ) ) ) );

							fnOk = fnOk && VERIFY(
							  ERROR_SUCCESS == ::RegCloseKey( hkStorageHandler ) );
						}
					}


          // TODO: Add PropertySheetHandlers entry!!!


					fnOk = fnOk && VERIFY(
					  ERROR_SUCCESS == ::RegCloseKey( hkShellEx ) );
				}
			}

			fnOk = fnOk && VERIFY(
			  ERROR_SUCCESS == ::RegCloseKey( hkClass ) );
		}
	}


	// Add extension

	{
		HKEY hkExt = 0;
		DWORD nDisp = 0;
		if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
			::RegCreateKeyExW(
				HKEY_CLASSES_ROOT, i_pwzFileExtToAssoc, 0, 0, 0,
				KEY_ALL_ACCESS, 0,
				& hkExt,
				& nDisp ) ) )
		{
			// Add default value as class name

			fnOk = fnOk && VERIFY( ERROR_SUCCESS == ::RegSetValueExW(
				hkExt, 0, 0,
				REG_SZ,
				(BYTE*)i_pwzShellClassName,
			  (UINT32)( (::wcsnlen( i_pwzShellClassName, 256 ) + 1)
			            * sizeof( WCHAR ) ) ) );

			// Add OpenWithProgids

			{
				HKEY hkExtOpenWithProgids = 0;
				DWORD nDisp = 0;
				if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
					::RegCreateKeyExW(
						hkExt, L"OpenWithProgids", 0, 0, 0,
						KEY_ALL_ACCESS, 0,
						& hkExtOpenWithProgids,
						& nDisp ) ) )
				{
					fnOk = fnOk && VERIFY( ERROR_SUCCESS == ::RegSetValueExW(
						hkExtOpenWithProgids, i_pwzShellClassName, 0,
						REG_SZ,
						(BYTE*)0, 0  ) );

					fnOk = fnOk && VERIFY(
					  ERROR_SUCCESS == ::RegCloseKey( hkExtOpenWithProgids ) );
				}
			}

			// Add Persistent handler

			{
				HKEY hkExtPersistentHandler = 0;
				DWORD nDisp = 0;
				if( fnOk = fnOk && VERIFY( ERROR_SUCCESS ==
					::RegCreateKeyExW(
						hkExt, L"PersistentHandler", 0, 0, 0,
						KEY_ALL_ACCESS, 0,
						& hkExtPersistentHandler,
						& nDisp ) ) )
				{
					WCHAR awzStdPersistHandlerGuid[]
					  = L"{098f2470-bae0-11cd-b579-08002b30bfeb}";
					fnOk = fnOk && VERIFY( ERROR_SUCCESS == ::RegSetValueExW(
						hkExtPersistentHandler, 0, 0,
						REG_SZ,
						(BYTE*)awzStdPersistHandlerGuid,
						(UINT32)( (::wcsnlen( awzStdPersistHandlerGuid,
						            256 ) + 1)
											* sizeof( WCHAR ) ) ) );

					fnOk = fnOk && VERIFY(
					  ERROR_SUCCESS == ::RegCloseKey( hkExtPersistentHandler ) );
				}
			}

			fnOk = fnOk && VERIFY(
			  ERROR_SUCCESS == ::RegCloseKey( hkExt ) );
		}
	}

	return fnOk;
}

# endif


/*
class CMyCoClass
{
public:

  virtual HRESULT
  IUnknown_QueryInterface( 
      REFIID riid,
      __RPC__deref_out void** ppvObject )
  {
    return S_OK;
  }

  virtual ULONG
  IUnknown_AddRef( void )
  {
    return 1;
  }

  virtual ULONG
  IUnknown_Release( void )
  {
    return 0;
  }

  CMyCoClass() :
    m_oIUnknown( this )
  {
  }

private:

  IUnknown< CMyCoClass > m_oIUnknown;
};

virtual void CoClass::Foo()
{
}
*/

/*
template< typename TIface >
class CIfaceImpl :
	public TIface
{
public:

	CIfaceImpl() :
		m_poContainer( 0 )
	{
	}

	virtual ~CIfaceImpl()
	{
		VERIFY( m_poContainer );
		m_poContainer = 0;
	}

	BOOL SetContainer( IUnknown* i_poContainer )
	{
		ENSURE( i_poContainer && ! m_poContainer );

		m_poContainer = i_poContainer;

		return TRUE;
	}

	virtual ULONG STDMETHODCALLTYPE
	AddRef( void )
	{
		return m_poContainer->AddRef();
	}

	virtual ULONG STDMETHODCALLTYPE
	Release( void )
	{
		return m_poContainer->Release();
	}

	virtual HRESULT STDMETHODCALLTYPE
	QueryInterface(
		__in  const IID &  i_poReqIfaceId,
		__out       void** o_ppoIface )
	{
		return m_poContainer->QueryInterface(
			i_poReqIfaceId, o_ppoIface );
	}

protected:

	IUnknown* m_poContainer;
};
*/


# if 0

class IUnknown_impl : public IUnknown
{

private:

  virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
      REFIID riid,
      __RPC__deref_out void** ppvObject )
  {
    return IUnknown_QueryInterface( riid, ppvObject );
  }

  virtual ULONG STDMETHODCALLTYPE AddRef( void )
  {
    return IUnknown_AddRef();
  }

  virtual ULONG STDMETHODCALLTYPE Release( void )
  {
    return IUnknown_Release();
  }

private:

  virtual HRESULT
  IUnknown_QueryInterface( REFIID riid, void** ppvObject ) = 0;

  virtual ULONG
  IUnknown_AddRef( void ) = 0;

  virtual ULONG
  IUnknown_Release( void ) = 0;
};

class IClassFactory_impl : 
  virtual public IUnknown_impl,
  virtual public IClassFactory
{
private:

  virtual HRESULT STDMETHODCALLTYPE
  CreateInstance( IUnknown* pUnkOuter, REFIID riid, void **ppvObject )
  {
    return IClassFactory_CreateInstance( pUnkOuter, riid, ppvObject );
  }
  
  virtual HRESULT STDMETHODCALLTYPE
  LockServer( BOOL fLock )
  {
    return IClassFactory_LockServer( fLock );
  }

private:

  virtual HRESULT
  IClassFactory_CreateInstance(
    IUnknown* pUnkOuter, REFIID riid, void **ppvObject ) = 0;
  
  virtual HRESULT
  IClassFactory_LockServer( BOOL fLock ) = 0;


};


class CMyRefCounted : virtual public IUnknown_impl
{
protected:

  virtual HRESULT
  IUnknown_QueryInterface( REFIID riid, void** ppvObject )
  {
    return S_OK;
  }

  virtual ULONG
  IUnknown_AddRef( void )
  {
    return 1;
  }

  virtual ULONG
  IUnknown_Release( void )
  {
    return 0;
  }

};

class CMyCoClass :
  virtual public CMyRefCounted,
  virtual public IClassFactory_impl
{
protected:
/*
  virtual HRESULT
  IClassFactory_CreateInstance(
    IUnknown* pUnkOuter, REFIID riid, void **ppvObject )
  {
    return S_OK;
  }
  
  virtual HRESULT
  IClassFactory_LockServer( BOOL fLock )
  {
    return S_OK;
  }
  */
};

# endif

//#pragma vtordisp( on )
/*
class __declspec( novtable )
CRefCounted : public IUnknown
{
public:

  virtual HRESULT STDMETHODCALLTYPE
  QueryInterface( REFIID riid, void** ppvObject )
  {
    return IUnknown_QueryInterface( riid, ppvObject );
  }

  virtual ULONG STDMETHODCALLTYPE
  AddRef( void )
  {
    return IUnknown_AddRef();
  }

  virtual ULONG STDMETHODCALLTYPE
  Release( void )
  {
    return IUnknown_Release();
  }

  virtual HRESULT
  IUnknown_QueryInterface( REFIID riid, void** ppvObject )
  {
    return S_OK;
  }

  virtual ULONG
  IUnknown_AddRef( void )
  {
    return 1;
  }

  virtual ULONG
  IUnknown_Release( void )
  {
    return 0;
  }


};
*/
