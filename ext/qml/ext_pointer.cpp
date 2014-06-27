#include "ext_pointer.h"
#include "markable.h"
#include "objectdata.h"
#include "objectgc.h"
#include "application.h"
#include <QObject>
#include <QHash>
#include <QQmlEngine>
#include <QQmlContext>
#include <QDebug>
#include <QCoreApplication>

namespace RubyQml {
namespace Ext {

Pointer::Pointer(RubyValue self) :
    self(self)
{
}

Pointer::~Pointer()
{
    ObjectGC::instance()->debug() << "\u267b releasing object:" << mObject << "owned:" << mIsOwned;
}

RubyValue Pointer::fromQObject(QObject *obj, bool owned)
{
    auto klass = wrapperRubyClass<Pointer>();
    auto ptr = klass.newInstance();
    klass.unwrap(ptr)->setQObject(obj, owned);
    return ptr;
}

QObject *Pointer::fetchQObject()
{
    if (!mObject) {
        fail("QML::QtObjectError", "referencing already deleted Qt Object");
    }
    return mObject;
}

void Pointer::setQObject(QObject *obj, bool owned)
{
    if (!obj) {
        throw std::logic_error("null object");
    }
    mObject = obj;
    ObjectGC::instance()->addObject(obj);

    if (QQmlEngine::objectOwnership(obj) == QQmlEngine::JavaScriptOwnership) {
        owned = true;
    }
    ObjectGC::instance()->debug() << "\u2728 acquiring object:" << obj << "owned:" << owned;
    setOwned(owned);
}

void Pointer::setOwned(bool owned)
{
    if (mObject) {
        if (owned) {
            mJSValue = Application::engine()->newQObject(mObject);
        } else {
            mJSValue = QJSValue();
            QQmlEngine::setObjectOwnership(mObject, QQmlEngine::CppOwnership);
        }

        ObjectData::getOrCreate(mObject)->owned = owned;
        mIsOwned = owned;
    }
}

RubyValue Pointer::ext_isOwned() const
{
    return RubyValue::from(mIsOwned);
}

RubyValue Pointer::ext_setOwned(RubyValue owned)
{
    setOwned(owned.to<bool>());
    return ext_isOwned();
}

RubyValue Pointer::ext_isNull() const
{
    return RubyValue::from(!mObject);
}

RubyValue Pointer::ext_toString() const
{
    QString name;
    QDebug(&name) << mObject.data();
    return RubyValue::from(name);
}

void Pointer::gc_mark()
{
    if (mIsOwned) {
        ObjectGC::instance()->markOwnedObject(mObject);
    }
}

void Pointer::defineClass()
{
    WrapperRubyClass<Pointer> klass("QML", "Pointer");
    klass.defineMethod("owned?", RUBYQML_MEMBER_FUNCTION_INFO(&Pointer::ext_isOwned));
    klass.defineMethod("owned=", RUBYQML_MEMBER_FUNCTION_INFO(&Pointer::ext_setOwned));
    klass.defineMethod("null?", RUBYQML_MEMBER_FUNCTION_INFO(&Pointer::ext_isNull));
    klass.defineMethod("to_s", RUBYQML_MEMBER_FUNCTION_INFO(&Pointer::ext_toString));
    klass.aliasMethod("to_s", "inspect");
}

} // namespace Ext
} // namespace RubyQml
