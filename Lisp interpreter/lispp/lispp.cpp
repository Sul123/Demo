#include "lispp.h"
#include "scope.h"
#include <iostream>
#include "tokenizer.h"
#include "parser.h"
#include <sstream>
#include <string>

#include "errors.h"


Interpreter::Interpreter() : global_scope_(std::make_shared<Scope>(MakeGlobalScope())) {}

Interpreter::~Interpreter() {
    global_scope_->Reset();
}

void Interpreter::Repl() {
    std::cout << "Welcome to Lispp interpeter!\n";
    while (true) {
        try {
            ObjectPtr result;

            std::cout << "> ";
            std::string s;
            std::getline(std::cin, s);
            std::stringstream ss(s);
            Tokenizer tok(&ss);
            ObjectPtr expression = Parse(&tok);

            result = expression->Evaluate(global_scope_);

            if (result->Printable()) {
                std::cout << result->Print() << "\n";
            }
        } catch (const LispError& error) {
            std::cout << error.what() << "\n";
        }
    }
}

std::string Interpreter::Test(const std::string& s) {
    std::stringstream ss(s);
    Tokenizer tok(&ss);
    ObjectPtr expression = Parse(&tok);

    ObjectPtr result = expression->Evaluate(global_scope_);
    std::string print;
    if (result->Printable()) {
        print = result->Print();
    }
    return print;
}