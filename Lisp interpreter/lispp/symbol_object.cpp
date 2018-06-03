#include "symbol_object.h"
#include "scope.h"
#include "errors.h"
#include <string>


SymbolObject::SymbolObject(const SymbolObject& other) : symbol_(other.symbol_) {}

SymbolObject::SymbolObject(const std::string& symbol) : symbol_(symbol) {}

ObjectPtr SymbolObject::Duplicate() const  {
    return std::make_shared<SymbolObject>(*this);
}

ObjectPtr SymbolObject::Evaluate(ScopePtr scope)  {
    ObjectPtr object = scope->Get(symbol_);
    if (!object) {
        throw NameError("Name error, " + symbol_ + " not defined");
    }
    return object;
}

std::string SymbolObject::Print() const  {
    return symbol_;
}

bool SymbolObject::IsSymbol() const  {
    return true;
}

const std::string& SymbolObject::GetSymbol() const {
    return symbol_;
}

