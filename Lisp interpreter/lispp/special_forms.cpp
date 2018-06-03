#include "special_forms.h"
#include "quoted_object.h"
#include "errors.h"
#include "pair_object.h"
#include "symbol_object.h"
#include "function_object.h"
#include "boolean_object.h"
#include "null_object.h"
#include "alias.h"


ObjectPtr Special::Quote(ObjectPtr arg) {
    SyntaxExpect(arg->IsList() && !arg->IsNull(), "Syntax error, wrong Quote syntax");

    std::shared_ptr<PairObject> list = ToPair(arg);
    SyntaxExpect(list->ListSize() == 1, "Syntax error, wrong number of arguments to quote");

    return MakeQuoted(list->Car());
}

ObjectPtr Special::If(ObjectPtr arg, ScopePtr scope) {
    RuntimeExpect(arg->IsList() && !arg->IsNull(), "Runtime error, wrong If argument");

    std::shared_ptr<PairObject> list = ToPair(arg);
    if (list->ListSize() == 2) {
        if (list->ListGet(0)->Evaluate(scope)->IsTrue()) {
            return list->ListGet(1)->Evaluate(scope);
        }
        return std::make_shared<NullObject>();
    }
    if (list->ListSize() == 3) {
        if (list->ListGet(0)->Evaluate(scope)->IsTrue()) {
            return list->ListGet(1)->Evaluate(scope);
        }
        return list->ListGet(2)->Evaluate(scope);
    }
    throw RuntimeError("Runtime error, wrong number for If arguments");
}

ObjectPtr Special::Lambda(ObjectPtr arg, ScopePtr scope) {
    RuntimeExpect(arg->IsList() && !arg->IsNull(), "Runtime error, wrong Lambda construction");

    std::shared_ptr<PairObject> list = ToPair(arg);
    RuntimeExpect(list->ListSize() >= 2, "Runtime error, wrong Lambda construction");
    RuntimeExpect(list->ListGet(0)->IsList(), "Runtime error, wrong Lambda construction");

    std::vector<std::string> arguments;
    if (!list->ListGet(0)->IsNull()) {
        auto arg_list_ptr = ToPair(list->ListGet(0));
        std::vector<ObjectPtr> extracted_arg_list = arg_list_ptr->ExtractFromList();
        for (const auto& object_ptr : extracted_arg_list) {
            RuntimeExpect(object_ptr->IsSymbol(),
                          "RE, wrong lambda construction, arguments must be symbols");
            arguments.push_back(ToSymbol(object_ptr)->GetSymbol());
        }
    }
    auto body_ptr = ToPair(list->Cdr());

    return MakeLambda(arguments, body_ptr, scope);
}

ObjectPtr Special::And(ObjectPtr arg, ScopePtr scope) {
    RuntimeExpect(arg->IsList(), "Runtime error, wrong And syntax");

    if (arg->IsNull()) {
        return MakeBoolean(true);
    }
    auto extracted_arg_vector = ToPair(arg)->ExtractFromList();
    for (auto it = extracted_arg_vector.begin(); it != extracted_arg_vector.end(); ++it) {
        auto value_ptr = (*it)->Evaluate(scope);
        if (!value_ptr->IsTrue()) {
            return value_ptr;
        }
        if (std::next(it) == extracted_arg_vector.end()) {
            return value_ptr;
        }
    }
}

ObjectPtr Special::Or(ObjectPtr arg, ScopePtr scope) {
    RuntimeExpect(arg->IsList(), "Runtime error, wrong Or syntax");

    if (arg->IsNull()) {
        return MakeBoolean(false);
    }
    auto extracted_arg_vector = ToPair(arg)->ExtractFromList();
    for (auto it = extracted_arg_vector.begin(); it != extracted_arg_vector.end(); ++it) {
        auto value_ptr = (*it)->Evaluate(scope);
        if (value_ptr->IsTrue()) {
            return value_ptr;
        }
        if (std::next(it) == extracted_arg_vector.end()) {
            return value_ptr;
        }
    }
}

ObjectPtr DefineVariable(std::shared_ptr<PairObject> define_body, ScopePtr scope) {
    RuntimeExpect(define_body->ListSize() == 2, "RE, wrong define syntax");

    std::string name = ToSymbol(define_body->Car())->GetSymbol();
    auto value = define_body->ListGet(1)->Evaluate(scope);
    scope->Define(name, value);
    return std::make_shared<NullObject>(false);
}

ObjectPtr DefineFunction(std::shared_ptr<PairObject> define_body, ScopePtr scope) {
    auto left_part = ToPair(define_body->Car());
    RuntimeExpect(left_part->Car()->IsSymbol(), "Runtime error, wrong define syntax");
    std::string name = ToSymbol(left_part->Car())->GetSymbol();

    std::vector<std::string> arguments;
    if (!left_part->Cdr()->IsNull()) {
        auto arg_list_ptr = ToPair(left_part->Cdr());
        std::vector<ObjectPtr> extracted_arg_list = arg_list_ptr->ExtractFromList();
        for (const auto& object_ptr : extracted_arg_list) {
            RuntimeExpect(object_ptr->IsSymbol(),
                          "RE, wrong function define construction, arguments must be symbols");
            arguments.push_back(ToSymbol(object_ptr)->GetSymbol());
        }
    }

    auto function_body_ptr = ToPair(define_body->Cdr());
    scope->Define(name, MakeLambda(arguments, function_body_ptr, scope));
    return std::make_shared<NullObject>(false);
}

ObjectPtr Special::Define(ObjectPtr arg, ScopePtr scope) {
    RuntimeExpect(arg->IsList() && !arg->IsNull() && ToPair(arg)->ListSize() >= 2,
                  "Runtime error, wrong define syntax");

    auto define_body = ToPair(arg);
    if (define_body->Car()->IsSymbol()) {
        return DefineVariable(define_body, scope);
    }

    // if function is defined
    if (define_body->Car()->IsList()) {
        return DefineFunction(define_body, scope);
    }
    throw RuntimeError("Runtime error, wrong define syntax");
}

ObjectPtr Special::Set(ObjectPtr arg, ScopePtr scope) {
    RuntimeExpect(arg->IsList() && !arg->IsNull() && ToPair(arg)->ListSize() >= 2,
                  "Runtime error, wrong set! syntax");

    auto set_body = ToPair(arg);
    if (set_body->Car()->IsSymbol()) {
        RuntimeExpect(set_body->ListSize() == 2, "RE, wrong set! syntax");

        std::string name = ToSymbol(set_body->Car())->GetSymbol();
        auto value = set_body->ListGet(1)->Evaluate(scope);
        scope->Set(name, value);
        return std::make_shared<NullObject>(false);
    }

    throw RuntimeError("Runtime error, wrong set! syntax");
}

ObjectPtr Special::Eval(ObjectPtr arg, ScopePtr scope) {
    RuntimeExpect(arg->IsList() && !arg->IsNull() && ToPair(arg)->ListSize() == 1,
                  "RE, wrong eval syntax");

    auto eval_argument_ptr = ToPair(arg)->Car()->Evaluate(scope);

    return eval_argument_ptr->Evaluate(scope);
}
