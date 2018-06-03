#pragma once
#include "base_object.h"
#include "number_object.h"
#include "pair_object.h"
#include "string_object.h"
#include "number_object.h"
#include "boolean_object.h"
#include "function_object.h"
#include "quoted_object.h"
#include "symbol_object.h"

auto ToNumber = [&] (ObjectPtr arg) { return std::dynamic_pointer_cast<NumberObject>(arg); };
auto ToPair = [&] (ObjectPtr arg) { return std::dynamic_pointer_cast<PairObject>(arg); };
auto ToString = [&] (ObjectPtr arg) { return std::dynamic_pointer_cast<StringObject>(arg); };
auto ToSymbol = [&] (ObjectPtr arg) { return std::dynamic_pointer_cast<SymbolObject>(arg); };


auto MakeNumber = [&] (int64_t num) { return std::make_shared<NumberObject>(num); };
auto MakeString = [&] (const std::string& string) {
    return std::make_shared<StringObject>(string);
};
auto MakeBoolean = [&] (bool value) { return std::make_shared<BooleanObject>(value); };
auto MakeLambda = [&] (const std::vector<std::string> &args,
                       const std::shared_ptr<PairObject> &body_ptr,
                       ScopePtr outer_scope) {
    return std::make_shared<LambdaFunction>(args, body_ptr, outer_scope);
};
auto MakeQuoted = [&] (ObjectPtr arg) { return std::make_shared<QuotedObject>(arg); };