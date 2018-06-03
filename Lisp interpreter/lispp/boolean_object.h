#include "base_object.h"
#include <string>

#pragma once



class BooleanObject : public BaseObject, public std::enable_shared_from_this<BooleanObject> {
public:
    BooleanObject();

    explicit BooleanObject(bool value);

    BooleanObject(const BooleanObject& other);

    BooleanObject(const std::string& boolean_symbol);

    std::string Print() const override;

    ObjectPtr Evaluate(ScopePtr scope) override;

    ObjectPtr Duplicate() const override;

    bool IsBoolean() const override;

    bool IsTrue() const override;

    std::string Type() const override {
        return "boolean";
    }


private:
    bool value_;
};