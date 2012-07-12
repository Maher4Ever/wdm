#include "global.h"
#include "entry.h"
#include "queue.h"

#ifndef WDM_H
#define WDM_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// ---------------------------------------------------------
// Prototypes
// ---------------------------------------------------------

static void wdm_rb_monitor_mark(LPVOID);
static void wdm_rb_monitor_free(LPVOID);
static VALUE wdm_rb_monitor_alloc(VALUE);

static VALUE wdm_rb_monitor_watch(VALUE, VALUE);

void CALLBACK wdm_rb_handle_entry_change(DWORD, DWORD, LPOVERLAPPED);
static void wdm_rb_register_monitoring_entry(WDM_PEntry);
static DWORD WINAPI wdm_rb_start_monitoring(LPVOID);

static VALUE wdm_rb_wait_for_changes(LPVOID);
static VALUE wdm_rb_process_changes(WDM_PQueue);
static void wdm_rb_stop_monitoring(LPVOID);
static VALUE wdm_rb_monitor_run_bang(VALUE);

static VALUE wdm_rb_monitor_stop(VALUE);

void Init_wdm();

// ---------------------------------------------------------

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WDM_H
