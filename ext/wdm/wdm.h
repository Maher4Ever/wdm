#include <stdio.h>

#define WINVER 0x0500       // Support Windows 2000 and later,
#define _WIN32_WINNT 0x0500 // this is needed for 'GetLongPathNameW'

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers
#endif

#include <Windows.h>

#include <ruby.h>
#include <ruby/encoding.h>

#ifndef WDM_H
#define WDM_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// ---------------------------------------------------------
// Constants
// ---------------------------------------------------------

#define WDM_DEBUG_ENABLED TRUE

#define WDM_BUFFER_SIZE 16384 // 2^14 or 16Kb

// ---------------------------------------------------------
// Macros
// ---------------------------------------------------------

#if WDM_DEBUG_ENABLED == TRUE
    #define WDM_DEBUG(str, ...) \
        fprintf(stderr, "[DEBUG] (%s@%d): " str "\n", __FILE__, __LINE__, ##__VA_ARGS__)

    #define WDM_WDEBUG(str, ...) \
        fwprintf(stderr, L"[DEBUG] (%S@%d): " str "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#else
    #define WDM_DEBUG(str, ...)
    #define WDM_WDEBUG(str, ...)
#endif

// ----------------------------------------------------------
// Extern global variables
// ----------------------------------------------------------

extern VALUE mWDM;

extern ID wdm_rb_sym_call;
extern ID wdm_rb_sym_at_file;
extern ID wdm_rb_sym_at_type;
extern ID wdm_rb_sym_added;
extern ID wdm_rb_sym_modified;
extern ID wdm_rb_sym_removed;
extern ID wdm_rb_sym_renamed_old_file;
extern ID wdm_rb_sym_renamed_new_file;

extern rb_encoding *wdm_rb_enc_utf8;

// ---------------------------------------------------------

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WDM_H