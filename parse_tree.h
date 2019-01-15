# pragma once

template< typename TChar >
class CParseTree
{
public:

  struct SParsedFolderPath
  {
    UINT32  m_nStrLen;
    const TChar* m_psFolderName;
    SParsedFolderPath* m_poNext;

    SParsedFolderPath() :
      m_nStrLen( 0 ),
      m_psFolderName( 0 ),
      m_poNext( 0 )
    {
    }

    ~SParsedFolderPath()
    {
      m_nStrLen = 0;
      m_psFolderName = 0;

      if( m_poNext )
      {
        delete m_poNext;
        m_poNext = 0;
      }
    }
  };

  struct SFileItem
  {
    UINT32  m_nStrLen;
    const TChar* m_psFileName;
    SFileItem* m_poNext;

    UINT64 m_nFileSize;
    UINT64 m_nCompressedFileSize;

    BOOL m_fnPwdProtected;

    SFileItem( const TChar* i_psFileName, UINT32 i_nStrLen,
               UINT64 i_nFileSize,
               UINT64 i_nCompressedFileSize ) :
      m_fnPwdProtected( FALSE ),
      m_psFileName( i_psFileName ),
      m_nStrLen( i_nStrLen ),
      m_nFileSize( i_nFileSize ),
      m_nCompressedFileSize( i_nCompressedFileSize ),
      m_poNext( 0 )
    {
    }

    ~SFileItem()
    {
      if( m_poNext )
      {
        delete m_poNext;
        m_poNext = 0;
      }
    }
  };

  struct SFolderItem
  {
    UINT32  m_nStrLen;
    const TChar* m_psFolderName;
    SFolderItem* m_poNext;

    SFolderItem* m_poFirstChildFolder;
    SFolderItem* m_poLastChildFolder;

    SFileItem* m_poFirstChildFile;
    SFileItem* m_poLastChildFile;

    UINT32 m_nFolders;
    UINT32 m_nFiles;

    SFolderItem( const TChar* i_psFolderName, UINT32 i_nStrLen ) :
      m_psFolderName( i_psFolderName ),
      m_nStrLen( i_nStrLen ),
      m_poNext( 0 ),
      m_poFirstChildFolder( 0 ),
      m_poLastChildFolder( 0 ),
      m_poFirstChildFile( 0 ),
      m_poLastChildFile( 0 ),
      m_nFolders( 0 ),
      m_nFiles( 0 )
    {
    }

    ~SFolderItem()
    {
      if( m_poFirstChildFile )
      {
        delete m_poFirstChildFile;
        m_poFirstChildFile = 0;
        m_poLastChildFile = 0;
        m_nFiles = 0;
      }

      if( m_poFirstChildFolder )
      {
        delete m_poFirstChildFolder;
        m_poFirstChildFolder = 0;
        m_poLastChildFolder = 0;
        m_nFolders = 0;
      }

      if( m_poNext )
      {
        delete m_poNext;
        m_poNext = 0;
      }
    }
  };

  // Add file with its full path specified
  // by SParsedFolderPath linked list.
  // Client may allocate the path list only once,
  // and renew records in it and add additional
  // items in the process of total parsing.
  // If i_poPath is 0, then file would added into the
  // root folder.
  BOOL AddFile( SParsedFolderPath* i_poPath,
    const TChar* i_psFileName, UINT32 i_nStrLen,
    UINT64 i_nFileSize, UINT64 i_nCompressedSize )
  {
    ENSURE( i_psFileName && i_nStrLen );

    SFileItem* poNewItem = new SFileItem( i_psFileName,
      i_nStrLen, i_nFileSize, i_nCompressedSize );
    ENSURE( poNewItem );

    SFolderItem* poFolderForAddTo = 0;
    ENSURE( FindFolder( i_poPath, & poFolderForAddTo ) );

    if( ! poFolderForAddTo )
    {
      ENSURE( AddFolder( i_poPath, & poFolderForAddTo ) );
      ENSURE( poFolderForAddTo );
    }

    SFileItem* poLastFile = poFolderForAddTo->m_poLastChildFile;
    if( poLastFile )
    {
      poLastFile->m_poNext = poNewItem;
    }
    else
    {
      poFolderForAddTo->m_poFirstChildFile = poNewItem;
    }

    poFolderForAddTo->m_poLastChildFile = poNewItem;

    ++ poFolderForAddTo->m_nFiles;

    return TRUE;
  }

  BOOL ListFolders( 
    __in_opt  SParsedFolderPath* i_poPath,
    __out     SFolderItem** o_ppoFoldersList,
    __out     UINT32* o_pnFolders,
    __out_opt BOOL* o_pfnIsPathExists )
  {
    ENSURE( o_ppoFoldersList );

    * o_ppoFoldersList = 0;

    * o_pnFolders = 0;

    if( o_pfnIsPathExists )
    {
      * o_pfnIsPathExists = FALSE;
    }

    SFolderItem* poParent = 0;

    ENSURE( FindFolder( i_poPath, & poParent ) );
    if( ! poParent )
    {
      // invalid path
      return TRUE;
    }

    * o_ppoFoldersList = poParent->m_poFirstChildFolder;
    * o_pnFolders = poParent->m_nFolders;

    * o_pfnIsPathExists = TRUE;

    return TRUE;
  }

  BOOL ListFiles(
    __in_opt  SParsedFolderPath* i_poPath,
    __out     SFileItem** o_ppoFileList,
    __out     UINT32* o_pnFiles,
    __out_opt BOOL* o_pfnIsPathExists )
  {
    ENSURE( o_ppoFileList );

    * o_ppoFileList = 0;
    * o_pnFiles = 0;
    if( o_pfnIsPathExists )
    {
      * o_pfnIsPathExists = FALSE;
    }

    SFolderItem* poParent = 0;

    ENSURE( FindFolder( i_poPath, & poParent ) );
    if( ! poParent )
    {
      // invalid path
      return TRUE;
    }

    * o_pnFiles = poParent->m_nFiles;

    * o_ppoFileList = poParent->m_poFirstChildFile;

    if( o_pfnIsPathExists )
    {
      * o_pfnIsPathExists = TRUE;
    }

    return TRUE;
  }

  CParseTree() :
    m_oRootFolder( 0, 0 )
  {
  }

  virtual ~CParseTree()
  {
  }

private:

  BOOL FindFolder( SParsedFolderPath* i_poPath,
    SFolderItem** o_ppoFound )
  {
    ENSURE( o_ppoFound );

    * o_ppoFound = 0;

    if( i_poPath
        &&
        i_poPath->m_nStrLen
        &&
        i_poPath->m_psFolderName )
    {
      SParsedFolderPath* poCurFolderName = i_poPath;
      SFolderItem* poCurFolder = & m_oRootFolder;

      do
      {
        SFolderItem* poFoundFolder = 0;
        ENSURE( FindChildFolder( poCurFolder,
          poCurFolderName->m_psFolderName,
          poCurFolderName->m_nStrLen,
          & poFoundFolder ) );

        if( ! poFoundFolder )
        {
          // the path not exists!
          return TRUE;
        }

        poCurFolder = poFoundFolder;
        poCurFolderName = poCurFolderName->m_poNext;
      }
      while( poCurFolderName
             &&
             poCurFolderName->m_nStrLen
             &&
             poCurFolderName->m_psFolderName );

      * o_ppoFound = poCurFolder;
      return TRUE;
    }
    else
    {
      * o_ppoFound = & m_oRootFolder;
    }

    return TRUE;
  }

  BOOL AddFolder( SParsedFolderPath* i_poPath,
    SFolderItem** o_ppoCreated )
  {
    ENSURE( i_poPath && o_ppoCreated );

    * o_ppoCreated = 0;

    SParsedFolderPath* poCurFolderName = i_poPath;
    SFolderItem* poCurFolder = & m_oRootFolder;

    do
    {
      // break if empty folder path (as terminator)
      if( ! poCurFolderName->m_psFolderName
          ||
          ! poCurFolderName->m_nStrLen )
      {
        break;
      }

      SFolderItem* poFoundFolder = 0;
      ENSURE( FindChildFolder( poCurFolder,
        poCurFolderName->m_psFolderName,
        poCurFolderName->m_nStrLen,
        & poFoundFolder ) );

      if( ! poFoundFolder )
      {
        // the path not exists, so create a subfolder:

        poFoundFolder = new SFolderItem(
          poCurFolderName->m_psFolderName,
          poCurFolderName->m_nStrLen );
        ENSURE( poFoundFolder );

        if( poCurFolder->m_poLastChildFolder )
        {
          poCurFolder->m_poLastChildFolder->m_poNext = poFoundFolder;
        }
        else
        {
          poCurFolder->m_poFirstChildFolder = poFoundFolder;
        }

        poCurFolder->m_poLastChildFolder = poFoundFolder;

        ++ poCurFolder->m_nFolders;
      }

      poCurFolder = poFoundFolder;
      poCurFolderName = poCurFolderName->m_poNext;
    }
    while( poCurFolderName );

    * o_ppoCreated = poCurFolder;

    return TRUE;
  }

  BOOL FindChildFolder( SFolderItem* i_poSearchIn,
    const TChar* i_psName, UINT32 i_nStrLen, SFolderItem** o_ppoFound )
  {
    ENSURE( i_poSearchIn && i_psName && i_psName[ 0 ] && i_nStrLen
      && o_ppoFound );

    * o_ppoFound = FALSE;

    SFolderItem* poCurr = i_poSearchIn->m_poFirstChildFolder;

    while( poCurr )
    {
      if( poCurr->m_nStrLen == i_nStrLen )
      {
        if( 0 == ::memcmp( poCurr->m_psFolderName, i_psName, i_nStrLen ) )
        {
          * o_ppoFound = poCurr;
          return TRUE;
        }
      }

      poCurr = poCurr->m_poNext;
    }

    return TRUE;
  }

  
  /*
  // Find folder corresponding to i_psFilePath,
  // create all subfolder items, and return the target
  // folder item
  BOOL GetFolderForAddTo( TChar* i_psFilePath, UINT32 i_nStrLen,
    SFolderItem** o_ppoFolder )
  {
    // parse i_psFilePath to find folders in it:

    for( UINT32 nPos = 

    return TRUE;
  }
*/
  // Find subfolder with given name.
  BOOL FindSubfolder( SFolderItem* i_poFolderForSearchIn,
    TChar* i_psFolderName, UINT32 i_nStrLen, SFolderItem** o_ppoFound )
  {
    ENSURE( i_poFolderForSearchIn
            && i_psFolderName && i_psFolderName[ 0 ]
            && i_nStrLen && o_ppoFound );

    * o_ppoFound = 0;

    SFolderItem* poSubfolder = i_poFolderForSearchIn->m_poFirstChildFolder;
    while( poSubfolder )
    {
      if( poSubfolder->m_nStrLen == i_nStrLen )
      {
        if( 0 == ::memcmp(
          poSubfolder->m_psFolderName, i_psFolderName, i_nStrLen ) )
        {
          * o_ppoFound = poSubfolder;
          return TRUE;
        }
      }

      poSubfolder = poSubfolder->m_poNext;
    }

    return TRUE;
  }

  SFolderItem m_oRootFolder;
};
