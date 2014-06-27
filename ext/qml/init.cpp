#include "ext_metaobject.h"
#include "ext_pointer.h"
#include "ext_pluginloader.h"
#include "ext_gcmarker.h"
#include "ext_accesssupport.h"
#include "ext_testutil.h"
#include "ext_kernel.h"
#include "application.h"
#include "signalforwarder.h"
#include "valuereference.h"
#include "objectgc.h"
#include "rubyclasses.h"
#include <QtCore/QSet>

using namespace RubyQml;

namespace {

void defineMetaTypes()
{
    qRegisterMetaType<const QMetaObject *>();
}

void defineClasses()
{
    Ext::MetaObject::defineClass();
    Ext::Pointer::defineClass();
    Ext::PluginLoader::defineClass();
    Ext::GCMarker::defineClass();
    Ext::AccessSupport::defineClass();
    Ext::TestUtil::defineModule();
    Ext::Kernel::defineModule();
    RubyClasses::initialize();
}

void setupGlobalGCMarking()
{
    auto marker = Ext::GCMarker::fromMarkFunction([] {
        ValueReference::markAllReferences();
        ObjectGC::instance()->markNonOwnedObjects();
    });
    rb_gc_register_mark_object(marker);
}

void cleanup()
{
    ObjectGC::cleanUp();
}

void setupEndProc()
{
    rb_set_end_proc([](VALUE) { unprotect(cleanup); }, Qnil);
}

}

extern "C"
void Init_qml()
{
    rb_require("qml/errors");
    rb_require("qml/error_converter");
    auto args = RubyValue(rb_argv).to<QList<QByteArray>>();

    protect([&] {
        Application::init(args);
        defineMetaTypes();
        defineClasses();
        setupGlobalGCMarking();
        setupEndProc();
    });
}
