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

VALUE rbqml_mQML;
VALUE rbqml_application = Qnil;
VALUE rbqml_engine = Qnil;

static VALUE qml_init(VALUE module, VALUE args) {
    if (!NIL_P(rbqml_application)) {
        rb_raise(rb_eRuntimeError, "QML already initialized");
    }
    rbqml_application = rb_funcall(rbqml_cApplication, rb_intern("new"), 1, args);
    rbqml_engine = rb_funcall(rbqml_cEngine, rb_intern("new"), 0);
    return module;
}

static VALUE qml_application(VALUE module) {
    if (NIL_P(rbqml_application)) {
        rb_raise(rb_eRuntimeError, "QML not yet initialized");
    }
    return rbqml_application;
}

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

    rb_define_module_function(rbqml_mQML, "init", &qml_init, 1);
    rb_define_module_function(rbqml_mQML, "application", &qml_application, 0);
    rb_define_module_function(rbqml_mQML, "engine", &qml_engine, 0);
}