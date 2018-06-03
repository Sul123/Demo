cmake_minimum_required(VERSION 3.5)
project(lispp)

add_executable(lispp
        main.cpp
        lispp.cpp
        tokenizer.cpp
        number_object.cpp
        boolean_object.cpp
        symbol_object.cpp
        builtin_functions.cpp
        function_object.cpp
        null_object.cpp
        pair_object.cpp
        parser.cpp
        quoted_object.cpp
        scope.cpp
        special_forms.cpp
        string_object.cpp errors.cpp)
