#include "base_object.h"
#pragma once

class QuotedObject : public BaseObject {
public:
    QuotedObject(const ObjectPtr& unquoted_obj_ptr);
    QuotedObject(const QuotedObject& other);

    ObjectPtr Duplicate() const override;

    ObjectPtr Evaluate(ScopePtr scope) override;
    std::string Print() const override;
    bool IsQuoted() const override;

    std::string Type() const override {
        return "quoted";
    }

private:
    ObjectPtr object_ptr_;
};