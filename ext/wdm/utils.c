#include "wdm.h"

#include "utils.h"

// ---------------------------------------------------------
// Paths functions
// ---------------------------------------------------------

LPWSTR
wdm_utils_convert_back_to_forward_slashes(LPWSTR path, DWORD path_len) {
    UINT i;

    for(i = 0; i < (path_len - 1); ++i) { // path_len-1 because we don't need to check the NULL-char!
        if ( path[i] == L'\\' ) path[i] = L'/';
    }

    return path;
}

LPWSTR
wdm_utils_convert_forward_to_back_slashes(LPWSTR path, DWORD path_len) {
    UINT i;

    for(i = 0; i < (path_len - 1); ++i) { // path_len-1 because we don't need to check the NULL-char!
        if ( path[i] == L'/' ) path[i] = L'\\';
    }

    return path;
}