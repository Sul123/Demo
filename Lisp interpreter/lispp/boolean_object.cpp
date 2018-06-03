#include "boolean_object.h"
#include "errors.h"
#include <string>

BooleanObject::BooleanObject() = default;

BooleanObject::BooleanObject(bool value) : value_{value} {}

BooleanObject::BooleanObject(const BooleanObject& other) : value_{other.value_} {}

BooleanObject::BooleanObject(const std::string& boolean_symbol) {
    if (boolean_symbol == "#t") {
        value_ = true;
    } else if (boolean_symbol == "#f") {
        value_ = false;
    } else {
        throw SyntaxError("Not a boolean symbol");
    }
}


std::string BooleanObject::Print() const {
    return (value_) ? "#t" : "#f";
}

ObjectPtr BooleanObject::Evaluate(ScopePtr scope) {
    return shared_from_this();
}

ObjectPtr BooleanObject::Duplicate() const {
    return std::make_shared<BooleanObject>(*this);
}

bool BooleanObject::IsBoolean() const {
    return true;
}

bool BooleanObject::IsTrue() const {
    return value_;
}

