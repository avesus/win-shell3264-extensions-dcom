# pragma once

# include "./../../../avesus_commons/i_enum_id_list.h"
# include "archive_folders.h"

# include "./../codexsrv/com/codexsrv.h"

class CArchiveFoldersEnumIDList :
  IMPLEMENTS( IUnknown ),
  IMPLEMENTS( IEnumIDList ),
  public CRefCounted
{
public:

  virtual HRESULT
	IUnknown_QueryInterface(
	  __in  const IID &  i_poReqIfaceId,
	  __out       void** o_ppoIface );

  virtual ULONG
  IUnknown_AddRef( void );

  virtual ULONG
  IUnknown_Release( void );

  virtual HRESULT
  IEnumIDList_Next( __in  ULONG i_celt,
        __out_ecount_part(i_celt, * o_pceltFetched)
          PITEMID_CHILD* o_prgelt,
        __out_opt ULONG* o_pceltFetched );
  
  virtual HRESULT
  IEnumIDList_Skip( ULONG i_celt );
  
  virtual HRESULT
  IEnumIDList_Reset( void );
  
  virtual HRESULT
  IEnumIDList_Clone( __RPC__deref_out_opt IEnumIDList** o_ppIEnumIDList );

  CArchiveFoldersEnumIDList(
    class CArchiveFolders* i_poParent,
    ICodexArchive* i_poArchive,
    SHCONTF i_nFlags,
    const WCHAR* i_pwzInFolderPath );

  virtual ~CArchiveFoldersEnumIDList();

private:

  class CArchiveFolders* m_poParent;

  UINT32 m_nCurrItem;
  UINT32 m_nTotalCount;

  ICodexArchive* m_poArchive;

  SHCONTF m_nFlags;

  UINT32 m_nCountOfFiles;
  UINT32 m_nCountOfFolders;
  
  SAFEARRAY* m_plFilesList;
  // ptr to unlocked m_plFilesList
  BSTR* m_abstrFiles;

  SAFEARRAY* m_plFoldersList;
  // ptr to unlocked m_plFoldersList
  BSTR* m_abstrFolders;

  SAFEARRAY* m_plFilesSizes;

  SAFEARRAY* m_plFilesCompSizes;

  SAFEARRAY* m_plPwdProtFlags;

  const WCHAR* m_pwzInFolderPath;

  BOOL m_fnFilesListGetted;
  BOOL m_fnFoldersListGetted;
};
