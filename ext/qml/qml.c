#include "qml.h"
#include "application.h"
#include "component.h"
#include "engine.h"
#include "exporter.h"
#include "interface.h"
#include "js_object.h"
#include "js_array.h"
#include "js_function.h"
#include "js_wrapper.h"
#include "signal_emitter.h"
#include "plugin_loader.h"
#include "dispatcher.h"
#include "meta_object.h"

VALUE rbqml_mQML;
VALUE rbqml_application = Qnil;
VALUE rbqml_engine = Qnil;

/*
 * @api private
 */
static VALUE qml_init(VALUE module, VALUE args) {
    if (!NIL_P(rbqml_application)) {
        rb_raise(rb_eRuntimeError, "QML already initialized");
    }

    rbqml_application = rb_funcall(rbqml_cApplication, rb_intern("new"), 1, args);
    rbqml_engine = rb_funcall(rbqml_cEngine, rb_intern("new"), 0);

    rb_gc_register_address(&rbqml_application);
    rb_gc_register_address(&rbqml_engine);

    VALUE blocks = rb_const_get(module, rb_intern("INIT_BLOCKS"));
    for (int i = 0; i < RARRAY_LEN(blocks); ++i) {
        rb_proc_call(RARRAY_AREF(blocks, i), rb_ary_new());
    }

    return module;
}

static VALUE qml_initialized_p(VALUE module) {
    if (NIL_P(rbqml_application)) {
        return Qfalse;
    } else {
        return Qtrue;
    }
}

/*
 * Returns the instance of {Application}.
 * @return [Application]
 */
static VALUE qml_application(VALUE module) {
    if (NIL_P(rbqml_application)) {
        rb_raise(rb_eRuntimeError, "QML not yet initialized");
    }
    return rbqml_application;
}

/*
 * Returns the instance of {Engine}.
 * @return [Engine]
 */
static VALUE qml_engine(VALUE module) {
    if (NIL_P(rbqml_engine)) {
        rb_raise(rb_eRuntimeError, "QML not yet initialized");
    }
    return rbqml_engine;

}

void Init_qml(void)
{
    rbqml_mQML = rb_define_module("QML");

    rbqml_init_application();
    rbqml_init_engine();
    rbqml_init_component();
    rbqml_init_interface();
    rbqml_init_exporter();
    rbqml_init_js_object();
    rbqml_init_js_array();
    rbqml_init_js_function();
    rbqml_init_js_wrapper();
    rbqml_init_signal_emitter();
    rbqml_init_plugin_loader();
    rbqml_init_dispatcher();
    rbqml_init_meta_object();

    rb_define_module_function(rbqml_mQML, "initialized?", qml_initialized_p, 0);
    rb_define_module_function(rbqml_mQML, "init_impl", qml_init, 1);
    rb_define_module_function(rbqml_mQML, "application", qml_application, 0);
    rb_define_module_function(rbqml_mQML, "engine", qml_engine, 0);
}
