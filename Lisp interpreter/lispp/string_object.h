#pragma once
#include "base_object.h"

class StringObject : public BaseObject, public std::enable_shared_from_this<StringObject> {
public:
    explicit StringObject(const std::string& string);

    StringObject(const StringObject& other);

    ObjectPtr Duplicate() const override;
    ObjectPtr Evaluate(ScopePtr scope) override;

    std::string Print() const override;

    bool IsString() const override;

    size_t Size() const;

    char GetChar(size_t index) const;

    std::string String() const;

    std::string Type() const override {
        return "string";
    }

private:
    std::string string_;
};
