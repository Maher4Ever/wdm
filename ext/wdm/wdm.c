#include "wdm.h"

#include "entry.h"
#include "queue.h"
#include "monitor.h"

#include "rb_monitor.h"
#include "rb_change.h"

// ----------------------------------------------------------
// Global variables
// ----------------------------------------------------------

VALUE mWDM;

ID wdm_rb_sym_call;
rb_encoding *wdm_rb_enc_utf16;

// ----------------------------------------------------------

void
Init_wdm() {
    WDM_DEBUG("Registering WDM with Ruby!");

    wdm_rb_sym_call = rb_intern("call");
    wdm_rb_enc_utf16 = rb_enc_find("UTF-16LE");

    mWDM = rb_define_module("WDM");

    wdm_rb_monitor_init();
}