# pragma once

class ICodexArchiveApi
{
public:

  virtual ~ICodexArchiveApi() { }

  // get access to the codex interface
  virtual BOOL GetICodex( ICodexArchive** o_ppoICodex ) = 0;
};

typedef BOOL __stdcall AccessArchiveExportedFuncType(
  __in  BSTR i_bstrArchiveFilePath,
  __in  BOOL i_fnAdminAccess,
  __out ICodexArchiveApi** o_ppoArchiveApi,
  __out BSTR* o_pbstrError );