#include "function_object.h"
#include "errors.h"
#include "alias.h"

bool FunctionObject::IsFunction() const {
    return true;
}

/////////*************Lambda Implementation **********//////////
LambdaFunction::LambdaFunction(const std::vector<std::string> &args,
               const std::shared_ptr<PairObject>& body_ptr,
               ScopePtr outer_scope)
        : args_(args), body_(*body_ptr), outer_scope_(outer_scope) {}

LambdaFunction::LambdaFunction(const LambdaFunction &other)
        : args_(other.args_), body_(other.body_), outer_scope_(other.outer_scope_) {}

ObjectPtr LambdaFunction::Evaluate(ScopePtr scope) {
    throw RuntimeError("Runtime error, can't evaluate function");
}

bool LambdaFunction::IsLambda() const {
    return true;
}

ScopePtr LambdaFunction::ParentScope() {
    return outer_scope_;
}

ObjectPtr LambdaFunction::Apply(const std::vector<ObjectPtr>& arg_values) {

    auto function_scope = std::make_shared<Scope>(args_, arg_values, outer_scope_);
    PairObject body_part = body_;

    while (!(body_part.Cdr()->IsNull())) {
        body_part.Car()->Evaluate(function_scope);
        RuntimeExpect(body_part.Cdr()->IsPair(), "RE, incorrect function body");

        body_part = *(ToPair(body_part.Cdr()));
    }
    return body_part.Car()->Evaluate(function_scope);
}

ObjectPtr LambdaFunction::Duplicate() const {
    return std::make_shared<LambdaFunction>(*this);
}

std::string LambdaFunction::Print() const {
    return "#<function>";
}

////////////************BuiltIn Implementation****************/////////////

BuiltInFunction::BuiltInFunction(FuncType function) {
    builtin_function_ = function;
}

ObjectPtr BuiltInFunction::Apply(
        const std::vector<ObjectPtr>& arg_values) {
    return builtin_function_(arg_values);
}

ObjectPtr BuiltInFunction::Evaluate(ScopePtr scope) {
    throw RuntimeError("Runtime error, can't evaluate function");
}

ObjectPtr BuiltInFunction::Duplicate() const {
    return std::make_shared<BuiltInFunction>(*this);
}

std::string BuiltInFunction::Print() const {
    return "#<function>";
}
