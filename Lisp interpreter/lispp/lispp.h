#pragma once
#include <memory>
#include <string>
#include "scope.h"


class Scope;

class Interpreter {
public:
    Interpreter();

    ~Interpreter();

    void Repl();

    std::string Test(const std::string& s);

private:
    ScopePtr global_scope_;
};
