#include "number_object.h"

NumberObject::NumberObject() = default;

NumberObject::NumberObject(const NumberObject& other) : number_(other.number_) {}

NumberObject::NumberObject(int64_t number) : number_(number) {}

std::string NumberObject::Print() const {
    return std::to_string(number_);
}


ObjectPtr NumberObject::Duplicate() const {
    return std::make_shared<NumberObject>(*this);
}

ObjectPtr NumberObject::Evaluate(ScopePtr scope) {
    return shared_from_this();
}

int64_t NumberObject::GetNumber() const {
    return number_;
}

bool NumberObject::IsNumber() const {
    return true;
}