#include <Windows.h>

#ifndef WDM_UTILS_H
#define WDM_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// ---------------------------------------------------------
// Prototypes
// ---------------------------------------------------------

LPWSTR wdm_utils_convert_back_to_forward_slashes(LPWSTR, DWORD);
LPWSTR wdm_utils_convert_forward_to_back_slashes(LPWSTR, DWORD);

// ---------------------------------------------------------

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WDM_UTILS_H