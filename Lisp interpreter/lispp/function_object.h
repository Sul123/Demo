#pragma once

#include <vector>
#include "base_object.h"
#include "pair_object.h"
#include "scope.h"
#include "builtin_functions.h"
#include <functional>

class BaseObject;
class FunctionObject;
class Scope;

class FunctionObject : public BaseObject {
public:
    virtual ~FunctionObject() = default;

    virtual ObjectPtr Apply(const std::vector<ObjectPtr>& arg_values) = 0;

    bool IsFunction() const override;

    std::string Type() const override {
        return "function";
    }

};

class LambdaFunction : public FunctionObject {
public:
    LambdaFunction(const std::vector<std::string> &args,
                   const std::shared_ptr<PairObject> &body_ptr,
                   ScopePtr outer_scope);

    explicit LambdaFunction(const LambdaFunction &other);

    ObjectPtr Evaluate(ScopePtr scope) override;

    ObjectPtr Apply(
            const std::vector<ObjectPtr>& arg_values) override;

    ObjectPtr Duplicate() const override;

    std::string Print() const override;

    bool IsLambda() const override;

    ScopePtr ParentScope();

private:
    std::vector<std::string> args_;
    PairObject body_;
    ScopePtr outer_scope_;
};

using FuncType = ObjectPtr(const std::vector<ObjectPtr>&);

class BuiltInFunction : public FunctionObject {
public:

    explicit BuiltInFunction(FuncType function);

    ObjectPtr Apply(const std::vector<ObjectPtr>& arg_values) override;

    ObjectPtr Evaluate(ScopePtr scope) override;

    ObjectPtr Duplicate() const override;

    std::string Print() const override;

private:
    std::function<FuncType> builtin_function_;
};