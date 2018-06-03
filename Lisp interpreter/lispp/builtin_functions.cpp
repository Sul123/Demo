#include "builtin_functions.h"
#include <iostream>
#include <algorithm>
#include <string>
#include "number_object.h"
#include "errors.h"
#include "boolean_object.h"
#include "pair_object.h"
#include "null_object.h"
#include "string_object.h"
#include "tokenizer.h"
#include "parser.h"
#include "alias.h"

using ObjectPtr = std::shared_ptr<BaseObject>;


ObjectPtr BuiltIn::Plus(ArgVector arg_values) {
    int64_t result = 0;
    for (const auto &arg_ptr : arg_values) {
        RuntimeExpect(arg_ptr->IsNumber(), "Runtime error, + argument is not a number");

        result += ToNumber(arg_ptr)->GetNumber();
    }
    return MakeNumber(result);
}

ObjectPtr BuiltIn::Minus(ArgVector arg_values) {
    RuntimeExpect(!arg_values.empty(), "Runtime error, no arguments for -");

    int64_t result = 0;
    for (size_t ind = 0; ind < arg_values.size(); ++ind) {
        auto arg_ptr = arg_values[ind];

        RuntimeExpect(arg_ptr->IsNumber(), "Runtime error, - argument is not a number");

        int64_t sign = (arg_values.size() != 1 && ind == 0) ? 1 : -1;
        result += sign * ToNumber(arg_ptr)->GetNumber();
    }
    return MakeNumber(result);
}


ObjectPtr BuiltIn::Multiply(ArgVector arg_values) {
    int64_t result = 1;
    for (const auto &arg_ptr : arg_values) {
        RuntimeExpect(arg_ptr->IsNumber(), "Runtime error, * argument is not a number");

        result *= ToNumber(arg_ptr)->GetNumber();
    }
    return MakeNumber(result);
}

ObjectPtr BuiltIn::Divide(ArgVector arg_values) {
    int64_t result;
    RuntimeExpect(!arg_values.empty(), "Runtime error, no arguments for /");
    for (size_t ind = 0; ind < arg_values.size(); ++ind) {
        auto arg_ptr = arg_values[ind];
        RuntimeExpect(arg_ptr->IsNumber(), "Runtime error, / argument is not a number");
        int64_t cur_num = ToNumber(arg_ptr)->GetNumber();
        if (ind == 0) {
            result = cur_num;
        } else {
            RuntimeExpect(cur_num != 0, "You're not cool enough to divide by zero");
            result /= cur_num;
        }
    }
    return MakeNumber(result);
}

ObjectPtr BuiltIn::Equal(ArgVector arg_values) {
    if (arg_values.size() < 2) {
        return MakeBoolean(true);
    }
    for (size_t ind = 0; ind < arg_values.size() - 1; ++ind) {
        auto arg_ptr_1 = arg_values[ind];
        auto arg_ptr_2 = arg_values[ind + 1];

        RuntimeExpect(arg_ptr_1->IsNumber() && arg_ptr_2->IsNumber(),
                      "RE, = argument is not a number");

        int64_t cur_num_1 = ToNumber(arg_ptr_1)->GetNumber();
        int64_t cur_num_2 = ToNumber(arg_ptr_2)->GetNumber();
        if (cur_num_1 != cur_num_2) {
            return MakeBoolean(false);
        }
        if (ind == arg_values.size() - 2) {
            return MakeBoolean(true);
        }
    }
}

ObjectPtr BuiltIn::Greater(ArgVector arg_values) {
    if (arg_values.size() < 2) {
        return MakeBoolean(true);
    }
    for (size_t ind = 0; ind < arg_values.size() - 1; ++ind) {
        auto arg_ptr_1 = arg_values[ind];
        auto arg_ptr_2 = arg_values[ind + 1];

        RuntimeExpect(arg_ptr_1->IsNumber() && arg_ptr_2->IsNumber(),
                      "RE, > argument is not a number");

        int64_t cur_num_1 = ToNumber(arg_ptr_1)->GetNumber();
        int64_t cur_num_2 = ToNumber(arg_ptr_2)->GetNumber();
        if (cur_num_1 <= cur_num_2) {
            return MakeBoolean(false);
        }
        if (ind == arg_values.size() - 2) {
            return MakeBoolean(true);
        }
    }
}

ObjectPtr BuiltIn::Less(ArgVector arg_values) {
    if (arg_values.size() < 2) {
        return MakeBoolean(true);
    }
    for (size_t ind = 0; ind < arg_values.size() - 1; ++ind) {
        auto arg_ptr_1 = arg_values[ind];
        auto arg_ptr_2 = arg_values[ind + 1];

        RuntimeExpect(arg_ptr_1->IsNumber() && arg_ptr_2->IsNumber(),
                      "RE, < argument is not a number");

        int64_t cur_num_1 = ToNumber(arg_ptr_1)->GetNumber();
        int64_t cur_num_2 = ToNumber(arg_ptr_2)->GetNumber();
        if (cur_num_1 >= cur_num_2) {
            return MakeBoolean(false);
        }
        if (ind == arg_values.size() - 2) {
            return MakeBoolean(true);
        }
    }
}

ObjectPtr BuiltIn::Geq(ArgVector arg_values) {
    if (arg_values.size() < 2) {
        return MakeBoolean(true);
    }
    for (size_t ind = 0; ind < arg_values.size() - 1; ++ind) {
        auto arg_ptr_1 = arg_values[ind];
        auto arg_ptr_2 = arg_values[ind + 1];

        RuntimeExpect(arg_ptr_1->IsNumber() && arg_ptr_2->IsNumber(),
                      "RE, >= argument is not a number");

        int64_t cur_num_1 = ToNumber(arg_ptr_1)->GetNumber();
        int64_t cur_num_2 = ToNumber(arg_ptr_2)->GetNumber();
        if (cur_num_1 < cur_num_2) {
            return MakeBoolean(false);
        }
        if (ind == arg_values.size() - 2) {
            return MakeBoolean(true);
        }
    }
}

ObjectPtr BuiltIn::Leq(ArgVector arg_values) {
    if (arg_values.size() < 2) {
        return MakeBoolean(true);
    }
    for (size_t ind = 0; ind < arg_values.size() - 1; ++ind) {
        auto arg_ptr_1 = arg_values[ind];
        auto arg_ptr_2 = arg_values[ind + 1];

        RuntimeExpect(arg_ptr_1->IsNumber() && arg_ptr_2->IsNumber(),
                      "RE, <= argument is not a number");

        int64_t cur_num_1 = ToNumber(arg_ptr_1)->GetNumber();
        int64_t cur_num_2 = ToNumber(arg_ptr_2)->GetNumber();
        if (cur_num_1 > cur_num_2) {
            return MakeBoolean(false);
        }
        if (ind == arg_values.size() - 2) {
            return MakeBoolean(true);
        }
    }
}

ObjectPtr BuiltIn::Min(ArgVector arg_values) {
    RuntimeExpect(!arg_values.empty(), "RE, wrong number of arguments for min");

    int64_t min;
    for (size_t ind = 0; ind < arg_values.size(); ++ind) {
        auto arg_ptr = arg_values[ind];

        RuntimeExpect(arg_ptr->IsNumber(), "RE, min argument is not a number");

        int64_t cur_num = ToNumber(arg_ptr)->GetNumber();
        min = (ind == 0) ? cur_num : std::min(min, cur_num);
    }
    return MakeNumber(min);
}

ObjectPtr BuiltIn::Max(ArgVector arg_values) {
    RuntimeExpect(!arg_values.empty(), "RE, wrong number of arguments for max");

    int64_t max;
    for (size_t ind = 0; ind < arg_values.size(); ++ind) {
        auto arg_ptr = arg_values[ind];

        RuntimeExpect(arg_ptr->IsNumber(), "RE, max argument is not a number");

        int64_t cur_num = ToNumber(arg_ptr)->GetNumber();
        max = (ind == 0) ? cur_num : std::max(max, cur_num);
    }
    return MakeNumber(max);
}

ObjectPtr BuiltIn::Abs(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 1, "RE, wrong number of arguments for abs");

    auto arg_ptr = arg_values[0];

    RuntimeExpect(arg_ptr->IsNumber(), "RE, abs argument is not a number");

    int64_t cur_num = ToNumber(arg_ptr)->GetNumber();

    return MakeNumber(std::abs(cur_num));
}


ObjectPtr BuiltIn::Cons(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 2, "RE, wrong number of arguments for cons");

    return std::make_shared<PairObject>(arg_values[0], arg_values[1]);
}

ObjectPtr BuiltIn::Car(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 1, "RE, wrong number of arguments for car");
    RuntimeExpect(arg_values[0]->IsPair(), "RE, car argument isn't a pair");

    auto pair = ToPair(arg_values[0]);
    return pair->Car();
}

ObjectPtr BuiltIn::Cdr(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 1, "RE, wrong number of arguments for cdr");
    RuntimeExpect(arg_values[0]->IsPair(), "RE, cdr argument isn't a pair");

    auto pair = ToPair(arg_values[0]);
    return pair->Cdr();
}

ObjectPtr BuiltIn::SetCar(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 2, "RE, wrong number of arguments for set-car!");
    RuntimeExpect(arg_values[0]->IsPair(), "RE, set-car! first argument isn't a pair");

    auto pair = ToPair(arg_values[0]);
    pair->SetCar(arg_values[1]);
    return std::make_shared<NullObject>(false);
}

ObjectPtr BuiltIn::SetCdr(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 2, "RE, wrong number of arguments for set-cdr!");
    RuntimeExpect(arg_values[0]->IsPair(), "RE, set-cdr! first argument isn't a pair");

    auto pair = ToPair(arg_values[0]);
    pair->SetCdr(arg_values[1]);
    return std::make_shared<NullObject>(false);
}

ObjectPtr BuiltIn::List(ArgVector arg_values) {
    auto tail = std::make_shared<NullObject>();
    if (arg_values.empty()) {
        return tail;
    }
    size_t ind = arg_values.size();
    ObjectPtr list;
    while (ind > 0) {
        --ind;
        list = (ind == arg_values.size() - 1) ? std::make_shared<PairObject>(arg_values[ind], tail)
                                              : std::make_shared<PairObject>(arg_values[ind], list);
    }
    return list;
}

ObjectPtr BuiltIn::ListRef(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 2, "RE, wrong syntax for list-ref");
    RuntimeExpect(arg_values[0]->IsList(), "RE, first list-ref argument must be a list");

    auto list = ToPair(arg_values[0]);
    RuntimeExpect(arg_values[1]->IsNumber(), "RE, second list-ref argument must be a number");

    int64_t index = ToNumber(arg_values[1])->GetNumber();
    RuntimeExpect(index >=0 && index < list->ListSize(), "RE, list index out of range");

    return list->ListGet(static_cast<size_t>(index));
}

ObjectPtr BuiltIn::ListTail(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 2, "RE, wrong syntax for list-tail");
    RuntimeExpect(arg_values[0]->IsList(), "RE, first list-tail argument must be a list");

    auto list = ToPair(arg_values[0]);
    RuntimeExpect(arg_values[1]->IsNumber(), "RE, second list-tail argument must be a number");

    int64_t index = ToNumber(arg_values[1])->GetNumber();
    RuntimeExpect(index >=0 && index < list->ListSize(), "RE, tail start is out of range");

    if (index == 0) {
        return list;
    }
    size_t counter = 1;
    while (counter < index) {
        list = ToPair(list->Cdr());
        ++counter;
    }
    return list->Cdr();
}

ObjectPtr BuiltIn::IsNull(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 1, "RE, wrong arguments number for null? predicate");

    return MakeBoolean(arg_values[0]->IsNull());
}

ObjectPtr BuiltIn::IsPair(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 1, "RE, wrong arguments number for pair? predicate");

    return MakeBoolean(arg_values[0]->IsPair());
}

ObjectPtr BuiltIn::IsNumber(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 1, "RE, wrong arguments number for number? predicate");

    return MakeBoolean(arg_values[0]->IsNumber());
}

ObjectPtr BuiltIn::IsBoolean(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 1, "RE, wrong arguments number for boolean? predicate");

    return MakeBoolean(arg_values[0]->IsBoolean());
}

ObjectPtr BuiltIn::IsSymbol(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 1, "RE, wrong arguments number for symbol? predicate");

    return MakeBoolean(arg_values[0]->IsSymbol());
}

ObjectPtr BuiltIn::IsList(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 1, "RE, wrong arguments number for list? predicate");

    return MakeBoolean(arg_values[0]->IsList());
}

ObjectPtr BuiltIn::Not(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 1, "RE, wrong arguments number for not");

    return MakeBoolean(!arg_values[0]->IsTrue());
}

ObjectPtr BuiltIn::Concat(ArgVector arg_values) {
    std::string result;
    for (const auto& arg_ptr : arg_values) {
        RuntimeExpect(arg_ptr->IsString(), "RE, concat arguments must be strings only");

        result += ToString(arg_ptr)->String();
    }
    return MakeString(result);
}

ObjectPtr BuiltIn::Read(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 1, "RE, wrong number of arguments to read");
    RuntimeExpect(arg_values[0]->IsString(), "RE, read argument must be string");

    std::string expression = ToString(arg_values[0])->String();
    std::stringstream ss(expression);
    Tokenizer tok(&ss);
    return Parse(&tok);
}

ObjectPtr BuiltIn::StringSize(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 1, "RE, expected one and only one argument for string-size");
    RuntimeExpect(arg_values[0]->IsString(), "RE, string-size argument must be string");

    int64_t size = ToString(arg_values[0])->Size();
    return MakeNumber(size);
}

ObjectPtr BuiltIn::GetChar(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 2, "RE, wrong number of arguments for get-char");
    RuntimeExpect(arg_values[0]->IsString(), "RE, first get-char argument must be string");
    RuntimeExpect(arg_values[1]->IsNumber(), "RE, second get-char argument must be number");

    auto string_obj_ptr = ToString(arg_values[0]);
    int64_t index = ToNumber(arg_values[1])->GetNumber();
    RuntimeExpect(index >=0 && index < string_obj_ptr->Size(), "RE, string index out of range");

    std::string character;
    character += string_obj_ptr->GetChar(static_cast<size_t>(index));
    return MakeString(character);
}

ObjectPtr BuiltIn::ConvertToString(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 1, "RE, wrong number of arguments for to-string");

    if (arg_values[0]->IsString()) {
        return arg_values[0];
    }
    return MakeString(arg_values[0]->Print());
}

ObjectPtr BuiltIn::Type(ArgVector arg_values) {
    RuntimeExpect(arg_values.size() == 1, "RE, wrong number of arguments for type");

    return MakeString(arg_values[0]->Type());
}
