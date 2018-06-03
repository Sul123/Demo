#include "null_object.h"
#include "errors.h"

NullObject::NullObject() = default;

NullObject::NullObject(bool printable) : printable_(printable) {}

ObjectPtr NullObject::Evaluate(ScopePtr scope) {
    throw RuntimeError("can't evaluate nullptr");
}

ObjectPtr NullObject::Duplicate() const {
    return std::make_shared<NullObject>();
}

std::string NullObject::Print() const {
    return "()";
}

bool NullObject::Printable() const {
    return printable_;
}

bool NullObject::IsList() const {
    return true;
}

bool NullObject::IsNull() const {
    return true;
}