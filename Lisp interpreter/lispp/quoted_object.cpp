#include "quoted_object.h"
#include "errors.h"

QuotedObject::QuotedObject(const ObjectPtr& unquoted_obj_ptr) {
    if (!unquoted_obj_ptr) {
        throw RuntimeError("Runtime error, can't quote nothing");
    } else {
        object_ptr_ = unquoted_obj_ptr->Duplicate();
    }
}

QuotedObject::QuotedObject(const QuotedObject& other) {
    if (!other.object_ptr_) {
        object_ptr_ = nullptr;
    } else {
        object_ptr_ = other.object_ptr_->Duplicate();
    }
}

ObjectPtr QuotedObject::Duplicate() const {
    return std::make_shared<QuotedObject>(*this);
}

ObjectPtr QuotedObject::Evaluate(ScopePtr scope) {
    return object_ptr_;
}

std::string QuotedObject::Print() const {

    return object_ptr_->Print();
}

bool QuotedObject::IsQuoted() const {
    return true;
}