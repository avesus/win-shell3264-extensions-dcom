# pragma once

# include "common_header.h"

# include "./../../../avesus_commons/avecom.h"

# include "./com/codexsrv.h"

class __declspec( novtable )
ICodexArchive_wrap :
  public IUnknown_wrap,
  public ICodexArchive
{
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
    ICodexArchive,
    OpenArchive, ( 
      __in  BSTR i_bstrArchiveFilePath,
      __out BSTR* o_pbstrErrorText ),
      ( i_bstrArchiveFilePath, o_pbstrErrorText ) );
  
  IFACE_METHOD( HRESULT,
    ICodexArchive,
    ListFiles, ( 
      __in  BSTR i_bstrInFolder,
      __out SAFEARRAY** o_plbstrFiles,
      __out SAFEARRAY** o_plqFilesSizes,
      __out SAFEARRAY** o_plqFilesCompSizes,
      __out SAFEARRAY** o_plfPwdProtectedFlags ),
      ( i_bstrInFolder, o_plbstrFiles, o_plqFilesSizes,
        o_plqFilesCompSizes, o_plfPwdProtectedFlags ) );
  
  IFACE_METHOD( HRESULT,
    ICodexArchive,
    ListFolders, ( 
      __in  BSTR i_bstrInFolder,
      __out SAFEARRAY** o_plbstrFolders ),
      ( i_bstrInFolder, o_plbstrFolders ) );
  
  IFACE_METHOD( HRESULT,
    ICodexArchive,
    CloseArchive, ( void ), () );
/*
  IFACE_METHOD( HRESULT,
    ICodexArchive,
    Init, ( WCHAR* i_pwzPath ), ( i_pwzPath ) );
    */
/*
  IFACE_METHOD( HRESULT,
    ICodexArchive,
    ListItems, ( 
    __in BSTR i_swArchiveFilePath,
    __in BSTR i_swSubfolderPath,
    __in EListItems i_eWhatToList,
    __out SAFEARRAY** o_plFolders,
    __out SAFEARRAY** o_plFiles,
    __out SAFEARRAY** o_plFileSizes ),
    ( i_swArchiveFilePath, i_swSubfolderPath, i_eWhatToList,
      o_plFolders, o_plFiles, o_plFileSizes ) );

      */
};
