//////////////////////////////////////////////////////////////////////////
//                         Avesus Common Headers
//         Copyright (c) Ivan Borisenko 2009-2010 All Rights reserved.
//
// This file and its contents are protected by International copyright 
// laws. Unauthorized reproduction and/or distribution of all or any 
// portion of the code contained herein is strictly prohibited and will 
// result in severe civil and criminal penalties. Any violations of this 
// copyright will be prosecuted to the fullest extent possible under law.
//                                                                        
// THE SOURCE CODE CONTAINED HEREIN AND IN RELATED FILES IS THE PROPERTY  
// OF IVAN BORISENKO (HEREIN AND AFTER "SOFTWARE DEVELOPER").
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS 
// AND CONDITIONS OUTLINED IN THE SOURCE CODE LICENSE AGREEMENT EXECUTED 
// BETWEEN THE SOFTWARE DEVELOPER AND YOU.
//  
// UNDER NO CIRCUMSTANCES MAY ANY PART OF THE SOURCE CODE BE
// DISTRIBUTED, DISCLOSED OR OTHERWISE MADE AVAILABLE TO ANY THIRD PARTY 
// WITHOUT THE EXPRESS WRITTEN CONSENT OF THE SOFTWARE DEVELOPER.
//
// USER(S) OF THIS SOURCE CODE ACKNOWLEDGES THAT THIS SOURCE CODE
// CONTAINS VALUABLE AND PROPRIETARY TRADE SECRETS OF THE SOFTWARE DEVELOPER.
// USER(S) OF THIS SOURCE CODE AGREES TO MAKE EVERY EFFORT TO INSURE
// ITS CONFIDENTIALITY.
//
// THIS COPYRIGHT NOTICE MAY NOT BE REMOVED FROM THIS FILE.
//////////////////////////////////////////////////////////////////////////

# pragma once

// For simple delete registry key trees.
# include <shlwapi.h>
# pragma comment( lib, "shlwapi.lib" )

// For building access lists for accessing to com object
# include <accctrl.h>
# include <aclapi.h>

// ConvertStringSecurityDescriptorToSecurityDescriptorW
# include <sddl.h>

class CAveRegKey
{
public:

  CAveRegKey() :
    m_hKey( 0 ),
    m_hKeyParent( 0 )
  {
  }

  virtual ~CAveRegKey()
  {
    VERIFY( Close() );
  }

  // Create or open a registry key.
  BOOL Create(
    __in  HKEY i_hKeyParent,
    __in  const WCHAR* i_pwzNewKeyName,
    __in  REGSAM i_nSamDesired,
    __out BOOL* o_pfnOk,
    __out_opt BOOL* o_pfnExistingOpened )
  {
    // check registry limits
    ENSURE( ::wcsnlen( i_pwzNewKeyName, 255 ) <= 255 );

    // Store our name copy
    ENSURE( 0 == ::wcsncpy_s( m_awzKeyName, ALEN( m_awzKeyName ),
      i_pwzNewKeyName, ALEN( m_awzKeyName ) - 1 ) );

    HKEY hCreatedKeyId = 0;
		DWORD nDisp = 0;
		if( ERROR_SUCCESS == ::RegCreateKeyExW(
			  i_hKeyParent, i_pwzNewKeyName, 0, 0, 0,
			  i_nSamDesired | KEY_CREATE_SUB_KEY,
        0,
			  & hCreatedKeyId,
			  & nDisp ) )
    {
      if( o_pfnExistingOpened )
      {
        if( REG_CREATED_NEW_KEY == nDisp )
        {
          * o_pfnExistingOpened = FALSE;
        }
        else if( REG_OPENED_EXISTING_KEY == nDisp )
        {
          * o_pfnExistingOpened = TRUE;
        }
      }

      ENSURE( Close() );

      m_hKey = hCreatedKeyId;
      m_hKeyParent = i_hKeyParent;

      * o_pfnOk = TRUE;
    }

    return TRUE;
  }

  // Create or open a registry key with name of i_poNewKeyGuid.
  BOOL Create(
    __in  HKEY i_hKeyParent,
    __in  const GUID & i_poNewKeyGuid,
    __in  REGSAM i_nSamDesired,
    __out BOOL* o_pfnOk,
    __out_opt BOOL* o_pfnExistingOpened )
  {
    ENSURE( o_pfnOk );

    * o_pfnOk = FALSE;

    CRegistryGuid oKeyGuid;
	  ENSURE( oKeyGuid.Build( i_poNewKeyGuid ) );

    WCHAR* pwzKeyGuid = oKeyGuid.Get();
    ENSURE( pwzKeyGuid && pwzKeyGuid[ 0 ] );

    return VERIFY( Create( i_hKeyParent,
      pwzKeyGuid, i_nSamDesired, o_pfnOk, o_pfnExistingOpened ) );
  }

  HKEY Handle( void )
  {
    return m_hKey;
  }

  BOOL Close( void )
  {
    if( m_hKey )
    {
      ENSURE( ERROR_SUCCESS == ::RegCloseKey( m_hKey ) );
      m_hKey = 0;
      m_hKeyParent = 0;
    }

    return TRUE;
  }

  BOOL SetValue(
    __in_opt const WCHAR* i_pwzValueName,
    __in     const WCHAR* i_pwzValueData,
    __in_opt const UINT32 i_nCustomType = REG_SZ )
  {
    BOOL fnOk = FALSE;

    BYTE* pbData = 0;
    UINT32 nSize = 0;

    // support empty values
    if( i_pwzValueData )
    {
      pbData = (BYTE*)i_pwzValueData;
      nSize = (UINT32)( (::wcsnlen( i_pwzValueData, MAX_PATH ) + 1)
                * sizeof( WCHAR ) );
    }

    ENSURE( SetValue(
      i_pwzValueName, i_nCustomType, pbData, nSize, & fnOk ) );

    return fnOk;
  }

  BOOL SetValue(
    __in_opt const WCHAR* i_pwzValueName,
    __in     const UINT32 i_nValueData )
  {
    BOOL fnOk = FALSE;

    ENSURE( SetValue( i_pwzValueName, REG_DWORD,
      (BYTE*) & i_nValueData,
      (UINT32) sizeof( i_nValueData ), & fnOk ) );

    return fnOk;
  }

  BOOL SetValue(
    __in_opt const WCHAR* i_pwzValueName,
    __in     const GUID & i_poGuidValueData )
  {
    CRegistryGuid oGuid;
    ENSURE( oGuid.Build( i_poGuidValueData ) );

    return SetValue( i_pwzValueName, oGuid.Get() );
  }

  BOOL SetValue(
     __in     const GUID & i_poGuidValueName,
     __in_opt const WCHAR* i_pwzValueData )
  {
    CRegistryGuid oGuid;
    ENSURE( oGuid.Build( i_poGuidValueName ) );

    return SetValue( oGuid.Get(), i_pwzValueData );
  }

  BOOL SetValue(
    __in_opt const WCHAR* i_pwzValueName,
    __in     const UINT32 i_nType,
    __in     const BYTE*  i_pValueData,
    __in     const UINT32 i_nDataSizeInBytes,
    __out          BOOL*  o_pfnValueSetOk )
  {
    ENSURE( o_pfnValueSetOk );

    * o_pfnValueSetOk = FALSE;

    if( ERROR_SUCCESS == ::RegSetValueExW(
          m_hKey, i_pwzValueName, 0, i_nType,
          (BYTE*)i_pValueData, i_nDataSizeInBytes ) )
    {
      * o_pfnValueSetOk = TRUE;
    }

    return TRUE;
  }

  BOOL Delete( __out BOOL* o_pfnDeleted )
  {
    ENSURE( o_pfnDeleted );

    * o_pfnDeleted = FALSE;

    if( ! m_hKey )
    {
      return TRUE;
    }

    HKEY hKeyParent = m_hKeyParent;

    ENSURE( Close() );

    if( ERROR_SUCCESS == ::SHDeleteKeyW( hKeyParent, m_awzKeyName ) )
    {
      * o_pfnDeleted = TRUE;
    }

    return TRUE;
  }

private:

  HKEY  m_hKey;
  HKEY  m_hKeyParent;
  WCHAR m_awzKeyName[ 256 ];
};

class CAveCommonComReg
{
public:
  
  CAveCommonComReg()
  {
  }

  virtual ~CAveCommonComReg()
  {
  }

  // Call this method if you want to register or unregister COM-object
  BOOL InitReg(
    __in const CLSID & i_poCoClassId,
    __in const WCHAR*  i_pwzClassDescription,

    __in const WCHAR*  i_pwzModulePath,
    __out      BOOL*   o_pfnRegistered )
  {
    ENSURE( o_pfnRegistered );

    * o_pfnRegistered = FALSE;

    BOOL fnOk = FALSE;

    m_oClsid = i_poCoClassId;

    // HKEY_CLASSES_ROOT/CLSID/
    ENSURE(
      m_oClsidKey.Create( HKEY_CLASSES_ROOT, L"CLSID", 0, & fnOk, 0 ) );
    if( fnOk )
    {

      // HKEY_CLASSES_ROOT/CLSID/{CLASS GUID}
      ENSURE( m_oObjectClsidKey.Create(
        m_oClsidKey.Handle(), i_poCoClassId, KEY_ALL_ACCESS, & fnOk, 0 ) );
      if( fnOk )
      {
        fnOk = m_oObjectClsidKey.SetValue( 0, i_pwzClassDescription );
      }
    }

    ENSURE( 0 == ::wcsncpy_s( m_awzModulePath, ALEN( m_awzModulePath ),
      i_pwzModulePath, MAX_PATH ) );

    * o_pfnRegistered = fnOk;

    return TRUE;
  }


  BOOL Unreg( __out BOOL* o_pfnUnregistered )
  {
    ENSURE( m_oObjectClsidKey.Delete( o_pfnUnregistered ) );

    return TRUE;
  }

protected:

  // "CLSID" entry
  CAveRegKey m_oClsidKey;

  // Object CoClass GUID
  CLSID m_oClsid;

  // Object CoClass GUID CLSID registry key entry
  CAveRegKey m_oObjectClsidKey;

  // Module path (dll or exe) for use by any child class
  WCHAR m_awzModulePath[ MAX_PATH + 1 ];
};

class CAveInProcComReg :
  public CAveCommonComReg
{
  typedef CAveCommonComReg TParent;

public:

  CAveInProcComReg()
  {
  }

  virtual ~CAveInProcComReg()
  {
  }

  BOOL InitReg(
    __in const CLSID & i_poCoClassId,
    __in const WCHAR*  i_pwzClassDescription,
    __in const WCHAR*  i_pwzModulePath,

    __in const WCHAR*  i_pwzThreadingModel,
    __out      BOOL*   o_pfnRegistered )
  {
    ENSURE( o_pfnRegistered );

    * o_pfnRegistered = FALSE;

    BOOL fnOk = FALSE;
    ENSURE( TParent::InitReg( i_poCoClassId, i_pwzClassDescription,
      i_pwzModulePath, & fnOk ) );
    if( ! fnOk )
    {
      return TRUE;
    }

    // HKEY_CLASSES_ROOT/CLSID/{CLASS GUID}/InProcServer32
    CAveRegKey oInProcServer32;
    ENSURE( oInProcServer32.Create(
      m_oObjectClsidKey.Handle(), L"InProcServer32",
      KEY_ALL_ACCESS, & fnOk, 0 ) );
    if( fnOk )
    {

      // Default value as dll path
      ENSURE( oInProcServer32.SetValue( 0, m_awzModulePath,
        REG_EXPAND_SZ ) );

      // Threading model
      ENSURE( oInProcServer32.SetValue(
        L"ThreadingModel", i_pwzThreadingModel ) );
    }
    else
    {
      return TRUE;
    }

    // HKEY_LOCAL_MACHINE
    //   SOFTWARE
    //     Microsoft
    //       Windows
    //         CurrentVersion
    //           Shell Extensions
    //             Approved

    CAveRegKey oSoftware;
    ENSURE( oSoftware.Create( HKEY_LOCAL_MACHINE,
      L"SOFTWARE", 0, & fnOk, 0 ) );
    if( fnOk )
    {
      CAveRegKey oMicrosoft;
      ENSURE( oMicrosoft.Create( oSoftware.Handle(),
        L"Microsoft", 0, & fnOk, 0 ) );
      if( fnOk )
      {
        CAveRegKey oWindows;
        ENSURE( oWindows.Create( oMicrosoft.Handle(),
          L"Windows", 0, & fnOk, 0 ) );
        if( fnOk )
        {
          CAveRegKey oCurrentVersion;
          ENSURE( oCurrentVersion.Create( oWindows.Handle(),
            L"CurrentVersion", 0, & fnOk, 0 ) );
          if( fnOk )
          {
            CAveRegKey oShellExtensions;
            ENSURE( oShellExtensions.Create( oCurrentVersion.Handle(),
              L"Shell Extensions", 0, & fnOk, 0 ) );
            if( fnOk )
            {
              CAveRegKey oApproved;
              ENSURE( oApproved.Create( oShellExtensions.Handle(),
                L"Approved", KEY_SET_VALUE, & fnOk, 0 ) );
              if( fnOk )
              {
                * o_pfnRegistered
                  = oApproved.SetValue( m_oClsid, i_pwzClassDescription );
              }
            }
          }
        }
      }
    }

	  return TRUE;
  }
};

class CAveSrvComReg :
  public CAveCommonComReg
{
  typedef CAveCommonComReg TParent;

public:

  BOOL InitReg(
    __in const CLSID & i_poCoClassId,
    __in const WCHAR*  i_pwzClassDescription,
    __in const WCHAR*  i_pwzModulePath,
    __in const WCHAR*  i_pwzLocalizedStringResourcePath,

//    __in const CLSID & i_poAppId,
//    __in const WCHAR*  i_pwzAppDescription,

    __out      BOOL*   o_pfnRegistered )
  {
    ENSURE( o_pfnRegistered );

    * o_pfnRegistered = FALSE;

    BOOL fnOk = FALSE;
    ENSURE( TParent::InitReg( i_poCoClassId, i_pwzClassDescription,
      i_pwzModulePath, & fnOk ) );
    if( ! fnOk )
    {
      return TRUE;
    }

    if( ! m_oObjectClsidKey.SetValue( L"LocalizedString",
      i_pwzLocalizedStringResourcePath, REG_EXPAND_SZ ) )
    {
      return TRUE;
    }
/*
    // HKEY_CLASSES_ROOT/AppID
    {
      ENSURE( m_oAppId.Create( HKEY_CLASSES_ROOT, L"AppID",
        KEY_CREATE_SUB_KEY, & fnOk, 0 ) );
      if( fnOk )
      {

        ENSURE( m_oAppIdEntry.Create( m_oAppId.Handle(), i_poAppId,
          KEY_ALL_ACCESS, & fnOk, 0 ) );
        if( fnOk )
        {

          m_oAppIdEntry.SetValue( 0, i_pwzAppDescription );

          {
            SECURITY_DESCRIPTOR oSd = { 0 };
            //WCHAR* pwzSddl = L"O:BAG:BAD:(A;;0x3;;;IU)(A;;0x3;;;SY)";
            WCHAR* pwzSddl = L"O:BAG:BAD:(A;;0xb;;;WD)S:(ML;;NX;;;LW)";
            ::ConvertStringSecurityDescriptorToSecurityDescriptorW(
              pwzSddl, SDDL_REVISION_1, (PSECURITY_DESCRIPTOR*) & oSd, 0 );

            DWORD dwLen = ::GetSecurityDescriptorLength( & oSd );

            BYTE abMyAcl[] = {
              0x01, 0x00, 0x04, 0x80, 0x30, 0x00, 0x00, 0x00, 0x4c,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00,
              0x00, 0x00, 0x02, 0x00, 0x1c, 0x00, 0x01, 0x00, 0x00,
              0x00, 0x00, 0x02, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00,
              0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
              0x00, 0x00, 0x00, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x05, 0x15, 0x00, 0x00, 0x00, 0x7c, 0xeb, 0x24,
              0x0d, 0xfa, 0x4f, 0x0c, 0x2f, 0xf8, 0x9f, 0xb4, 0x74,
              0xf2, 0x03, 0x00, 0x00, 0x01, 0x05, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x05, 0x15, 0x00, 0x00, 0x00, 0x7c, 0xeb,
              0x24, 0x0d, 0xfa, 0x4f, 0x0c, 0x2f, 0xf8, 0x9f, 0xb4,
              0x74, 0x01, 0x02, 0x00, 0x00 };

            SID_IDENTIFIER_AUTHORITY oSIDAuthWorld
              = SECURITY_WORLD_SID_AUTHORITY;
            // Create a well-known SID for the Everyone group.
            
            PSID poEveryoneSID = 0;
            ENSURE( ::AllocateAndInitializeSid(
              & oSIDAuthWorld, 1, SECURITY_WORLD_RID,
              0, 0, 0, 0, 0, 0, 0, & poEveryoneSID ) );
            ENSURE( poEveryoneSID );

            // Initialize an EXPLICIT_ACCESS structure for an ACE.
            // The ACE will allow Everyone read access to the key.
            EXPLICIT_ACCESSW oExplicitAccess = { 0 };
            oExplicitAccess.grfAccessPermissions
              = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
            oExplicitAccess.grfAccessMode = SET_ACCESS;
            oExplicitAccess.grfInheritance= NO_INHERITANCE;
            //oExplicitAccess.Trustee.TrusteeForm = TRUSTEE_IS_SID;
            oExplicitAccess.Trustee.TrusteeForm = TRUSTEE_IS_NAME;
            //oExplicitAccess.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
            oExplicitAccess.Trustee.TrusteeType = TRUSTEE_IS_USER;
            //oExplicitAccess.Trustee.ptstrName  = (WCHAR*)poEveryoneSID;
            oExplicitAccess.Trustee.ptstrName  = L"DEV\\I.Borisenko";

            // Create a new ACL that contains the new ACEs.
            ACL* poAcl = 0;
            if( ! VERIFY( ERROR_SUCCESS == ::SetEntriesInAclW(
                    1, & oExplicitAccess, NULL, & poAcl ) )
                ||
                ! VERIFY( poAcl ) )
            {
              ::FreeSid( poEveryoneSID );
              return FALSE;
            }

            if( ! VERIFY( m_oAppIdEntry.SetValue(
                    L"AccessPermission", REG_BINARY,
                    //(BYTE*) poAcl, poAcl->AclSize, & fnOk ) )
                    //abMyAcl, ALEN( abMyAcl ), & fnOk ) )
                    (BYTE*) & oSd, dwLen, & fnOk ) )
                ||
                ! fnOk )
            {
              ::FreeSid( poEveryoneSID );
              ::LocalFree( poAcl );

              return FALSE;
            }

            if( ! VERIFY( m_oAppIdEntry.SetValue(
                    L"LaunchPermission", REG_BINARY,
                    //(BYTE*) poAcl, poAcl->AclSize, & fnOk ) )
                    //abMyAcl, ALEN( abMyAcl ), & fnOk ) )
                    (BYTE*) & oSd, dwLen, & fnOk ) )
                ||
                ! fnOk )
            {
              ::FreeSid( poEveryoneSID );
              ::LocalFree( poAcl );

              return FALSE;
            }

            ::FreeSid( poEveryoneSID );
            ::LocalFree( poAcl );
          }
        }
        else
        {
          return TRUE;
        }
      }
      else
      {
        return TRUE;
      }
    }
*/
    // HKEY_CLASSES_ROOT/CLSID/{CLASS GUID}/LocalServer32
    {
      CAveRegKey oLocalServer32;
      ENSURE( oLocalServer32.Create(
        m_oObjectClsidKey.Handle(), L"LocalServer32",
        KEY_ALL_ACCESS, & fnOk, 0 ) );
      if( fnOk )
      {

        // Default value as exe path
        if( ! oLocalServer32.SetValue( 0, m_awzModulePath,
          REG_EXPAND_SZ ) )
        {
          return TRUE;
        }
      }
      else
      {
        return TRUE;
      }
    }

    // HKEY_CLASSES_ROOT/CLSID/{CLASS GUID}/AppID
/*    {
      CAveRegKey oAppId;
      ENSURE( oAppId.Create(
        m_oObjectClsidKey.Handle(), L"AppID",
        KEY_ALL_ACCESS, & fnOk, 0 ) );
      if( fnOk )
      {

        // Default value as AppID GUID
        if( ! oAppId.SetValue( 0, i_poAppId ) )
        {
          return TRUE;
        }
      }
      else
      {
        return TRUE;
      }
    }
*/
    // HKEY_CLASSES_ROOT/CLSID/{CLASS GUID}/Elevation
    {
      CAveRegKey oElevation;
      ENSURE( oElevation.Create(
        m_oObjectClsidKey.Handle(), L"Elevation",
        KEY_ALL_ACCESS, & fnOk, 0 ) );
      if( fnOk )
      {

        // Elevation enabled
        if( ! oElevation.SetValue( L"Enabled", 1 ) )
        {
          return TRUE;
        }
      }
      else
      {
        return TRUE;
      }
    }

    ITypeLib* poITypeLib = 0;
    fnOk = (S_OK == ::LoadTypeLibEx( m_awzModulePath,
      REGKIND_REGISTER, & poITypeLib ) );
    if( fnOk )
    {
      ENSURE( poITypeLib );

      TLIBATTR* poAttr = 0;
      ENSURE( S_OK == poITypeLib->GetLibAttr( & poAttr ) );
      m_oTypeLibGuid = poAttr->guid;
      poITypeLib->ReleaseTLibAttr( poAttr );
      poAttr = 0;

      poITypeLib->Release();
      poITypeLib = 0;

      * o_pfnRegistered = TRUE;
    }

	  return TRUE;
  }

  BOOL Unreg( __out BOOL* o_pfnUnregistered )
  {
    ENSURE( o_pfnUnregistered );

    * o_pfnUnregistered = FALSE;

    if( S_OK != ::UnRegisterTypeLib( m_oTypeLibGuid, 1, 0,
      MAKELCID(
        MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ),
        SORT_DEFAULT ),
      SYS_WIN32 ) )
    {
      return TRUE;
    }

/*    BOOL fnOk = FALSE;
    ENSURE( m_oAppIdEntry.Delete( & fnOk ) );
    if( ! fnOk )
    {
      return TRUE;
    }
*/
    ENSURE( TParent::Unreg( o_pfnUnregistered ) );

    return TRUE;
  }

  CAveSrvComReg()
  {
    ::memset( & m_oTypeLibGuid, 0, sizeof( m_oTypeLibGuid ) );
  }

  virtual ~CAveSrvComReg()
  {
  }

protected:

  GUID m_oTypeLibGuid;

  //CAveRegKey m_oAppId;
  //CAveRegKey m_oAppIdEntry;
};
/*
class CAveFileAssocReg
{
public:

  CAveFileAssocReg()
  {
  }

  virtual ~CAveFileAssocReg()
  {
  }

  BOOL InitReg(
    __in const WCHAR*  i_pwzFileExt,
    __out      BOOL*   o_pfnRegistered )
  {
    return TRUE;
  }

  BOOL Unreg( __out BOOL* o_pfnUnregistered )
  {

    return TRUE;
  }
};
*/
# include <shobjidl.h>

class CAveShFolderInFileReg :
  public CAveInProcComReg
{
  typedef CAveInProcComReg TParent;

public:

  BOOL InitReg(
    __in const CLSID & i_poCoClassId,
    __in const WCHAR*  i_pwzClassDescription,
    __in const WCHAR*  i_pwzModulePath,

    // ProgId
    __in const WCHAR*  i_pwzFilesCategoryId,

    // ProgId default value
    __in const WCHAR*  i_pwzFilesCategoryDescription,

    // FriendlyTypeName
    __in const WCHAR*  i_pwzLocalizedFilesCategoryDescription,

    // ProgId DefaultIcon
    __in const WCHAR*  i_pwzDefaultFilesCategoryIcon,

    __in const BOOL    i_fnUseContextMenu,
    __in const BOOL    i_fnUseDragAndDrop,
    __in const BOOL    i_fnUseStorageHandler,

    // <OBJCLSID>/ShellFolder/Attributes
    __in const SFGAOF  i_nFolderObjectAttributes,
    //__in const WCHAR** i_ppwzFileExtensions,
    //__in const UINT32  i_nCountOfExtension,

    __out      BOOL*   o_pfnRegistered )
  {
    ENSURE( o_pfnRegistered );

    * o_pfnRegistered = FALSE;

    BOOL fnOk = FALSE;
    ENSURE( TParent::InitReg( i_poCoClassId, i_pwzClassDescription,
      i_pwzModulePath, L"Apartment", & fnOk ) );
    if( ! fnOk )
    {
      return TRUE;
    }


    // Add implemented category link.
    {
      CAveRegKey oImplCat;
      ENSURE( oImplCat.Create( m_oObjectClsidKey.Handle(),
        L"Implemented Categories", KEY_ALL_ACCESS, & fnOk, 0 ) );
      if( fnOk )
      {

        CAveRegKey oCatBrowsable;
        ENSURE( oCatBrowsable.Create( oImplCat.Handle(),
          L"{00021490-0000-0000-C000-000000000046}",
          KEY_ALL_ACCESS, & fnOk, 0 ) );
        if( ! fnOk )
        {
          return TRUE;
        }

      }
      else
      {
        return TRUE;
      }
    }


    // Add ProgID.
    {
      CAveRegKey oProgIdInObject;
      ENSURE( oProgIdInObject.Create( m_oObjectClsidKey.Handle(),
        L"ProgID", KEY_ALL_ACCESS, & fnOk, 0 ) );
      if( fnOk )
      {
        if( ! oProgIdInObject.SetValue( 0, i_pwzFilesCategoryId ) )
        {
          return TRUE;
        }
      }
      else
      {
        return TRUE;
      }
    }


    // Add ShellFolder.
    {
      CAveRegKey oShellFolder;
      ENSURE( oShellFolder.Create( m_oObjectClsidKey.Handle(),
        L"ShellFolder", KEY_ALL_ACCESS, & fnOk, 0 ) );
      if( fnOk )
      {
        if( ! oShellFolder.SetValue( L"Attributes",
                i_nFolderObjectAttributes ) )
        {
          return TRUE;
        }

        if( i_fnUseDragAndDrop )
        {
          if( ! oShellFolder.SetValue( L"UseDropHandler", (WCHAR*)0 ) )
          {
            return TRUE;
          }
        }
      }
      else
      {
        return TRUE;
      }
    }

    // Add ProgID record in Classes
    ENSURE( m_oProgId.Create( HKEY_CLASSES_ROOT, i_pwzFilesCategoryId,
      KEY_ALL_ACCESS, & fnOk, 0 ) );
    if( fnOk )
    {
      if( ! m_oProgId.SetValue( 0, i_pwzFilesCategoryDescription ) )
      {
        return TRUE;
      }

      if( i_pwzLocalizedFilesCategoryDescription
          &&
          i_pwzLocalizedFilesCategoryDescription[ 0 ] )
      {
        if( ! m_oProgId.SetValue( L"FriendlyTypeName",
          i_pwzLocalizedFilesCategoryDescription, REG_EXPAND_SZ ) )
        {
          return TRUE;
        }
      }

      // Add CLSID key in ProgID entry

      {
        CAveRegKey oClsId;
        ENSURE( oClsId.Create( m_oProgId.Handle(), L"CLSID",
          KEY_ALL_ACCESS, & fnOk, 0 ) );
        if( fnOk )
        {
          if( ! oClsId.SetValue( 0, i_poCoClassId ) )
          {
            return TRUE;
          }
        }
        else
        {
          return TRUE;
        }
      }

      // Add default icon key in ProgID entry

      if( i_pwzDefaultFilesCategoryIcon &&
          i_pwzDefaultFilesCategoryIcon[ 0 ] )
      {
        CAveRegKey oDefaultIcon;
        ENSURE( oDefaultIcon.Create( m_oProgId.Handle(), L"DefaultIcon",
          KEY_ALL_ACCESS, & fnOk, 0 ) );
        if( fnOk )
        {
          if( ! oDefaultIcon.SetValue( 0, i_pwzDefaultFilesCategoryIcon,
                  REG_EXPAND_SZ ) )
          {
            return TRUE;
          }
        }
        else
        {
          return TRUE;
        }
      }

      // Add Shell entry
      {
        CAveRegKey oShell;
        ENSURE( oShell.Create( m_oProgId.Handle(),
          L"shell", KEY_ALL_ACCESS, & fnOk, 0 ) );
        if( fnOk )
        {
          CAveRegKey oShellOpen;
          ENSURE( oShellOpen.Create( oShell.Handle(),
            L"Open", KEY_ALL_ACCESS, & fnOk, 0 ) );
          if( fnOk )
          {
            if( ! oShellOpen.SetValue( L"MultiSelectModel",
              L"Single" ) )
            {
              return TRUE;
            }

            // TODO: Only at XP !!!
/*
            if( ! oShellOpen.SetValue( L"BrowserFlags", 0x00000010 ) )
            {
              return TRUE;
            }

            if( ! oShellOpen.SetValue( L"ExplorerFlags", 0x00000012 ) )
            {
              return TRUE;
            }
*/
            CAveRegKey oShellOpenCommand;
            ENSURE( oShellOpenCommand.Create( oShellOpen.Handle(),
              L"Command", KEY_ALL_ACCESS, & fnOk, 0 ) );
            if( fnOk )
            {
              if( ! oShellOpenCommand.SetValue( 0,
                L"%SystemRoot%\\Explorer.exe /idlist,%I,%L",
                REG_EXPAND_SZ ) )
              {
                return TRUE;
              }

              if( ! oShellOpenCommand.SetValue( L"DelegateExecute",
                L"{11dbb47c-a525-400b-9e80-a54615a090c0}" ) )
              {
                return TRUE;
              }
            }
            else
            {
              return TRUE;
            }
          }
          else
          {
            return TRUE;
          }
        }
        else
        {
          return TRUE;
        }
      }


      // Add ShellEx entry
      {
        CAveRegKey oShellEx;
        ENSURE( oShellEx.Create( m_oProgId.Handle(),
          L"ShellEx", KEY_ALL_ACCESS, & fnOk, 0 ) );
        if( fnOk )
        {
          if( i_fnUseContextMenu )
          {
            CAveRegKey oCtxtMenu;
            ENSURE( oCtxtMenu.Create( oShellEx.Handle(),
              L"ContextMenuHandlers", KEY_ALL_ACCESS, & fnOk, 0 ) );
            if( fnOk )
            {
              CAveRegKey oCtxtMenuClsid;
              ENSURE( oCtxtMenuClsid.Create( oCtxtMenu.Handle(),
               i_poCoClassId, KEY_ALL_ACCESS, & fnOk, 0 ) );
              if( fnOk )
              {
                oCtxtMenuClsid.SetValue( 0, i_pwzFilesCategoryDescription );
              }
              else
              {
                return TRUE;
              }
            }
            else
            {
              return TRUE;
            }
          }

          if( i_fnUseDragAndDrop )
          {
            CAveRegKey oDropHandler;
            ENSURE( oDropHandler.Create( oShellEx.Handle(),
              L"DropHandler", KEY_ALL_ACCESS, & fnOk, 0 ) );
            if( fnOk )
            {
              if( ! oDropHandler.SetValue( 0,
                      i_poCoClassId ) )
              {
                return TRUE;
              }
            }
            else
            {
              return TRUE;
            }
          }

          if( i_fnUseStorageHandler )
          {
            CAveRegKey oStorHandler;
            ENSURE( oStorHandler.Create( oShellEx.Handle(),
              L"StorageHandler", KEY_ALL_ACCESS, & fnOk, 0 ) );
            if( fnOk )
            {
              if( ! oStorHandler.SetValue( 0,
                      i_poCoClassId ) )
              {
                return TRUE;
              }
            }
            else
            {
              return TRUE;
            }
          }
        }
        else
        {
          return TRUE;
        }
      }
    }
    else
    {
      return TRUE;
    }

    ///////////////////////////////////////////////
    // Associate file extensions
/*
    if( i_nCountOfExtension )
    {
      m_poRegFileAssociations
        = new CAveFileAssocReg[ i_nCountOfExtension ];
      ENSURE( m_poRegFileAssociations );

      m_nFileAssociationsCount = i_nCountOfExtension;
    }

    for( UINT32 nExt = 0; nExt < i_nCountOfExtension; ++ nExt )
    {
      const WCHAR* pwzExt = i_ppwzFileExtensions[ nExt ];

      ENSURE( pwzExt && pwzExt[ 0 ] );

      ENSURE( m_poRegFileAssociations[ nExt ].InitReg(
        i_ppwzFileExtensions[ nExt ], & fnOk ) );
      if( ! fnOk )
      {
        return TRUE;
      }
    }
*/
    * o_pfnRegistered = TRUE;

    return TRUE;
  }

  BOOL Unreg( __out BOOL* o_pfnUnregistered )
  {
    ENSURE( o_pfnUnregistered );

    * o_pfnUnregistered = FALSE;

    BOOL fnOk = FALSE;
/*    for( UINT32 nExt = 0; nExt < m_nFileAssociationsCount; ++ nExt )
    {
      fnOk = FALSE;
      ENSURE( m_poRegFileAssociations[ nExt ].Unreg( & fnOk ) );
      if( ! fnOk )
      {
        return TRUE;
      }
    }
*/
    ENSURE( m_oProgId.Delete( & fnOk ) );
    if( ! fnOk )
    {
      return TRUE;
    }

    ENSURE( TParent::Unreg( o_pfnUnregistered ) );

    return TRUE;
  }

  CAveShFolderInFileReg()
    /*:
    m_poRegFileAssociations( 0 ),
    m_nFileAssociationsCount( 0 )*/
  {
  }

  virtual ~CAveShFolderInFileReg()
  {
    /*
    if( m_poRegFileAssociations )
    {
      VERIFY( m_nFileAssociationsCount );

      delete[] m_poRegFileAssociations;
      m_poRegFileAssociations = 0;
      m_nFileAssociationsCount = 0;
    }
    */
  }

protected:

  // all of the files extension entries
  //CAveFileAssocReg* m_poRegFileAssociations;
  //UINT32 m_nFileAssociationsCount;

  // ProgID entry in Classes root
  CAveRegKey m_oProgId;
};
