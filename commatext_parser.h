# pragma once

# include "commatext_state_machine.h"

// Extract next filename from COMMATEXT
// and create or reuse folder path linked list.
// Note that if file is root-level, no
// list items will be created and
// o_poPath->m_nStrLen will be filled with 0.
// i_nRestListLen - count of the rest characters in the COMMATEXT.
static BOOL ExtractFilePath(
  __inout const CHAR** io_ppcCommaDelimitedFilePathsList,
  __inout UINT32* io_pnRestListLen,
  __out   CParseTree< CHAR >::SParsedFolderPath* o_poPath,
  __out   const CHAR** o_ppcFileName,
  __out   UINT32* o_pnFileNameLength )
{
  ENSURE( io_ppcCommaDelimitedFilePathsList
    && io_pnRestListLen
    && o_ppcFileName
    && o_poPath && o_pnFileNameLength );

  // This function parses mixed-string-style COMMATEXT
  // filelists:
  // "Stri,ng 1", "Stri""ng 2" , String 3,String4
  // string will be parsed as:
  //   Stri,ng 1
  //   Stri"ng 2
  //   String
  //   3
  //   String4

  // Current implementation assumes that the filepaths
  // contains no any quotes ("), because it's deprecated
  // filesystem symbol.

  // if quotes symbol is found in the list,
  // it starts or ends a filepath containing commas or spaces.
  
  // Special symbols:

  // states:
  // 1. START
  //    action: parse
  //    if( cQuotes )
  //    
  // 2. 

  // if " ->
  //   if next is no "
  //     -> prev " was start of filepath
  //       perhaps containing spaces and commas.
  //   if next is "" ->

  const CHAR* pcCommaTxt = * io_ppcCommaDelimitedFilePathsList;

  * o_ppcFileName = 0;
  * o_pnFileNameLength = 0;

  // clear the path
  o_poPath->m_nStrLen = 0;
  o_poPath->m_psFolderName = 0;

  if( ! pcCommaTxt[ 0 ]
      ||
      ! * io_pnRestListLen )
  {
    // end of parsing
    * io_ppcCommaDelimitedFilePathsList = 0;

    return TRUE;
  }

  //CParseTree< CHAR >::SParsedFolderPath* poLastFoundFolder = o_poPath;

  //CHAR* pcCurrName = 0;

  //BOOL fnNextCanBeStartOfName = FALSE;

  //UINT32 nCurrNameLen = 0;

  CCommatextParser oParser;
  ENSURE( oParser.Init( o_poPath ) );
/*  CCommaTxtParseStateMachine::EEvent eEvent =
    CCommaTxtParseStateMachine::EVENT_NONE;*/

  ENSURE( oParser.ExtractFilePath( io_ppcCommaDelimitedFilePathsList,
    io_pnRestListLen, o_ppcFileName, o_pnFileNameLength ) );
/*
  // each byte parsing
  for( CHAR* pChar = pcCommaTxt; pChar < & pcCommaTxt[ * io_pnRestListLen ];
       ++ pChar )
  {
    ENSURE( oParseMachine.ParseChar( pChar, & eEvent ) );

    switch( eEvent )
    {
      break; case CCommaTxtParseStateMachine::EVENT_NONE:
      {
        continue;
      }

      break; case CCommaTxtParseStateMachine::EVENT_FOLDERNAME_DETECTED:
      {
        // create a new folder, if there's no pre-cached one:
        if( ! poLastFoundFolder->m_poNext )
        {
          poLastFoundFolder->m_poNext
            = new CParseTree< CHAR* >::SParsedFolderPath();

          ENSURE( poLastFoundFolder->m_poNext );
        }
        else
        {
          // clear terminator:

          poLastFoundFolder->m_poNext->m_nStrLen = 0;
          poLastFoundFolder->m_poNext->m_psFolderName = 0;
        }

        // store found info:
        poLastFoundFolder->m_psFolderName
          = oParseMachine.m_pcDetectedFolderName;
        poLastFoundFolder->m_nStrLen
          = oParseMachine.m_nDetectedFolderNameLen;

        // advance:
        poLastFoundFolder = poLastFoundFolder->m_poNext;

        // EXPAND THIS CODE if plugins will be supporting the empty folders.
      }

      break; case CCommaTxtParseStateMachine::EVENT_NAME_DETECTED:
      {
        * o_ppcFileName = oParseMachine.m_pcDetectedName;
        * o_pnFileNameLength = oParseMachine.m_nDetectedNameLen;

        // advance pChar to the next char:
        ++ pChar;

        // calc eaten count:
        UINT32 nEaten = pChar - pcCommaTxt;

        * io_pnRestListLen -= nEaten;

        * io_ppcCommaDelimitedFilePathsList = pChar;

        return TRUE;
      }
    }
/*
    if( '\"' == cCurr )
    {
      if( ! fnNextCanBeStartOfName )
      { // opening

        fnNextCanBeStartOfName = TRUE;
      }
      else
      { // closing

        fnNextCanBeStartOfName = FALSE;

        if( nCurrNameLen )
        { // if was enclosing text

          * o_ppcFileName = pcCurrName;
          * o_pnFileNameLength = nCurrNameLen;

          // skip to next " or end of COMMATEXT:
          

        }
      }

      continue;
    }

    if( fnNextCanBeStartOfName )
    {
      pcCurrName = & pcCommaTxt[ nChar ];
      ++ nCurrNameLen;
    }

    */
  //}

  return TRUE;
}
