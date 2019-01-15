// This header only for include within state machine initialization
// method!!!
// Before the use, immediately
// include without_state_machine_init_helper.h !!!

UINT32 nState = 0;

# define FOR( STATE ) \
    nState = STATE; \
    for( UINT32 cFor = 0; cFor < 256; ++ cFor ) \
    { \
      CAnsiLexicalParser< TSelf, EState >::SStateDef& def \
        = m_aoStatesTable[ nState ][ cFor ]; \
 \
      BOOL (TSelf::*&evt)( void ) = def.m_xEvent; \
      BOOL& store = def.m_fStoreStart; \
      EState& next = def.m_eNextState; \
 \
      evt = 0; \
      store = FALSE; \
      next = (EState)nState; \
 \
      switch( (BYTE) cFor )

# define END_FOR } nState

# define ON( _EVENT ) \
  break; case _EVENT:

# define OR( _EVENT ) \
    case _EVENT:

# define ON_CHAR() \
  break; default:

# define OR_CHAR() \
    default:

# define EVENT( _EVENT ) \
  evt = & TSelf::_EVENT

# define NEXT( _STATE ) \
  next = _STATE

# define STORE_PTR() \
  store = TRUE

