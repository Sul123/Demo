#pragma once
#include "base_object.h"
#include <iostream>
#include <memory>

class NumberObject : public BaseObject, public std::enable_shared_from_this<NumberObject> {
public:
    NumberObject();

    NumberObject(const NumberObject& other);

    explicit NumberObject(int64_t number);

    std::string Print() const override;

    std::string Type() const override {
        return "number";
    }


    ObjectPtr Duplicate() const override;

    ObjectPtr Evaluate(ScopePtr scope)  override;

    int64_t GetNumber() const;

    bool IsNumber() const override;

private:
    int64_t number_;
};