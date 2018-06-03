#pragma once

#include <memory>
#include "scope.h"
#include <string>

class BaseObject;
using ObjectPtr = std::shared_ptr<BaseObject>;

class Scope;
using ScopePtr = std::shared_ptr<Scope>;

class BaseObject {

public:

    virtual ~BaseObject() = default;
    virtual ObjectPtr Evaluate(ScopePtr scope) = 0;
    virtual ObjectPtr Duplicate() const = 0;
    virtual std::string Print() const = 0;

    virtual std::string Type() const = 0;

    virtual bool IsPair() const {
        return false;
    }

    virtual bool IsList() const {
        return false;
    }

    virtual bool IsNull() const {
        return false;
    }

    virtual bool IsBoolean() const {
        return false;
    }

    virtual bool IsTrue() const {
        return true;
    }

    virtual bool IsSymbol() const {
        return false;
    }

    virtual bool IsNumber() const {
        return false;
    }

    virtual bool IsFunction() const {
        return false;
    }

    virtual bool IsLambda() const {
        return false;
    }

    virtual bool IsQuoted() const {
        return false;
    }

    virtual bool IsString() const {
        return false;
    }

    virtual bool Printable() const {
        return true;
    }
};



