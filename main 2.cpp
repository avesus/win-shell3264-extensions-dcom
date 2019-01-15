// XP min!
# define _WIN32_WINNT 0x0501

# define _WIN32_DCOM

// IPersistIDList only after this IE
# define _WIN32_IE _WIN32_IE_IE70

// SHParseDisplayName only after XP
# include <windows.h>

# include "./../avesus_commons/avedbg.h"
# include "./../avesus_commons/aveutils.h"

# include "./com/PureCOM.h"

# include <shlobj.h>
# include <shlwapi.h>

# pragma pack( push, 8 )

class CRefCounted :
	public IUnknown
{
public:

	CRefCounted() :
     m_nReferencesCount( 1 )
  {
    VERIFY( TRUE );
  }

  virtual ~CRefCounted()
  {
    VERIFY( ! m_nReferencesCount );
  }

  virtual ULONG __stdcall AddRef( void )
  {
    LONG nNewRefCount = ::InterlockedIncrement( & m_nReferencesCount );

    VERIFY( LONG_MAX != nNewRefCount );

    return (ULONG)nNewRefCount;
  }

  virtual ULONG __stdcall Release( void )
  {
    VERIFY( m_nReferencesCount );

    LONG nNewRefCount = ::InterlockedDecrement( & m_nReferencesCount );
    if( ! nNewRefCount )
    {
      delete this;
    }

    return (ULONG)nNewRefCount;
  }

private:

  volatile LONG m_nReferencesCount;
};

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

private:

	IUnknown* m_poContainer;
};

class CTest :
	public CRefCounted
{
public:

  class CIPersistFolderImpl :
		public CIfaceImpl< IPersistFolder2 >
	{
	public:

    CIPersistFolderImpl() :
      m_poIdList( 0 )
		{
		}

		virtual ~CIPersistFolderImpl()
		{
      if( m_poIdList )
      {
        //::free( m_poIdList );//
        ::CoTaskMemFree( m_poIdList );
        m_poIdList = 0;
      }
		}

		virtual HRESULT STDMETHODCALLTYPE GetClassID(
		  __RPC__out CLSID* o_poClsID )
		{
      if( ! VERIFY( o_poClsID ) )
      {
        return E_INVALIDARG;
      }

			//::MessageBoxW( 0, L"", L"IPersistFolder2::GetClassID", 0 );

      * o_poClsID = __uuidof( TestCOM );

			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE
		Initialize( __RPC__in PCIDLIST_ABSOLUTE i_poIdList )
		{
      if( ! VERIFY( i_poIdList ) )
      {
        return E_INVALIDARG;
      }

      // Calculate size of list.
      UINT32 nSize = 0;
      nSize = GetSize( i_poIdList );

      LPITEMIDLIST poMyIdList = (LPITEMIDLIST)
        //::malloc( nSize );
        ::CoTaskMemAlloc( nSize );
      if( ! VERIFY( poMyIdList ) )
      {
        return E_OUTOFMEMORY;
      }

      ::CopyMemory( poMyIdList, i_poIdList, nSize );

      if( m_poIdList )
      {
        //::free( m_poIdList );
        ::CoTaskMemFree( m_poIdList );
      }

      m_poIdList = poMyIdList;

			return NOERROR;
		}

    // IPersistFolder2
    virtual HRESULT STDMETHODCALLTYPE
    GetCurFolder( __RPC__deref_out_opt PIDLIST_ABSOLUTE* o_ppoIdList )
    {
      if( ! VERIFY( o_ppoIdList ) )
      {
        return E_INVALIDARG;
      }

      if( ! m_poIdList )
      {
        return E_FAIL;
      }

      // Calculate size of list.
      UINT32 nSize = 0;
      nSize = GetSize( m_poIdList );

      LPITEMIDLIST poMyIdList = (LPITEMIDLIST)::CoTaskMemAlloc( nSize );
      if( ! VERIFY( poMyIdList ) )
      {
        return E_OUTOFMEMORY;
      }

      ::CopyMemory( poMyIdList, m_poIdList, nSize );

      * o_ppoIdList = poMyIdList;

      //::MessageBoxW( 0, L"", L"IPersistFolder2::GetCurFolder", 0 );

      return NOERROR;
    }

    LPITEMIDLIST GetItemIDList( void )
    {
      return m_poIdList;
    }

    LPITEMIDLIST GetNextItemID( LPCITEMIDLIST pidl )
    { 
      // Check for valid pidl.
      if( ! pidl )
      {
        return 0;
      }

      // Get the size of the specified item identifier. 
      int cb = pidl->mkid.cb; 

      // If the size is zero, it is the end of the list. 
      if( ! cb )
      {
        return 0;
      }

      // Add cb to pidl (casting to increment by bytes). 
      pidl = (LPITEMIDLIST) (((LPBYTE) pidl) + cb); 

      // Return NULL if it is null-terminating, or a pidl otherwise. 
      return (pidl->mkid.cb == 0) ? NULL : (LPITEMIDLIST) pidl; 
    } 

  private:

    UINT GetSize( LPCITEMIDLIST pidl )
    {
      UINT cbTotal = 0;
      if( pidl )
      {
        cbTotal += sizeof(pidl->mkid.cb);    // Terminating null character
        while (pidl)
        {
          cbTotal += pidl->mkid.cb;
          pidl = GetNextItemID(pidl);
        }
      }

      return cbTotal;
    }

    LPITEMIDLIST m_poIdList;
	};


	class CIShellFolderImpl :
		public CIfaceImpl< IShellFolder2 >
	{
	public:
    CIShellFolderImpl()
		{
      VERIFY( m_oIEnumIDListImpl.SetContainer( this ) );
		}

    void SetCIPersistFolder( class CIPersistFolderImpl* i_poPersist )
    {
      m_oIEnumIDListImpl.SetCIPersistFolder( i_poPersist );
    }

		virtual ~CIShellFolderImpl()
		{
		}

    // IShellFolder INTERFACE

    class CIEnumIDListImpl :
		  public CIfaceImpl< IEnumIDList >
	  {
      CIPersistFolderImpl* m_poPersist;

	  public:
      CIEnumIDListImpl() :
        m_nCurrItem( 0 ),
        m_nCount( 1 ),
        m_poPersist( 0 )
		  {
		  }

		  virtual ~CIEnumIDListImpl()
		  {
		  }

      void SetCIPersistFolder( CIPersistFolderImpl* i_poPersist )
      {
        m_poPersist = i_poPersist;
      }

      virtual HRESULT STDMETHODCALLTYPE
      Next( __in  ULONG i_celt,
            __out_ecount_part(i_celt, * o_pceltFetched)
              PITEMID_CHILD* o_prgelt,
            __out_opt ULONG* o_pceltFetched )
      {
        if( o_pceltFetched )
        {
          * o_pceltFetched = 0;
        }

        if( ! i_celt )
        {
          return E_INVALIDARG;
        }

        if( ! o_prgelt || (! o_pceltFetched && i_celt != 1) )
        {
          return E_POINTER;
        }

        /*
        if( ! m_data )
        {
          return E_FAIL
        }
        */

        struct SMyItem
        {
          ITEMIDLIST m_oList;
        };

        SMyItem oItem = { 0 };
        oItem.m_oList.mkid.cb = sizeof( oItem );
        oItem.m_oList.mkid.abID[ 0 ] = 1;

        PITEMID_CHILD poItem = (PITEMID_CHILD)::CoTaskMemAlloc(
          sizeof( oItem ) + sizeof( ITEMIDLIST ) );
        if( ! VERIFY( poItem ) )
        {
          return E_OUTOFMEMORY;
        }

        ITEMIDLIST* pidlTemp = poItem + 1;
        pidlTemp->mkid.cb = 0;
        pidlTemp->mkid.abID[ 0 ] = 0;


        * ((SMyItem*) poItem) = oItem;
        * o_prgelt = poItem;

        if( o_pceltFetched )
        {
          * o_pceltFetched = 1;
        }

        /*
        CHAR aczReqCount[ 16 ] = { 0 };
        ::itoa( i_celt, aczReqCount, 10 ); 
        ::MessageBoxA( 0, aczReqCount, "IEnumIDList::Next", 0 );
        */

        if( ! m_nCurrItem )
        {
          m_nCurrItem = 1;
          return S_OK;
        }
        else
        {
          return S_FALSE;
        }
      }
      
      virtual HRESULT STDMETHODCALLTYPE
      Skip( ULONG i_celt )
      {
        ::MessageBoxW( 0, L"", L"IEnumIDList::Next", 0 );

        return E_NOTIMPL;
      }
      
      virtual HRESULT STDMETHODCALLTYPE
      Reset( void )
      {
        ::MessageBoxW( 0, L"", L"IEnumIDList::Next", 0 );

        return E_NOTIMPL;
      }
      
      virtual HRESULT STDMETHODCALLTYPE
      Clone( __RPC__deref_out_opt IEnumIDList** o_ppIEnumIDList )
      {
        ::MessageBoxW( 0, L"", L"IEnumIDList::Next", 0 );

        return E_NOTIMPL;
      }

      void ShowHidden( BOOL i_fnShow )
      {
        m_fnShowHidden = i_fnShow;
      }

    private:

      UINT32 m_nCurrItem;
      UINT32 m_nCount;

      BOOL m_fnShowHidden;
    };
	      
		virtual HRESULT STDMETHODCALLTYPE
		EnumObjects(
			__RPC__in_opt HWND i_hwnd,
			SHCONTF i_grfFlags,
			__RPC__deref_out_opt IEnumIDList** o_ppIEnumIDList )
		{
      if( ! VERIFY( o_ppIEnumIDList ) )
      {
        return E_INVALIDARG;
      }

      BOOL fnShowHidden
        = ( i_grfFlags & SHCONTF_INCLUDEHIDDEN ) ? TRUE : FALSE;

      if( i_grfFlags & SHCONTF_FOLDERS )
      {
      }

      if( i_grfFlags & SHCONTF_NONFOLDERS )
      {
      }

      //SHCONTF_FLATLIST	= 0x4000,
	    //SHCONTF_ENABLE_ASYNC	= 0x8000

      m_oIEnumIDListImpl.ShowHidden( fnShowHidden );

      * o_ppIEnumIDList
        = static_cast< IEnumIDList* >( & m_oIEnumIDListImpl );

      AddRef();

      /*
      CHAR aczText[ 16 ] = { 0 };
      ::itoa( i_grfFlags, aczText, 10 );

      ::MessageBoxA( 0, aczText, "IShellFolder::EnumObjects", 0 );
      */

      return S_OK;
		}
	      
		virtual HRESULT STDMETHODCALLTYPE
		BindToObject( 
			__RPC__in PCUIDLIST_RELATIVE i_poItemId,
			__RPC__in_opt IBindCtx* i_poIBindCtx,
			__RPC__in REFIID i_poReqShellFolderIface,
			__RPC__deref_out_opt void** o_ppIShellFolder )
		{
      /*if( i_poIBindCtx )
      {
        return E_FAIL;
      }*/

      return E_FAIL;

      if( o_ppIShellFolder )
      {
        * o_ppIShellFolder = 0;
/*
        CTest* poTest = new CTest();
		    if( ! VERIFY( poTest ) )
		    {
			    return E_OUTOFMEMORY;
		    }

		    HRESULT hRes = poTest->QueryInterface( i_poReqShellFolderIface,
          o_ppIShellFolder );
		    //VERIFY( S_OK == hRes );

		    poTest->Release();

        return hRes;
*/
        return QueryInterface( i_poReqShellFolderIface,
          o_ppIShellFolder );
        //* o_ppIShellFolder = 0;//static_cast< IShellFolder2* >( this );
      }

			//::MessageBoxW( 0, L"", L"IShellFolder::BindToObject", 0 );

			return E_NOINTERFACE;
		}

    virtual HRESULT STDMETHODCALLTYPE
		ParseDisplayName(
			__RPC__in_opt HWND hwnd,
			__RPC__in_opt IBindCtx *pbc,
			__RPC__in LPWSTR pszDisplayName,
			__reserved  ULONG *pchEaten,
			__RPC__deref_out_opt PIDLIST_RELATIVE *ppidl,
			__RPC__inout_opt ULONG *pdwAttributes )
		{
			::MessageBoxW( 0, L"", L"IShellFolder::ParseDisplayName", 0 );
			return E_NOTIMPL;
		}
	      
		virtual HRESULT STDMETHODCALLTYPE
		BindToStorage( 
				/* [in] */ __RPC__in PCUIDLIST_RELATIVE pidl,
				/* [unique][in] */ __RPC__in_opt IBindCtx *pbc,
				/* [in] */ __RPC__in REFIID riid,
				/* [iid_is][out] */ __RPC__deref_out_opt void **ppv)
		{
			::MessageBoxW( 0, L"", L"IShellFolder::BindToStorage", 0 );
			return E_NOTIMPL;
		}
	  
		virtual HRESULT STDMETHODCALLTYPE CompareIDs( 
				/* [in] */ LPARAM lParam,
				/* [in] */ __RPC__in PCUIDLIST_RELATIVE pidl1,
				/* [in] */ __RPC__in PCUIDLIST_RELATIVE pidl2)
		{
			::MessageBoxW( 0, L"", L"IShellFolder::CompareIDs", 0 );
			return E_NOTIMPL;
		}
	  
		virtual HRESULT STDMETHODCALLTYPE
    CreateViewObject( 
		  __RPC__in_opt HWND i_hwndOwner,
		  __RPC__in REFIID i_poReqIfaceId,
		  __RPC__deref_out_opt void** o_ppIShellView )
		{
      UNREFERENCED_PARAMETER( i_hwndOwner );

      if( ! VERIFY( o_ppIShellView ) )
      {
        return E_INVALIDARG;
      }

      * o_ppIShellView = 0;

      //::MessageBoxW( 0, L"", L"IShellFolder::CreateViewObject", 0 );

      if( ::IsEqualIID( i_poReqIfaceId, IID_IShellView ) )
		  {
        SFV_CREATE oCreate = { 0 };
        oCreate.cbSize = sizeof( oCreate );
        oCreate.pshf = static_cast< IShellFolder2* >( this );
        IShellView* poIShellView = 0;
        HRESULT hRes = ::SHCreateShellFolderView( & oCreate,
          & poIShellView );
        if( VERIFY( S_OK == hRes ) )
        {
          * o_ppIShellView = poIShellView;
          return S_OK;
        }
        else
        {
          return hRes;
        }
		  }

  		return E_NOINTERFACE;
		}
	  
		virtual HRESULT STDMETHODCALLTYPE
    GetAttributesOf( 
				UINT i_cidl,
				__RPC__in_ecount_full_opt( i_cidl )
          PCUITEMID_CHILD_ARRAY i_apidl,
				__RPC__inout SFGAOF* io_prgfInOut )
		{

      * io_prgfInOut = (* io_prgfInOut) & (
        SFGAO_BROWSABLE
        //SFGAO_CANCOPY | SFGAO_CANDELETE
        //| SFGAO_CANMOVE | SFGAO_CANRENAME | SFGAO_COMPRESSED
        //| SFGAO_DROPTARGET
        | SFGAO_FOLDER
        | SFGAO_STREAM
        //| SFGAO_HASPROPSHEET
        //| SFGAO_HASSUBFOLDER
        //| SFGAO_HIDDEN
        | SFGAO_ISSLOW
        );
      
			//::MessageBoxW( 0, L"", L"IShellFolder::GetAttributesOf", 0 );

			return S_OK;
		}
	  
		virtual HRESULT STDMETHODCALLTYPE
    GetUIObjectOf(
		  __RPC__in_opt HWND i_hwndOwner,
		  UINT i_cidl,
			__RPC__in_ecount_full_opt(i_cidl)
        PCUITEMID_CHILD_ARRAY apidl,
			__RPC__in REFIID i_riid,
			
			__reserved UINT* io_rgfReserved,
			__RPC__deref_out_opt void** o_ppv )
		{
      if( o_ppv )
      {
        * o_ppv = 0;

        return QueryInterface( i_riid, o_ppv );
      }
      /*
      Interface Identifier Allowed cidl Value 
      IContextMenu         The cidl parameter can be greater than or equal to one. 
      IContextMenu2        The cidl parameter can be greater than or equal to one. 
      IDataObject          The cidl parameter can be greater than or equal to one. 
      IDropTarget          The cidl parameter can only be one. 
      IExtractIcon         The cidl parameter can only be one. 
      IQueryInfo           The cidl parameter can only be one. 

      */

			// ::MessageBoxW( 0, L"", L"IShellFolder::GetUIObjectOf", 0 );

			return E_NOINTERFACE;
		}
	  
		virtual HRESULT STDMETHODCALLTYPE
    GetDisplayNameOf( 
				/* [in] */ __RPC__in PCUITEMID_CHILD pidl,
				/* [in] */ SHGDNF uFlags,
				/* [out] */ __RPC__out STRRET* o_poName )
		{
      if( ! VERIFY( o_poName ) )
      {
        return E_INVALIDARG;
      }

      struct SMyItem
      {
        ITEMIDLIST m_oList;
        BYTE m_bFinalZeroes[ 2 ];
      };

      SMyItem oItem = { 0 };
      oItem.m_oList.mkid.cb = sizeof( oItem );
      oItem.m_oList.mkid.abID[ 0 ] = 1;

      if( 0 == ::memcmp( pidl, & oItem, sizeof( oItem ) ) )
      {
        o_poName->uType = STRRET_CSTR;
        ::strcpy( o_poName->cStr, "Test Item" );

        /*CHAR aczFlags[ 16 ] = { 0 };
        ::itoa( uFlags, aczFlags, 10 );
        ::MessageBoxA( 0, aczFlags, "IShellFolder::GetDisplayNameOf", 0 );*/
      }
      else
      {
        //::MessageBoxW( 0, L"", L"IShellFolder::GetDisplayNameOf", 0 );
        return E_FAIL;
      }

			return S_OK;
		}
	  
		virtual HRESULT STDMETHODCALLTYPE SetNameOf( 
				/* [unique][in] */ __RPC__in_opt HWND hwnd,
				/* [in] */ __RPC__in PCUITEMID_CHILD pidl,
				/* [string][in] */ __RPC__in LPCWSTR pszName,
				/* [in] */ SHGDNF uFlags,
				/* [out] */ __RPC__deref_out_opt PITEMID_CHILD *ppidlOut)
		{
			::MessageBoxW( 0, L"", L"IShellFolder::SetNameOf", 0 );
			return E_NOTIMPL;
		}

    // IShellFolder2 INTERFACE

    virtual HRESULT STDMETHODCALLTYPE GetDefaultSearchGUID( 
        /* [out] */ __RPC__out GUID *pguid)
    {
			::MessageBoxW( 0, L"", L"IShellFolder2::GetDefaultSearchGUID", 0 );

			return E_NOTIMPL;
		}
    
    virtual HRESULT STDMETHODCALLTYPE EnumSearches( 
        /* [out] */ __RPC__deref_out_opt IEnumExtraSearch** ppenum)
    {
      //////////////!!!!!!!!!

      * ppenum = 0;

			::MessageBoxW( 0, L"", L"IShellFolder2::EnumSearches", 0 );
			return E_NOTIMPL;
		}
    
    virtual HRESULT STDMETHODCALLTYPE
    GetDefaultColumn( DWORD dwRes,
                      __RPC__out ULONG* o_pnSort,
                      __RPC__out ULONG* o_pnDisplay )
    {
      UNREFERENCED_PARAMETER( dwRes );

      if( ! VERIFY( o_pnSort && o_pnDisplay ) )
      {
        return E_INVALIDARG;
      }

      * o_pnSort = 0;
      * o_pnDisplay = 0;

			//::MessageBoxW( 0, L"", L"IShellFolder2::GetDefaultColumn", 0 );
			return S_OK;
		}
    
    virtual HRESULT STDMETHODCALLTYPE
    GetDefaultColumnState( UINT i_iColumn,
                           __RPC__out SHCOLSTATEF* o_pcsFlags )
    {
      if( ! VERIFY( o_pcsFlags ) )
      {
        return E_INVALIDARG;
      }

      switch( i_iColumn )
      {
        break; case 0:
          * o_pcsFlags = SHCOLSTATE_TYPE_STR
            | SHCOLSTATE_ONBYDEFAULT | SHCOLSTATE_SLOW
            | SHCOLSTATE_PREFER_VARCMP;
        break; default:
          return E_FAIL;
      }

			//::MessageBoxW( 0, L"", L"IShellFolder2::GetDefaultColumnState", 0 );

			return S_OK;
		}
    
    virtual HRESULT STDMETHODCALLTYPE
    GetDetailsEx(
      __RPC__in PCUITEMID_CHILD i_pidl,
      __RPC__in const SHCOLUMNID* i_pscid,
      __RPC__out VARIANT* o_pv )
    {

      struct SMyItem
      {
        ITEMIDLIST m_oList;
        BYTE m_bFinalZeroes[ 2 ];
      };

      SMyItem oItem = { 0 };
      oItem.m_oList.mkid.cb = sizeof( oItem );
      oItem.m_oList.mkid.abID[ 0 ] = 1;

      if( 0 == ::memcmp( i_pidl, & oItem, sizeof( oItem ) ) )
      {
        if( 10 == i_pscid->pid )
        {
          //{B725F130-47EF-101A-A5F1-02608C9EEBAC}

          VARIANTARG oArg = { 0 };
          ::VariantInit( & oArg );

          oArg.vt = VT_BSTR;
          oArg.bstrVal = ::SysAllocString( L"Test Item" );

          ::VariantCopy( & oArg, o_pv );
          

          //o_pv->vt = VT_BSTR;
          //o_pv->bstrVal = ::SysAllocString( L"Test Item" );

          
          //o_pv- = L"Test Item";
        }
        else
        {
          return E_FAIL;
        }
      }
      else
      {
        return E_FAIL;
      }

			//::MessageBoxW( 0, L"", L"IShellFolder2::GetDetailsEx", 0 );

			return S_OK;
		}
    
    virtual HRESULT STDMETHODCALLTYPE
    GetDetailsOf( __RPC__in_opt PCUITEMID_CHILD i_poItemIdList,
                  UINT i_nColumn,
                  __RPC__out SHELLDETAILS* o_poShellDetails )
    {
      if( ! VERIFY( o_poShellDetails ) )
      {
        return E_INVALIDARG;
      }

      if( ! i_poItemIdList )
      {
        // the title of the information field
        // specified by nColumn is returned.
      }

      switch( i_nColumn )
      {
        // Name
        break; case 0:
          o_poShellDetails->fmt = LVCFMT_LEFT;
          o_poShellDetails->cxChar = 30;
          o_poShellDetails->str.uType = STRRET_CSTR;
          ::strcpy( o_poShellDetails->str.cStr, "Name" );

          return S_OK;

        // Size
        break; case 1:
          return E_FAIL;

        // Type
        break; case 2:
          return E_FAIL;

        // Date modified
        break; case 3:
          return E_FAIL;

        break; default:
          return E_FAIL;
      }

      /*
      WCHAR awzPath[ MAX_PATH ] = { 0 };
      if( i_poItemIdList )
      {
        ENSURE( ::SHGetPathFromIDListW( i_poItemIdList, awzPath ) );
      }

      WCHAR awzText[ MAX_PATH * 2 ] = { 0 };
      ::swprintf_s( awzText, ALEN( awzText ), L"ID: %d\nPath: %s",
        i_nColumn, awzPath );

			::MessageBoxW( 0, awzText, L"IShellFolder2::GetDetailsOf", 0 );
      */
		}
    
    virtual HRESULT STDMETHODCALLTYPE
    MapColumnToSCID( UINT iColumn,
                     __RPC__out SHCOLUMNID* o_pscid )
    {
      if( ! VERIFY( o_pscid ) )
      {
        return E_INVALIDARG;
      }

      GUID oColumnType = { 0xB725F130, 0x47EF, 0x101A,
        { 0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC } };

      o_pscid->fmtid = oColumnType;
      o_pscid->pid = 10;//PID_STG_NAME;

			// ::MessageBoxW( 0, L"", L"IShellFolder2::MapColumnToSCID", 0 );
			return S_OK;
		}

  private:

    CIEnumIDListImpl m_oIEnumIDListImpl;
	};

	class CITestImpl :
		public CIfaceImpl< ITest >
	{
	public:

		CITestImpl()
		{
		}

		virtual ~CITestImpl()
		{
		}

		virtual HRESULT STDMETHODCALLTYPE
		Test( long test )
		{
			return S_OK;
		}
	};

  class CIDataObjectImpl :
		public CIfaceImpl< IDataObject >
	{
	public:

    CIDataObjectImpl()
		{
		}

		virtual ~CIDataObjectImpl()
		{
		}

    virtual /* [local] */ HRESULT STDMETHODCALLTYPE
    GetData( 
        /* [unique][in] */ FORMATETC *pformatetcIn,
        /* [out] */ STGMEDIUM *pmedium)
    {
      ::MessageBoxW( 0, L"", L"IDataObject::GetData", 0 );
      return E_NOTIMPL;
    }
    
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetDataHere( 
        /* [unique][in] */ FORMATETC *pformatetc,
        /* [out][in] */ STGMEDIUM *pmedium)
    {
      ::MessageBoxW( 0, L"", L"IDataObject::GetDataHere", 0 );
      return E_NOTIMPL;
    }
    
    virtual HRESULT STDMETHODCALLTYPE QueryGetData( 
        /* [unique][in] */ __RPC__in_opt FORMATETC *pformatetc)
    {
      ::MessageBoxW( 0, L"", L"IDataObject::QueryGetData", 0 );
      return E_NOTIMPL;
    }
    
    virtual HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc( 
        /* [unique][in] */ __RPC__in_opt FORMATETC *pformatectIn,
        /* [out] */ __RPC__out FORMATETC *pformatetcOut)
    {
      ::MessageBoxW( 0, L"", L"IDataObject::GetCanonicalFormatEtc", 0 );
      return E_NOTIMPL;
    }
    
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE SetData( 
        /* [unique][in] */ FORMATETC *pformatetc,
        /* [unique][in] */ STGMEDIUM *pmedium,
        /* [in] */ BOOL fRelease)
    {
      ::MessageBoxW( 0, L"", L"IDataObject::SetData", 0 );
      return E_NOTIMPL;
    }
    
    virtual HRESULT STDMETHODCALLTYPE EnumFormatEtc( 
        /* [in] */ DWORD dwDirection,
        /* [out] */ __RPC__deref_out_opt IEnumFORMATETC **ppenumFormatEtc)
    {
      ::MessageBoxW( 0, L"", L"IDataObject::EnumFormatEtc", 0 );
      return E_NOTIMPL;
    }
    
    virtual HRESULT STDMETHODCALLTYPE DAdvise( 
        /* [in] */ __RPC__in FORMATETC *pformatetc,
        /* [in] */ DWORD advf,
        /* [unique][in] */ __RPC__in_opt IAdviseSink *pAdvSink,
        /* [out] */ __RPC__out DWORD *pdwConnection)
    {
      ::MessageBoxW( 0, L"", L"IDataObject::DAdvise", 0 );
      return E_NOTIMPL;
    }
    
    virtual HRESULT STDMETHODCALLTYPE DUnadvise( 
        /* [in] */ DWORD dwConnection)
    {
      ::MessageBoxW( 0, L"", L"IDataObject::DUnadvise", 0 );
      return E_NOTIMPL;
    }
    
    virtual HRESULT STDMETHODCALLTYPE EnumDAdvise( 
        /* [out] */ __RPC__deref_out_opt IEnumSTATDATA **ppenumAdvise)
    {
      ::MessageBoxW( 0, L"", L"IDataObject::EnumDAdvise", 0 );
      return E_NOTIMPL;
    }


  };

  class CIShellIconImpl :
    public CIfaceImpl< IShellIcon >
	{
	public:

    CIShellIconImpl()
		{
		}

		virtual ~CIShellIconImpl()
		{
		}

    virtual HRESULT STDMETHODCALLTYPE
    GetIconOf( __RPC__in PCUITEMID_CHILD i_poItemIdList,
               UINT i_nFlags,
               __RPC__out int* o_pnIconIndex )
    {
      if( ! VERIFY( i_poItemIdList ) )
      {
        return E_INVALIDARG;
      }

      * o_pnIconIndex = 0;

/*      if( GIL_OPENICON & i_nFlags )
      {
        
      }
      else
      {
        * o_pnIconIndex = 3;
      }
*/
      return S_OK;
    }
  };

  class CIPersistIDListImpl :
		public CIfaceImpl< IPersistIDList >
	{
	public:

    CIPersistIDListImpl()
		{
		}

		virtual ~CIPersistIDListImpl()
		{
		}

    // IPersist interface
		virtual HRESULT STDMETHODCALLTYPE
    GetClassID( __RPC__out CLSID* o_poClsID )
		{
      if( ! VERIFY( o_poClsID ) )
      {
        return E_INVALIDARG;
      }

			//::MessageBoxW( 0, L"", L"IPersistIDList::GetClassID", 0 );

      * o_poClsID = __uuidof( TestCOM );

			return S_OK;
		}

    // IPersistIDList interface

    virtual HRESULT STDMETHODCALLTYPE
    SetIDList( __RPC__in PCIDLIST_ABSOLUTE pidl )
    {
      ::MessageBoxW( 0, L"", L"IPersistIDList::SetIDList", 0 );
      return E_NOTIMPL;
    }
        
    virtual HRESULT STDMETHODCALLTYPE
    GetIDList( __RPC__deref_out_opt PIDLIST_ABSOLUTE *ppidl )
    {
      ::MessageBoxW( 0, L"", L"IPersistIDList::GetIDList", 0 );
      return E_NOTIMPL;
    }
	};

	virtual HRESULT STDMETHODCALLTYPE
	QueryInterface(
		__in  const IID &  i_poReqIfaceId,
		__out       void** o_ppoIface )
	{
		if( ! VERIFY( o_ppoIface ) )
		{
			return E_INVALIDARG;
		}

		* o_ppoIface = 0;

		if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IPersistFolder ) ) )
		{
			* o_ppoIface = static_cast< IPersistFolder* >( & m_oIPersistFolder );

			AddRef();
		}
    else if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IPersistFolder2 ) ) )
		{
			* o_ppoIface = static_cast< IPersistFolder2* >( & m_oIPersistFolder );

			AddRef();
		}
    else if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IPersist ) ) )
    {
      * o_ppoIface = static_cast< IPersist* >( & m_oIPersistFolder );

			AddRef();
    }
    else if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IShellIcon ) ) )
		{
			* o_ppoIface = static_cast< IShellIcon* >( & m_oIShellIcon );

			AddRef();
		}
# if 0
    else if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IPersistIDList ) ) )
		{
			* o_ppoIface = static_cast< IPersistIDList* >( & m_oIPersistIdList );

			AddRef();
		}
# endif
    else if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IDataObject ) ) )
		{
			* o_ppoIface = static_cast< IDataObject* >( & m_oIDataObject );

			AddRef();
		}
		else if( ::IsEqualIID( i_poReqIfaceId, __uuidof( ITest ) ) )
		{
			* o_ppoIface = static_cast< ITest* >( & m_oITest );

			AddRef();
		}
		else if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IShellFolder ) ) )
		{
			* o_ppoIface = static_cast< IShellFolder* >( & m_oIShellFolder );

			AddRef();
		}
    else if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IShellFolder2 ) ) )
		{
			* o_ppoIface = static_cast< IShellFolder2* >( & m_oIShellFolder );

			AddRef();
		}
		else if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IUnknown ) ) )
		{
			* o_ppoIface = static_cast< IUnknown* >( this );

			AddRef();
		}
		else
		{
			IID oBogus = { 0x9B45E435, 0x34A9, 0x4E6B,
				{ 0xA2, 0xA1, 0xB0, 0xEC, 0xD2, 0x84, 0x96, 0x7C } };

			IID oBogus2 = { 0x8CD9494C, 0xAC44, 0x44D0,
			  { 0x8F, 0xFD, 0xD4, 0xCF, 0x86, 0x72, 0xB0, 0xA0 } };

      IID oBogus4 = { 0x053B4A86, 0x0DC9, 0x40A3,
        { 0xB7, 0xED, 0xBC, 0x6A, 0x2E, 0x95, 0x1F, 0x48 } };

      IID oBogus5 = { 0x7361EE29, 0x5BAD, 0x459D,
        { 0xA9, 0xF5, 0xF6, 0x55, 0x06, 0x89, 0x82, 0xF0 } };

      IID oBogus8 = { 0x924502A7, 0xCC8E, 0x4F60,
        { 0xAE, 0x1F, 0xF7, 0x0C, 0x0A, 0x2B, 0x7A, 0x7C } };

      IID oBogus9 = { 0xB09C4B99, 0xA882, 0x4F63,
        { 0xBB, 0x41, 0xAC, 0x4C, 0x5D, 0xF8, 0x02, 0xC5 } };

      IID oIShellFolder3 = { 0x2EC06C64, 0x1296, 0x4F53,
        { 0x89, 0xE5, 0xEC, 0xCE, 0x4E, 0xFC, 0x21, 0x89 } };

      IID oIInfoPaneProvider = { 0x38698B65, 0x1CA7, 0x458C,
        { 0xB4, 0xD6, 0xE0, 0xA5, 0x13, 0x79, 0xC1, 0xD2 } };

			if( ! ::IsEqualIID( i_poReqIfaceId, oBogus )
			    &&
			  ! ::IsEqualIID( i_poReqIfaceId, oBogus2 )
			    &&
			  ! ::IsEqualIID( i_poReqIfaceId, __uuidof( IBrowserFrameOptions ) )
          &&
        ! ::IsEqualIID( i_poReqIfaceId, oBogus4 )
			    &&
			  ! ::IsEqualIID( i_poReqIfaceId, oBogus5 )
			    &&
        ! ::IsEqualIID( i_poReqIfaceId, __uuidof( IShellIconOverlay ) )
			    &&
// After Vista only
//        ! ::IsEqualIID( i_poReqIfaceId, __uuidof( IExplorerPaneVisibility ) )
//          &&
        ! ::IsEqualIID( i_poReqIfaceId, oBogus8 )
          &&
        ! ::IsEqualIID( i_poReqIfaceId, oBogus9 )
          &&
        ! ::IsEqualIID( i_poReqIfaceId, __uuidof( IContextMenu ) )
          &&
        ! ::IsEqualIID( i_poReqIfaceId, __uuidof( IQueryAssociations ) )
          &&
			  ! ::IsEqualIID( i_poReqIfaceId, __uuidof( IObjectWithSite ) )
			    &&
        ! ::IsEqualIID( i_poReqIfaceId, __uuidof( IInternetSecurityManager ) )
			    &&
			  ! ::IsEqualIID( i_poReqIfaceId, __uuidof( IPersistFolder3 ) )
          &&
			  ! ::IsEqualIID( i_poReqIfaceId, oIShellFolder3 )
          &&
			  ! ::IsEqualIID( i_poReqIfaceId, oIInfoPaneProvider )
          &&
			  ! ::IsEqualIID( i_poReqIfaceId, __uuidof( IPersistIDList ) )
          &&
			  ! ::IsEqualIID( i_poReqIfaceId, __uuidof( IOleCommandTarget ) )
          &&
			  ! ::IsEqualIID( i_poReqIfaceId, __uuidof( IObjectProvider ) )
          &&
        // Requested by BindToObject method
			  ! ::IsEqualIID( i_poReqIfaceId, __uuidof( IPropertyStoreFactory ) )
          &&
			  ! ::IsEqualIID( i_poReqIfaceId, __uuidof( IPropertyStore ) )
          &&
			  ! ::IsEqualIID( i_poReqIfaceId, __uuidof( IPropertyStoreCache ) )
          &&
			  ! ::IsEqualIID( i_poReqIfaceId, __uuidof( IDropTarget ) )
          &&
			  ! ::IsEqualIID( i_poReqIfaceId, __uuidof( IShellLinkW ) )        
          &&
			  ! ::IsEqualIID( i_poReqIfaceId, __uuidof( IExtractImage ) )
          &&
			  ! ::IsEqualIID( i_poReqIfaceId, __uuidof( IThumbnailHandlerFactory ) )
      )
			{
				CRegistryGuid oGuid;
				ENSURE( oGuid.Build( i_poReqIfaceId ) );
				::MessageBoxW( 0, oGuid.Get(), L"CTest::QueryInterface", 0 );
			}

			return E_NOINTERFACE;
		}

		return S_OK;
	}

public:

  CTest()
	{
		VERIFY( m_oIShellFolder.SetContainer( this ) );
		VERIFY( m_oITest.SetContainer( this ) );
		VERIFY( m_oIPersistFolder.SetContainer( this ) );
    VERIFY( m_oIPersistIdList.SetContainer( this ) );
    VERIFY( m_oIDataObject.SetContainer( this ) );
    VERIFY( m_oIShellIcon.SetContainer( this ) );

    m_oIShellFolder.SetCIPersistFolder( & m_oIPersistFolder );
	}

private:

	CIShellFolderImpl   m_oIShellFolder;
	CITestImpl          m_oITest;
	CIPersistFolderImpl m_oIPersistFolder;
  CIPersistIDListImpl m_oIPersistIdList;
  CIDataObjectImpl    m_oIDataObject;
  CIShellIconImpl     m_oIShellIcon;
};

class
CTestClassFactory :
  public IClassFactory
{
protected:

	virtual ~CTestClassFactory()
	{
		VERIFY( TRUE );
	}

public:

	CTestClassFactory() :
		m_nReferencesCount( 1 )
	{
	}

  virtual ULONG __stdcall AddRef( void )
  {
    LONG nNewRefCount = ::InterlockedIncrement( & m_nReferencesCount );

    VERIFY( LONG_MAX != nNewRefCount );

    return (ULONG)nNewRefCount;
  }

  virtual ULONG __stdcall Release( void )
  {
    VERIFY( m_nReferencesCount );

    LONG nNewRefCount = ::InterlockedDecrement( & m_nReferencesCount );
    if( ! nNewRefCount )
    {
      delete this;
    }

    return (ULONG)nNewRefCount;
  }

	virtual HRESULT STDMETHODCALLTYPE
	QueryInterface(
	  __in  const IID &  i_poReqIfaceId,
	  __out       void** o_ppoIface )
	{
		if( ! VERIFY( o_ppoIface ) )
		{
			return E_INVALIDARG;
		}

		* o_ppoIface = 0;

		if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IClassFactory ) ) )
		{
			* o_ppoIface = static_cast< IClassFactory* >( this );

			AddRef();
		}
		else if( ::IsEqualIID( i_poReqIfaceId, __uuidof( IUnknown ) ) )
		{
			* o_ppoIface = static_cast< IUnknown* >( this );

			AddRef();
		}
		else
		{
			return E_NOINTERFACE;
		}

		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE
	CreateInstance(
	  __in        IUnknown* i_poIfaceOuter,
    __in  const IID &     i_poReqIfaceId,
    __out       void**    o_ppoIface )
	{

		if( ! VERIFY( o_ppoIface ) )
		{
			return E_INVALIDARG;
		}

		if( i_poIfaceOuter )
		{
			return CLASS_E_NOAGGREGATION;
		}

		* o_ppoIface = 0;

		CTest* poTest = new CTest();
		if( ! VERIFY( poTest ) )
		{
			return E_OUTOFMEMORY;
		}

		HRESULT hRes = poTest->QueryInterface( i_poReqIfaceId, o_ppoIface );
		//VERIFY( S_OK == hRes );

		poTest->Release();

		return hRes;
	}
        
  virtual HRESULT STDMETHODCALLTYPE
	LockServer( BOOL i_fnLock )
	{
		return S_OK;
	}

private:

	volatile LONG m_nReferencesCount;
};

// TODO: реализовать классы для поддержки
// тех же интерфейсов, что и в ATL.
// Самый простейший случай!!!
// Всё должно работать.

# pragma pack( pop )

extern "C"
BOOL WINAPI
DllMain( HINSTANCE i_hInst, DWORD i_nReason, void* i_pReserved )
{
  return TRUE;
}

// Used to determine whether the DLL can be unloaded by OLE
STDAPI
DllCanUnloadNow( void )
{
  return S_OK;
}

// Returns a pointer to the IClassFactory
// of the requested type 'i_poClsid'.
// 'i_poClsid' should be CLSID of dll.
// Using IClassFactory, the client then will get
// a pointer to any interface, supported by this dll.\
// Real constuction of the object and determining its
// real type can be at that phase (by IID).
STDAPI
DllGetClassObject(
  // Which class of the objects to create?
  // Only one object class presented by this dll.
  __in  const CLSID & i_poReqClsid,
  // IID_IClassFactory
  __in  const IID &   i_poIClassFactoryId,
  __out       void**  o_ppoIClassFactory )
{
	if( ! VERIFY( o_ppoIClassFactory ) )
	{
		return E_INVALIDARG;
	}

	* o_ppoIClassFactory = 0;

	if( ::IsEqualCLSID( i_poReqClsid, __uuidof( TestCOM ) ) )
	{
		//::MessageBoxW( 0, L"DllGetClassObject", L"Entered", 0 );

		CTestClassFactory* poTestClassFactory = new CTestClassFactory();
		if( ! VERIFY( poTestClassFactory ) )
		{
			return E_OUTOFMEMORY;
		}

		HRESULT hRes = poTestClassFactory->QueryInterface(
		  i_poIClassFactoryId, o_ppoIClassFactory );
		VERIFY( S_OK == hRes );

		poTestClassFactory->Release();

		return hRes;
	}
	else
	{
		return CLASS_E_CLASSNOTAVAILABLE;
	}
}

# if 0

// DllRegisterServer - Adds entries to the system registry
STDAPI
DllRegisterServer( void )
{

  return S_OK;
}

// DllUnregisterServer - Removes entries from the system registry
STDAPI
DllUnregisterServer( void )
{
	return S_OK;
}

# endif