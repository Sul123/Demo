#pragma once
#include <vector>
#include <memory>
#include "base_object.h"

class BaseObject;

using ArgVector = const std::vector<std::shared_ptr<BaseObject>>&;

namespace BuiltIn {
    ObjectPtr Plus(ArgVector arg_values);
    ObjectPtr Minus(ArgVector arg_values);
    ObjectPtr Multiply(ArgVector arg_values);
    ObjectPtr Divide(ArgVector arg_values);
    ObjectPtr Equal(ArgVector arg_values);
    ObjectPtr Greater(ArgVector arg_values);
    ObjectPtr Less(ArgVector arg_values);
    ObjectPtr Geq(ArgVector arg_values);
    ObjectPtr Leq(ArgVector arg_values);
    ObjectPtr Min(ArgVector arg_values);
    ObjectPtr Max(ArgVector arg_values);
    ObjectPtr Abs(ArgVector arg_values);

    ObjectPtr Cons(ArgVector arg_values);
    ObjectPtr Car(ArgVector arg_values);
    ObjectPtr Cdr(ArgVector arg_values);
    ObjectPtr SetCar(ArgVector arg_values);
    ObjectPtr SetCdr(ArgVector arg_values);
    ObjectPtr List(ArgVector arg_values);
    ObjectPtr ListRef(ArgVector arg_values);
    ObjectPtr ListTail(ArgVector arg_values);



    ObjectPtr IsNull(ArgVector arg_values);
    ObjectPtr IsPair(ArgVector arg_values);
    ObjectPtr IsNumber(ArgVector arg_values);
    ObjectPtr IsBoolean(ArgVector arg_values);
    ObjectPtr IsSymbol(ArgVector arg_values);
    ObjectPtr IsList(ArgVector arg_values);
    ObjectPtr IsEq(ArgVector arg_values);

    ObjectPtr Not(ArgVector arg_values);

    ObjectPtr Concat(ArgVector arg_values);
    ObjectPtr Read(ArgVector arg_values);
    ObjectPtr StringSize(ArgVector arg_values);
    ObjectPtr GetChar(ArgVector arg_values);
    ObjectPtr ConvertToString(ArgVector arg_values);

    ObjectPtr Type(ArgVector arg_values);


}