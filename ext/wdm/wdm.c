#include "global.h"
#include "wdm.h"
#include "entry.h"
#include "queue.h"
#include "monitor.h"

// ----------------------------------------------------------
// Global variables
// ----------------------------------------------------------

VALUE mWDM;

VALUE cWDM_Monitor;

// ----------------------------------------------------------
// Cached values
// ----------------------------------------------------------

static ID wdm_rb_sym_call;
static rb_encoding *wdm_rb_enc_utf16;

// ----------------------------------------------------------

static void
wdm_rb_monitor_mark(LPVOID param) {
    WDM_PMonitor monitor;
    WDM_PEntry entry;

    monitor = (WDM_PMonitor)param;
    entry = monitor->head;

    while(entry != NULL) {
        rb_gc_mark(entry->user_data->callback);
        entry = entry->next;
    }
}

static void
wdm_rb_monitor_free(LPVOID param) {
    WDM_PMonitor monitor;
    WDM_PEntry entry;
    
    WDM_DEBUG("Freeing a monitor object!");
    
    monitor = (WDM_PMonitor)param;
    entry = monitor->head;

    wdm_rb_stop_monitoring(monitor); // If the monitor is already stopped, it would do nothing

    while(entry != NULL) {
        if ( entry->event_container.hEvent != NULL ) {
            wdm_monitor_callback_param_free(
                (WDM_PMonitorCallbackParam)entry->event_container.hEvent
            );
        }
        entry = entry->next;
    }

    wdm_monitor_free(monitor);
}

static VALUE
wdm_rb_monitor_alloc(VALUE self) {
    WDM_DEBUG("Allocating a new monitor object!");
    return Data_Wrap_Struct(self, wdm_rb_monitor_mark, wdm_rb_monitor_free, wdm_monitor_new());
}

static VALUE
wdm_rb_monitor_watch(VALUE self, VALUE directory) {
    WDM_PMonitor monitor;
    WDM_PEntry entry;
#ifdef  UNICODE
    VALUE wchar_directory;
#endif

    Check_Type(directory, T_STRING);

    // TODO: Maybe raise a more user-friendly error?
    rb_need_block();

    entry = wdm_entry_new();
    entry->user_data->callback =  rb_block_proc();

#ifdef  UNICODE
    wchar_directory = rb_str_export_to_enc(directory, wdm_rb_enc_utf16);
    entry->user_data->dir = (LPTSTR)RSTRING_PTR(wchar_directory);
#else
    entry->user_data->dir = RSTRING_PTR(directory);
#endif
 
    entry->dir_handle = CreateFile(
        entry->user_data->dir,     // pointer to the file name
        FILE_LIST_DIRECTORY,       // access (read/write) mode
        FILE_SHARE_READ            // share mode
            | FILE_SHARE_WRITE
            | FILE_SHARE_DELETE,
        NULL,                       // security descriptor
        OPEN_EXISTING,              // how to create
        FILE_FLAG_BACKUP_SEMANTICS
            | FILE_FLAG_OVERLAPPED, // file attributes
        NULL
    );

    if ( entry->dir_handle ==  INVALID_HANDLE_VALUE ) {
        rb_raise(rb_eRuntimeError, "Can't watch directory: '%s'!", RSTRING_PTR(directory));
    }

    Data_Get_Struct(self, WDM_Monitor, monitor);
    
    // Store a reference to the entry instead of an event as the event
    // won't be used when using callbacks.
    entry->event_container.hEvent = wdm_monitor_callback_param_new(monitor, entry);

    wdm_monitor_update_head(monitor, entry);

    WDM_DEBUG("Watching: '%s'", RSTRING_PTR(directory));

    return Qnil;
}

void CALLBACK
wdm_rb_handle_entry_change(
    DWORD err_code,                 // completion code
    DWORD bytes_transfered,         // number of bytes transferred
    LPOVERLAPPED event_container    // I/O information buffer
) {
    WDM_PMonitorCallbackParam param;
    WDM_PQueueItem data_to_process;

    if ( err_code == ERROR_OPERATION_ABORTED ) {
        // Async operation was canceld. This shouldn't happen.
        // TODO: 
        //   1. Maybe add a union in the queue for errors?
        //   2. What's the best action when this happens?
        WDM_DEBUG("Dir handler closed in the process callback!");
        return;
    }

    if ( ! bytes_transfered ) {
        WDM_DEBUG("Buffer overflow?! Changes are bigger than the buffer!");
        return;
    }

    param = (WDM_PMonitorCallbackParam)event_container->hEvent;
    data_to_process = wdm_queue_item_new();

    WDM_DEBUG("Change detected in '%s'", param->entry->user_data->dir);

    data_to_process->user_data = param->entry->user_data;

    // Copy change data to the backup buffer
    memcpy(data_to_process->buffer, param->entry->buffer, bytes_transfered);

    // Add the backup buffer to the change queue
    wdm_queue_enqueue(param->monitor->changes, data_to_process);

    // Resume watching the dir for changes
    wdm_rb_register_monitoring_entry(param->entry);

    // Tell the processing thread to process the changes
    if ( WaitForSingleObject( param->monitor->process_event, 0) != WAIT_OBJECT_0 ) { // Check if already signaled
        SetEvent(param->monitor->process_event);
    }
} 

static void
wdm_rb_register_monitoring_entry(WDM_PEntry entry) {
    BOOL success;
    DWORD bytes;
    bytes = 0; // Not used because the process callback gets passed the written bytes

    success = ReadDirectoryChangesW(
        entry->dir_handle,                  // handle to directory
        entry->buffer,                      // read results buffer
        WDM_BUFFER_SIZE,                    // length of buffer
        entry->user_data->watch_childeren,  // monitoring option
        FILE_NOTIFY_CHANGE_LAST_WRITE       // filter conditions
            | FILE_NOTIFY_CHANGE_CREATION
            | FILE_NOTIFY_CHANGE_FILE_NAME,        
        &bytes,                             // bytes returned
        &entry->event_container,            // overlapped buffer
        &wdm_rb_handle_entry_change         // process callback
    );

    if ( ! success ) {
        // --------------------------
        // TODO: Handle error!
        // --------------------------
        WDM_DEBUG("Failed registering directory: '%s'!", entry->user_data->dir);
    }
}

static DWORD WINAPI
wdm_rb_start_monitoring(LPVOID param) {
    WDM_PMonitor monitor;
    WDM_PEntry curr_entry;

    monitor = (WDM_PMonitor)param;
    curr_entry = monitor->head;

    WDM_DEBUG("Starting the monitoring thread!");

    while(curr_entry != NULL) {
        wdm_rb_register_monitoring_entry(curr_entry);
        curr_entry = curr_entry->next;
    }

    while(monitor->running) {
        // TODO: Is this the best way to do it?
        if ( WaitForSingleObjectEx(monitor->stop_event, INFINITE, TRUE) == WAIT_OBJECT_0) {
            WDM_DEBUG("Exiting the monitoring thread!");
            ExitThread(0);
        }
    }

    return 0;
}

static VALUE
wdm_rb_wait_for_changes(LPVOID param) {
    HANDLE process_event;

    process_event = (HANDLE)param;

    return WaitForSingleObject(process_event, INFINITE) == WAIT_OBJECT_0 ? Qtrue : Qfalse;
}

static void 
wdm_rb_process_changes(WDM_PQueue changes) {
    WDM_PQueueItem item;

    while( ! wdm_queue_is_empty(changes) ) {
        item = wdm_queue_dequeue(changes);
        rb_funcall(item->user_data->callback, wdm_rb_sym_call, 0);
        wdm_queue_item_free(item);
    }
}

static void
wdm_rb_stop_monitoring(LPVOID param) {
    BOOL already_stopped;
    WDM_PMonitor monitor;
    WDM_PEntry entry;

    monitor = (WDM_PMonitor)param;
    already_stopped = FALSE;

    WDM_DEBUG("Stopping the monitor!");

    EnterCriticalSection(&monitor->lock);
        if ( ! monitor->running ) {
            already_stopped = TRUE;
        }
        else {
            monitor->running = FALSE;
        }
    LeaveCriticalSection(&monitor->lock);

    if (already_stopped) {
        WDM_DEBUG("Can't stop monitoring because it's already stopped (or it's never been started)!!");
        return;
    }

    entry = monitor->head;

    while(entry != NULL) {
        CancelIo(entry->dir_handle); // Stop monitoring changes
        entry = entry->next;
    }

    SetEvent(monitor->stop_event);
    SetEvent(monitor->process_event); // The process code checks after the wait for an exit signal
    WaitForSingleObject(monitor->monitoring_thread, 10000);
}

static VALUE
wdm_rb_monitor_run_bang(VALUE self) {
    DWORD thread_id;
    BOOL already_running,
         waiting_succeeded;
    WDM_PMonitor monitor;

    WDM_DEBUG("Running the monitor!");

    Data_Get_Struct(self, WDM_Monitor, monitor);
    already_running = FALSE;

    EnterCriticalSection(&monitor->lock);
        if ( monitor->running ) {
            already_running = TRUE;
        }
        else {
            monitor->running = TRUE;
        }
    LeaveCriticalSection(&monitor->lock);

    if (already_running) {
        WDM_DEBUG("Not doing anything because the monitor is already running!");
        return;
    }

    // Reset events
    ResetEvent(monitor->process_event);
    ResetEvent(monitor->stop_event);

    monitor->monitoring_thread = CreateThread( 
        NULL,                     // default security attributes
        0,                        // use default stack size  
        wdm_rb_start_monitoring,  // thread function name
        monitor,                  // argument to thread function 
        0,                        // use default creation flags 
        &thread_id                // returns the thread identifier 
    );    

    if ( monitor->monitoring_thread == NULL ) {
        rb_raise(rb_eRuntimeError, "Can't create a thread for the monitor!");
    }

    while ( monitor->running ) {
        waiting_succeeded = rb_thread_blocking_region(wdm_rb_wait_for_changes, monitor->process_event, wdm_rb_stop_monitoring, monitor);

        if ( waiting_succeeded == Qfalse ) {
            rb_raise(rb_eRuntimeError, "Failed while waiting for a change in the watched directories!");
        }

        if ( ! monitor->running ) {
            wdm_queue_empty(monitor->changes);
            return;
        }
        
        wdm_rb_process_changes(monitor->changes);

        if ( ! ResetEvent(monitor->process_event) ) {
            rb_raise(rb_eRuntimeError, "Couldn't reset system events to watch for changes!");
        }
    }

    return Qnil;
}

static VALUE
wdm_rb_monitor_stop(VALUE self) {
    WDM_PMonitor monitor;

    Data_Get_Struct(self, WDM_Monitor, monitor);

    wdm_rb_stop_monitoring(monitor);

    WDM_DEBUG("Stopped the monitor!");

    return Qnil;
}

void
Init_wdm() {
    WDM_DEBUG("Registering WDM with Ruby!");

    wdm_rb_sym_call = rb_intern("call");
    wdm_rb_enc_utf16 = rb_enc_find("UTF-16LE");

    mWDM = rb_define_module("WDM");
    cWDM_Monitor = rb_define_class_under(mWDM, "Monitor", rb_cObject);

    rb_define_alloc_func(cWDM_Monitor, wdm_rb_monitor_alloc);
    rb_define_method(cWDM_Monitor, "watch", RUBY_METHOD_FUNC(wdm_rb_monitor_watch), 1);
    rb_define_method(cWDM_Monitor, "run!", RUBY_METHOD_FUNC(wdm_rb_monitor_run_bang), 0);
    rb_define_method(cWDM_Monitor, "stop", RUBY_METHOD_FUNC(wdm_rb_monitor_stop), 0);
}
