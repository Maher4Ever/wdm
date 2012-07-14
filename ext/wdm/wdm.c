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

VALUE eWDM_InvalidDirectoryError;

ID wdm_rb_sym_call;
ID wdm_rb_sym_at_file;
ID wdm_rb_sym_at_type;
ID wdm_rb_sym_added;
ID wdm_rb_sym_modified;
ID wdm_rb_sym_removed;
ID wdm_rb_sym_renamed_old_file;
ID wdm_rb_sym_renamed_new_file;

rb_encoding *wdm_rb_enc_utf8;

// ----------------------------------------------------------

void
Init_wdm() {
    WDM_DEBUG("Registering WDM with Ruby!");

    wdm_rb_sym_call = rb_intern("call");
    wdm_rb_sym_at_file = rb_intern("@file");
    wdm_rb_sym_at_type = rb_intern("@type");
    wdm_rb_sym_added = rb_intern("added");
    wdm_rb_sym_modified = rb_intern("modified");
    wdm_rb_sym_removed = rb_intern("removed");
    wdm_rb_sym_renamed_old_file = rb_intern("renamed_old_file");
    wdm_rb_sym_renamed_new_file = rb_intern("renamed_new_file");

    wdm_rb_enc_utf8 = rb_utf8_encoding();

    mWDM = rb_define_module("WDM");

    eWDM_InvalidDirectoryError = rb_define_class_under(mWDM, "InvalidDirectoryError", rb_eStandardError);

    wdm_rb_monitor_init();
    wdm_rb_change_init();
}