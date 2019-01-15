// XP min!
# define _WIN32_WINNT 0x0501
// From XP (so the first 64-bit OS)
# define _WIN32_IE    0x0600

# define _WIN32_DCOM

# include <windows.h>

# include "./../../../avesus_commons/avedbg.h"
# include "./../../../avesus_commons/aveutils.h"

extern volatile LONG g_nRefCounter;
