#include "global.h"
#include "entry.h"
#include "queue.h"

#ifndef WDM_RB_MONITOR_H
#define WDM_RB_MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// ----------------------------------------------------------
// Global variables
// ----------------------------------------------------------

extern VALUE mWDM;
VALUE cWDM_Monitor;

// ---------------------------------------------------------
// Prototypes
// ---------------------------------------------------------

void wdm_rb_monitor_init();

// ---------------------------------------------------------

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WDM_RB_MONITOR_H