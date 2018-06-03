#pragma once
#include <stdexcept>
#include <string>

class LispError : public std::runtime_error {
public:
    explicit LispError(const std::string& info)
            : std::runtime_error(info) {}
};

class RuntimeError : public LispError {
public:
    explicit RuntimeError(const std::string& info)
            : LispError(info) {}

};

void RuntimeExpect(bool condition, const std::string& error_message);

class SyntaxError : public LispError {
public:
    explicit SyntaxError(const std::string& info)
            : LispError(info) {}
};

void SyntaxExpect(bool condition, const std::string& error_message);

class NameError : public LispError {
public:
    explicit NameError(const std::string& info)
            : LispError(info) {}

};