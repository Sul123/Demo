#include "string_object.h"

StringObject::StringObject(const std::string &string) : string_(string) {}

StringObject::StringObject(const StringObject &other) : string_(other.string_) {}

ObjectPtr StringObject::Duplicate() const {
    return std::make_shared<StringObject>(*this);
}

ObjectPtr StringObject::Evaluate(ScopePtr scope) {
    return shared_from_this();
}

std::string StringObject::Print() const {
    return "\"" + string_ + "\"";
}

bool StringObject::IsString() const {
    return true;
}

size_t StringObject::Size() const {
    return string_.size();
}

std::string StringObject::String() const {
    return string_;
}

char StringObject::GetChar(size_t index) const {
    return string_[index];
}

