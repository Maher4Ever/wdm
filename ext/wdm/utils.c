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
wdm_utils_full_pathname(const LPWSTR path) {
    WCHAR maxed_path[WDM_MAX_WCHAR_LONG_PATH];
    LPWSTR full_path;
    size_t full_path_len;

    if ( GetFullPathNameW(path, WDM_MAX_WCHAR_LONG_PATH, maxed_path, NULL) == 0 ) {
        return 0;
    }

    full_path_len = wcslen(maxed_path);
    full_path = ALLOC_N(WCHAR, full_path_len + 1);

    wcscpy(full_path, maxed_path);

    return full_path;
}

BOOL
wdm_utils_unicode_is_directory(const LPWSTR path) {
    WCHAR unicode_path[WDM_MAX_WCHAR_LONG_PATH];

    wcscpy(unicode_path, L"\\\\?\\");
    wcscat(unicode_path, path);

    return wdm_utils_is_directory(unicode_path);
}

BOOL
wdm_utils_is_directory(const LPWSTR path) {
  DWORD dwAttrib = GetFileAttributesW(path);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
         (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}