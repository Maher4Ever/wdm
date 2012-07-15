#ifndef WDM_RB_MONITOR_H
#define WDM_RB_MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// ---------------------------------------------------------
// Constants
// ---------------------------------------------------------

#define WDM_MONITOR_FLAGS_DEFAULT       \
    FILE_NOTIFY_CHANGE_LAST_WRITE       \
        | FILE_NOTIFY_CHANGE_CREATION   \
        | FILE_NOTIFY_CHANGE_FILE_NAME

// ----------------------------------------------------------
// Global variables
// ----------------------------------------------------------

extern VALUE cWDM_Monitor;

extern VALUE eWDM_UnknownFlagError;
extern VALUE eWDM_MonitorRunningError;
extern VALUE eWDM_InvalidDirectoryError;

// ---------------------------------------------------------
// Prototypes
// ---------------------------------------------------------

void wdm_rb_monitor_init();

// ---------------------------------------------------------

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WDM_RB_MONITOR_H