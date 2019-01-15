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

// third-party Windows
# pragma pack( push, 8 )

  // GetLastError()
  # include <windows.h>

  // _ASSERT()
  # include <crtdbg.h>

# pragma pack( pop )

# include <stdio.h>

# ifdef _DEBUG
  # define DEBUG_BREAK ::DebugBreak()
# else
  # define DEBUG_BREAK ((void)0)
# endif

//warning C4505: 'DebugPrintA' : unreferenced local function has been removed
# pragma warning( disable: 4505 )

static void __cdecl DebugPrintA( LPCSTR pcFormatter, ... )
{
	DWORD nErr = ::GetLastError();

  va_list pcArgs;
	va_start( pcArgs, pcFormatter );

  char acFormatted[ 1024 ] = { 0 };

  ::vsprintf_s( acFormatted, sizeof( acFormatted ), pcFormatter, pcArgs );
  ::OutputDebugStringA( acFormatted );

  va_end( pcArgs );

  ::SetLastError( nErr );
}

# ifdef _DEBUG

  # define VERIFY( _expr_ )                                                    \
    ( ( _expr_ ) ? true :                                                      \
      ( DebugPrintA( "VERIFY( %s ) failed: file \"%s\" line %d.\n",            \
       #_expr_, __FILE__, __LINE__ ), DEBUG_BREAK, false ) )                   \

  # define ENSURE( _expr_ )                                                    \
  do                                                                           \
  {                                                                            \
    if( (void)0, ! (_expr_) )                                                  \
    {                                                                          \
      DebugPrintA( "ENSURE( %s ) failed: file \"%s\" line %d.\n",              \
       #_expr_, __FILE__, __LINE__ );                                          \
      DEBUG_BREAK;                                                             \
      return false;                                                            \
    }                                                                          \
  }                                                                            \
  while( (void)0, 0 )

# else

  # define VERIFY( _expr_ )                                                    \
    ( ( _expr_ ) ? true : false )

  # define ENSURE( _expr_ )                                                    \
  do                                                                           \
  {                                                                            \
    if( (void)0, ! (_expr_) )                                                  \
    {                                                                          \
      return false;                                                            \
    }                                                                          \
  }                                                                            \
  while( (void)0, 0 )

# endif

