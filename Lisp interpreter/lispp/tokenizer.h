#pragma once
#include <sstream>
#include <iostream>
#include <string>

class Tokenizer {
public:
    explicit Tokenizer(std::istream* in);

    enum class TokenType {
        UNKNOWN,
        NUMBER,
        OPEN,
        CLOSE,
        QUOTE,
        DOT,
        SYMBOL,
        STRING,
        END
    };

    void ReadNext();

    TokenType GetType() const;
    int64_t GetNumber() const;
    std::string GetSymbol() const;
    std::string GetString() const;

private:
    std::istream* in_;

    TokenType type_ = TokenType::UNKNOWN;
    int64_t number_;
    std::string symbol_;
    std::string string_;

    int balance_ = 0;


};
