# pragma once

# if 0

class CCommaTxtParseStateMachine
{
  typedef CCommaTxtParseStateMachine TSelf;

public:

  CHAR* m_pcDetectedName;
  UINT32 m_nDetectedNameLen;

  CHAR* m_pcDetectedFolderName;
  UINT32 m_nDetectedFolderNameLen;

  enum EState
  {
    STATE_START,
    STATE_INPATH,
    STATE_WAS_START_QUOTES,

    STATE_MAX_ID
  };

  enum EEvent
  {
    EVENT_NONE = 2,
    EVENT_NAME_DETECTED,
    EVENT_FOLDERNAME_DETECTED,
  };

  inline BOOL ParseChar( const CHAR* i_pcChar, EEvent* o_peEvent )
  {
    ENSURE(
      (this->*( m_axSmTable[ m_eState ][ *(BYTE*)(i_pcChar) ] ))(
        i_pcChar, o_peEvent )
    );

    return TRUE;
  }

/*    switch( m_eState )
    {
      break; case STATE_START:
      {
        // can be any symbol, even ','
        switch( * i_pcChar )
        {
          break; case QUOTES:
          {
            m_eState = STATE_WAS_START_QUOTES;
          }

          break; case COMMA:
          {

          }

          break; case SPACE:
          {
          }

          break;
            case SLASH:
            case BACKSLASH:
          {
          }

          break; default:
          {

          }
        }
      }

      break; case STATE_WAS_START_QUOTES:
      {
        switch( * i_pcChar )
        {
          break; case QUOTES:
          {
            m_eState = STATE_WAS_START_QUOTES;
          }

          break; case COMMA:
          {
          }

          break; case SPACE:
          {
            // skip first spaces
          }

          break;
            case SLASH:
            case BACKSLASH:
          {
          }

          break; default:
          {

          }
        }
      }
    }

  */

  CCommaTxtParseStateMachine() :
    m_eState( STATE_START ),
    m_pcDetectedName( 0 ),
    m_nDetectedNameLen( 0 ),
    m_pcDetectedFolderName( 0 ),
    m_nDetectedFolderNameLen( 0 )
  {
    for( UINT32 nState = 0; nState < STATE_MAX_ID; ++ nState )
    {
      for( UINT32 cFor = 0; cFor < 256; ++ cFor )
      {
        m_axSmTable[ nState ][ cFor ] = & TSelf::Default;
      }
    }

    // tune the state machine:

    m_axSmTable[ STATE_START ][ QUOTES ] = & TSelf::Start_Quotes;
    
  }

private:

  static const BYTE QUOTES = '\"';
  static const BYTE COMMA = ',';
  static const BYTE SPACE = ' ';
  static const BYTE SLASH = '/';
  static const BYTE BACKSLASH = '\\';

  typedef BOOL MAction( const CHAR* i_pcChar, EEvent* o_peEvent );

  MAction TSelf::* m_axSmTable[ STATE_MAX_ID ][ 256 ];

  EState m_eState;

  CHAR* m_pcCurrParsedNameStart;

  BOOL Default( const CHAR* i_pcChar, EEvent* o_peEvent )
  {
    * o_peEvent = EVENT_NONE;
    return TRUE;
  }

  BOOL Start_Quotes( const CHAR* i_pcChar, EEvent* o_peEvent )
  {
    // TODO: remove this line for optimize
    * o_peEvent = EVENT_NONE;

    m_eState = STATE_WAS_START_QUOTES;

    return TRUE;
  }

  BOOL Start_Comma( const CHAR* i_pcChar, EEvent* o_peEvent )
  {
    m_pcDetectedName = 0;
    m_nDetectedNameLen = 0;
    * o_peEvent = EVENT_NAME_DETECTED;

    // again start: m_eState == STATE_START

    return TRUE;
  }

  BOOL Start_Space( const CHAR* i_pcChar, EEvent* o_peEvent )
  {
    * o_peEvent = EVENT_NONE;

    // just skip, no state change
    // m_eState == STATE_START

    return TRUE;
  }

  BOOL Start_FileChar( const CHAR* i_pcChar, EEvent* o_peEvent )
  {
    * o_peEvent = EVENT_NONE;

    m_pcCurrParsedNameStart = i_pcChar;
    m_eState = STATE_INPATH;

    return TRUE;
  }

  BOOL InPath_Quotes( const CHAR* i_pcChar, EEvent* o_peEvent )
  {
    // TODO: remove this line for optimize
    * o_peEvent = EVENT_NONE;

    m_eState = STATE_WAS_START_QUOTES;

    return TRUE;
  }

  BOOL InPath_Comma( const CHAR* i_pcChar, EEvent* o_peEvent )
  {
    m_pcDetectedName = 0;
    m_nDetectedNameLen = 0;
    * o_peEvent = EVENT_NAME_DETECTED;

    // again start: m_eState == STATE_START

    return TRUE;
  }

  EEvent InPath_Space( const CHAR* i_pcChar )
  {
    // just skip, no state change
    // m_eState == STATE_START

    return EVENT_NONE;
  }

  EEvent InPath_FileChar( const CHAR* i_pcChar, EEvent* o_peEvent )
  {
    return EVENT_NONE;
  }

  BOOL InPath_PathDelimiter( const CHAR* i_pcChar, EEvent* o_peEvent )
  {
    * o_peEvent = EVENT_NONE;

    m_pcCurrParsedNameStart = i_pcChar;
    m_eState = STATE_INPATH;

    return TRUE;
  }
};

# endif

class CCommatextParser
{
  typedef CCommatextParser TSelf;

  static const BYTE QUOTES = '\"';
  static const BYTE COMMA = ',';
  static const BYTE SPACE = ' ';

  static const BYTE PATH_1 = '/';
  static const BYTE PATH_2 = '\\';

  static const BYTE INV_1 = '?';
  static const BYTE INV_2 = ':';
  static const BYTE INV_3 = '*';
  static const BYTE INV_4 = '>';
  static const BYTE INV_5 = '<';
  static const BYTE INV_6 = '|';

public:

  enum EState
  {
    STATE_START,
    STATE_NAME,
    STATE_PATH,
    STATE_INV_SEQ,

    STATE_QUOTES,
    STATE_DBL_QUOTES,

    STATE_NAME_IN_QUOTES,
    STATE_PATH_IN_QUOTES,
    STATE_INV_SEQ_IN_QUOTES,

    STATE_SKIP,


    STATE_MAX_ID
  };

  BOOL Init( CParseTree< CHAR >::SParsedFolderPath* i_poPathContainer )
  {
    ENSURE( i_poPathContainer );

    m_poCurrBuildingPath = i_poPathContainer;

    m_pcFoundFileName = 0;
    m_nFoundFileNameLen = 0;

    // TUNE THE STATE MACHINE:

#   include "./../../../avesus_commons/with_state_machine_init_helper.h"

    FOR( STATE_START )
    {
      ON( SPACE )
      OR( 0 )
      {
      }
       
      ON( COMMA )
      {
        EVENT( OnEmptyNameDetected );
        NEXT( STATE_START );
      }

      ON( QUOTES )
      {
        NEXT( STATE_QUOTES );
      }

      ON( PATH_1 )
      OR( PATH_2 )
      OR( INV_1 )
      OR( INV_2 )
      OR( INV_3 )
      OR( INV_4 )
      OR( INV_5 )
      OR( INV_6 )
      {
        NEXT( STATE_INV_SEQ );
        STORE_PTR();
      }

      ON_CHAR()
      {
        NEXT( STATE_NAME );
        STORE_PTR();
      }

    } END_FOR;

    FOR( STATE_NAME )
    {
      ON( SPACE )
      OR( COMMA )
      OR( 0 )
      {
        EVENT( OnNameDetected );
        NEXT( STATE_START );
      }
       
      ON( PATH_1 )
      OR( PATH_2 )
      {
        EVENT( OnPathDetected );
        NEXT( STATE_PATH );
      }

      ON( QUOTES )
      OR( INV_1 )
      OR( INV_2 )
      OR( INV_3 )
      OR( INV_4 )
      OR( INV_5 )
      OR( INV_6 )
      {
        NEXT( STATE_INV_SEQ );
      }

      ON_CHAR()
      {
      }

    } END_FOR;

    FOR( STATE_INV_SEQ )
    {
      ON( SPACE )
      OR( COMMA )
      OR( 0 )
      {
        EVENT( OnInvalidSeqDetected );
        NEXT( STATE_START );
      }
       
      ON( PATH_1 )
      OR( PATH_2 )
      OR( QUOTES )
      OR( INV_1 )
      OR( INV_2 )
      OR( INV_3 )
      OR( INV_4 )
      OR( INV_5 )
      OR( INV_6 )
      OR_CHAR()
      {
      }

    } END_FOR;

    FOR( STATE_PATH )
    {
      ON( SPACE )
      OR( COMMA )
      OR( 0 )
      {
        NEXT( STATE_START );
      }
       
      ON( PATH_1 )
      OR( PATH_2 )
      {
      }

      ON( QUOTES )
      OR( INV_1 )
      OR( INV_2 )
      OR( INV_3 )
      OR( INV_4 )
      OR( INV_5 )
      OR( INV_6 )
      {
        STORE_PTR();
        NEXT( STATE_INV_SEQ );
      }

      ON_CHAR()
      {
        STORE_PTR();
        NEXT( STATE_NAME );
      }

    } END_FOR;


    FOR( STATE_QUOTES )
    {
      ON( SPACE )
      {
      }

      ON( 0 )
      {
        EVENT( OnFormatFailure );
        NEXT( STATE_START );
      }

      ON( QUOTES )
      {
        NEXT( STATE_DBL_QUOTES );
      }

      ON( PATH_1 )
      OR( PATH_2 )
      OR( INV_1 )
      OR( INV_2 )
      OR( INV_3 )
      OR( INV_4 )
      OR( INV_5 )
      OR( INV_6 )
      {
        STORE_PTR();
        NEXT( STATE_INV_SEQ_IN_QUOTES );
      }

      ON( COMMA )
      OR_CHAR()
      {
        STORE_PTR();
        NEXT( STATE_NAME_IN_QUOTES );
      }

    } END_FOR;


    FOR( STATE_DBL_QUOTES )
    {
      ON( SPACE )
      OR( COMMA )
      OR( 0 )
      {
        EVENT( OnEmptyNameDetected );
        NEXT( STATE_START );
      }

      ON( QUOTES )
      OR( PATH_1 )
      OR( PATH_2 )
      OR( INV_1 )
      OR( INV_2 )
      OR( INV_3 )
      OR( INV_4 )
      OR( INV_5 )
      OR( INV_6 )
      OR_CHAR()
      {
        EVENT( OnFormatFailure );
        NEXT( STATE_START );
      }

    } END_FOR;


    FOR( STATE_INV_SEQ_IN_QUOTES )
    {
      ON( 0 )
      {
        EVENT( OnFormatFailure );
        NEXT( STATE_START );
      }

      ON( QUOTES )
      {
        EVENT( OnInvalidSeqDetected );
        NEXT( STATE_SKIP );
      }
      
      ON( SPACE )
      OR( COMMA )
      OR( PATH_1 )
      OR( PATH_2 )
      OR( INV_1 )
      OR( INV_2 )
      OR( INV_3 )
      OR( INV_4 )
      OR( INV_5 )
      OR( INV_6 )
      OR_CHAR()
      {
      }

    } END_FOR;


    FOR( STATE_PATH_IN_QUOTES )
    {
      ON( 0 )
      {
        EVENT( OnFormatFailure );
        NEXT( STATE_START );
      }

      ON( QUOTES )
      {
        NEXT( STATE_SKIP );
      }

      ON( SPACE )
      OR( PATH_1 )
      OR( PATH_2 )
      {
      }

      ON( INV_1 )
      OR( INV_2 )
      OR( INV_3 )
      OR( INV_4 )
      OR( INV_5 )
      OR( INV_6 )
      {
        NEXT( STATE_INV_SEQ_IN_QUOTES );
      }

      ON( COMMA )
      OR_CHAR()
      {
        STORE_PTR();
        NEXT( STATE_NAME_IN_QUOTES );
      }

    } END_FOR;


    FOR( STATE_SKIP )
    {
      ON( SPACE )
      {
        // if next after space is comma, skip it also
        EVENT( OnSmartSkip );
        NEXT( STATE_START );
      }

      ON( COMMA )
      OR( 0 )
      {
        NEXT( STATE_START );
      }

      ON( PATH_1 )
      OR( PATH_2 )
      OR( QUOTES )
      OR( INV_1 )
      OR( INV_2 )
      OR( INV_3 )
      OR( INV_4 )
      OR( INV_5 )
      OR( INV_6 )
      OR_CHAR()
      {
        EVENT( OnFormatFailure );
        NEXT( STATE_START );
      }

    } END_FOR;

    FOR( STATE_NAME_IN_QUOTES )
    {
      ON( 0 )
      {
        EVENT( OnFormatFailure );
        NEXT( STATE_START );
      }
      
      ON( PATH_1 )
      OR( PATH_2 )
      {
        EVENT( OnPathDetected );
        NEXT( STATE_PATH_IN_QUOTES );
      }

      ON( QUOTES )
      {
        EVENT( OnNameDetected );
        NEXT( STATE_SKIP );
      }

      ON( INV_1 )
      OR( INV_2 )
      OR( INV_3 )
      OR( INV_4 )
      OR( INV_5 )
      OR( INV_6 )
      {
        NEXT( STATE_INV_SEQ_IN_QUOTES );
      }

      ON( SPACE )
      OR( COMMA )
      OR_CHAR()
      {
      }

    } END_FOR;

/*
    for( UINT32 cFor = 0; cFor < 256; ++ cFor )
    {
      CAnsiLexicalParser< TSelf, EState >::SStateDef& def
        = m_aoStatesTable[ nState ][ cFor ];

      BOOL (TSelf::*&evt)( void ) = def.m_xEvent;
      BOOL& store = def.m_fStoreStart;
      EState& next = def.m_eNextState;

      evt = 0;
      store = FALSE;
      next = nState;

      switch( (BYTE) cFor )
      {
        break; case SPACE:

        break; case COMMA:

          evt = & TSelf::OnEmptyNameDetected;
          next = STATE_START;

        break; case QUOTES:

          next = STATE_QUOTES;

        break;
          case PATH_1:
          case PATH_2:
          case INV_1:
          case INV_2:
          case INV_3:
          case INV_4:
          case INV_5:
          case INV_6:
        { // PATH / INV
          
          def.m_xEvent = 0;
          def.m_eNextState = STATE_INV_SEQ;
          def.m_fStoreStart = TRUE;
        }

        break; default:
        { // for filepath symbols

          m_aoStatesTable[ STATE_START ][ cFor ].m_xEvent = 0;
          m_aoStatesTable[ STATE_START ][ cFor ].m_eNextState = STATE_NAME;
          m_aoStatesTable[ STATE_START ][ cFor ].m_fStoreStart = TRUE;
        }
      }
    }
    */

#   include "./../../../avesus_commons/without_state_machine_init_helper.h"
   
    return TRUE;
  }

  BOOL ExtractFilePath(
    __inout const CHAR** io_ppcCommaDelimitedFilePathsList,
    __inout UINT32* io_pnRestListLen,
    __out   const CHAR** o_ppcFileName,
    __out   UINT32* o_pnFileNameLength )
  {
    ENSURE( io_ppcCommaDelimitedFilePathsList
      && io_pnRestListLen
      && o_ppcFileName
      && o_pnFileNameLength );

    ENSURE( m_oParser.Parse( this, m_aoStatesTable,
      io_ppcCommaDelimitedFilePathsList,
      io_pnRestListLen ) );

    * o_ppcFileName = m_pcFoundFileName;
    * o_pnFileNameLength = m_nFoundFileNameLen;

    return TRUE;
  }

  BOOL OnIncompleteString( void )
  {
    // m_oParser.Parse() will return this value
    return TRUE;
  }

private:

  // Events from the state machine:

  BOOL OnEmptyNameDetected( void )
  {
    m_pcFoundFileName = 0;
    m_nFoundFileNameLen = 0;
    return FALSE;
  }

  BOOL OnPathDetected( void )
  {
    // TODO:
    // EXPAND THIS CODE if plugins will support the empty folders.

    // create a new folder, if there's no pre-cached one:
    if( ! m_poCurrBuildingPath->m_poNext )
    {
      m_poCurrBuildingPath->m_poNext
        = new CParseTree< CHAR >::SParsedFolderPath();

      ENSURE( m_poCurrBuildingPath->m_poNext );
    }
    else
    {
      // clear terminator:

      m_poCurrBuildingPath->m_poNext->m_nStrLen = 0;
      m_poCurrBuildingPath->m_poNext->m_psFolderName = 0;
    }

    // store found info:
    m_poCurrBuildingPath->m_psFolderName
      = m_oParser.GetDetectedCharPtr();

    m_poCurrBuildingPath->m_nStrLen
      = UINT32(m_oParser.GetCurrCharPtr() - m_oParser.GetDetectedCharPtr());

    // advance:
    m_poCurrBuildingPath = m_poCurrBuildingPath->m_poNext;

    return TRUE;
  }

  BOOL OnNameDetected( void )
  {
    m_pcFoundFileName = m_oParser.GetDetectedCharPtr();
    m_nFoundFileNameLen
      = UINT32(m_oParser.GetCurrCharPtr() - m_oParser.GetDetectedCharPtr());

    return FALSE;
  }

  BOOL OnSmartSkip( void )
  {
    if( COMMA == * (BYTE*) & ( m_oParser.GetCurrCharPtr() )[ 1 ] )
    {
      m_oParser.AdvanceCharPtr();
    }

    return TRUE;
  }

  BOOL OnInvalidSeqDetected( void )
  {
    return FALSE;
  }

  BOOL OnFormatFailure( void )
  {
    return FALSE;
  }

  CAnsiLexicalParser< TSelf, EState >::SStateDef
    m_aoStatesTable[ STATE_MAX_ID ][ 256 ];

  CAnsiLexicalParser< TSelf, EState > m_oParser;

  CParseTree< CHAR >::SParsedFolderPath* m_poCurrBuildingPath;
  const CHAR* m_pcFoundFileName;
  UINT32 m_nFoundFileNameLen;

};
