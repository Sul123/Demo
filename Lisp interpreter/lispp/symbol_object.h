#pragma once
#include "base_object.h"


class SymbolObject : public BaseObject {
public:
    SymbolObject(const SymbolObject& other);

    SymbolObject(const std::string& symbol);

    ObjectPtr Duplicate() const override;

    ObjectPtr Evaluate(ScopePtr scope) override;

    std::string Print() const override;

    std::string Type() const override {
        return "symbol";
    }

    bool IsSymbol() const override;
    const std::string& GetSymbol() const;

private:
    std::string symbol_;
};