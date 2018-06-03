#include "scope.h"
#include "function_object.h"
#include "errors.h"

Scope::Scope() = default;

Scope::Scope(ScopePtr parent_scope) : parent_scope_(parent_scope) {}

Scope::Scope(const std::vector<std::string>& args,
             const std::vector<ObjectPtr>& arg_values,
             ScopePtr outer_scope) : parent_scope_(outer_scope) {
    if (args.size() != arg_values.size()) {
        throw RuntimeError("Function scope build failed, wrong number of arguments");
    }
    if (!args.empty()) {
        for (size_t ind = 0; ind < args.size(); ++ind) {
            name_to_obj_[args[ind]] = arg_values[ind];
        }
    }
}

Scope::Scope(const Scope& other)
        : name_to_obj_(other.name_to_obj_), parent_scope_(other.parent_scope_) {}

ObjectPtr Scope::Get(const std::string& name) const {
    if (name_to_obj_.find(name) == name_to_obj_.end()) {
        if (!parent_scope_) {
            return nullptr;
        }
        return parent_scope_->Get(name);
    }
    return name_to_obj_.at(name);
}

void Scope::Define(const std::string& name, ObjectPtr object) {
    name_to_obj_[name] = object;
}

void Scope::Set(const std::string& name, ObjectPtr object) {
    if (name_to_obj_.find(name) == name_to_obj_.end()) {
        if (!parent_scope_) {
            throw NameError("name error, no name to change in scope pyramide");
        }
        parent_scope_->Set(name, object);
    }
    name_to_obj_[name] = object;
}

Scope::~Scope() = default;

void Scope::Reset() {
    for (auto& item : name_to_obj_) {
        item.second.reset();
    }
}


Scope MakeGlobalScope() {

    Scope global_scope;
    global_scope.Define("+", std::make_shared<BuiltInFunction>(BuiltIn::Plus));
    global_scope.Define("-", std::make_shared<BuiltInFunction>(BuiltIn::Minus));
    global_scope.Define("*", std::make_shared<BuiltInFunction>(BuiltIn::Multiply));
    global_scope.Define("/", std::make_shared<BuiltInFunction>(BuiltIn::Divide));
    global_scope.Define("=", std::make_shared<BuiltInFunction>(BuiltIn::Equal));
    global_scope.Define(">", std::make_shared<BuiltInFunction>(BuiltIn::Greater));
    global_scope.Define("<", std::make_shared<BuiltInFunction>(BuiltIn::Less));
    global_scope.Define(">=", std::make_shared<BuiltInFunction>(BuiltIn::Geq));
    global_scope.Define("<=", std::make_shared<BuiltInFunction>(BuiltIn::Leq));
    global_scope.Define("min", std::make_shared<BuiltInFunction>(BuiltIn::Min));
    global_scope.Define("max", std::make_shared<BuiltInFunction>(BuiltIn::Max));
    global_scope.Define("abs", std::make_shared<BuiltInFunction>(BuiltIn::Abs));

    global_scope.Define("cons", std::make_shared<BuiltInFunction>(BuiltIn::Cons));
    global_scope.Define("car", std::make_shared<BuiltInFunction>(BuiltIn::Car));
    global_scope.Define("cdr", std::make_shared<BuiltInFunction>(BuiltIn::Cdr));
    global_scope.Define("set-car!", std::make_shared<BuiltInFunction>(BuiltIn::SetCar));
    global_scope.Define("set-cdr!", std::make_shared<BuiltInFunction>(BuiltIn::SetCdr));
    global_scope.Define("list", std::make_shared<BuiltInFunction>(BuiltIn::List));
    global_scope.Define("list-ref", std::make_shared<BuiltInFunction>(BuiltIn::ListRef));
    global_scope.Define("list-tail", std::make_shared<BuiltInFunction>(BuiltIn::ListTail));

    global_scope.Define("null?", std::make_shared<BuiltInFunction>(BuiltIn::IsNull));
    global_scope.Define("pair?", std::make_shared<BuiltInFunction>(BuiltIn::IsPair));
    global_scope.Define("number?", std::make_shared<BuiltInFunction>(BuiltIn::IsNumber));
    global_scope.Define("boolean?", std::make_shared<BuiltInFunction>(BuiltIn::IsBoolean));
    global_scope.Define("list?", std::make_shared<BuiltInFunction>(BuiltIn::IsList));
    global_scope.Define("symbol?", std::make_shared<BuiltInFunction>(BuiltIn::IsSymbol));

    global_scope.Define("not", std::make_shared<BuiltInFunction>(BuiltIn::Not));

    global_scope.Define("concat", std::make_shared<BuiltInFunction>(BuiltIn::Concat));
    global_scope.Define("read", std::make_shared<BuiltInFunction>(BuiltIn::Read));
    global_scope.Define("string-size", std::make_shared<BuiltInFunction>(BuiltIn::StringSize));
    global_scope.Define("get-char", std::make_shared<BuiltInFunction>(BuiltIn::GetChar));
    global_scope.Define("to-string", std::make_shared<BuiltInFunction>(BuiltIn::ConvertToString));

    global_scope.Define("type", std::make_shared<BuiltInFunction>(BuiltIn::Type));


    return global_scope;
}