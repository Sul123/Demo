#include <sstream>
#include <iostream>
#include <cctype>
#include "tokenizer.h"
#include "errors.h"

Tokenizer::Tokenizer(std::istream* in) : in_(in) {}

Tokenizer::TokenType Tokenizer::GetType() const {
    return type_;
}
int64_t Tokenizer::GetNumber() const {
    return number_;
}

std::string Tokenizer::GetSymbol() const {
    return symbol_;
}
std::string Tokenizer::GetString() const {
    return string_;
}

void Tokenizer::ReadNext() {

    *in_ >> std::ws;

    if (std::isdigit(in_->peek())) {
        *in_ >> number_;
        type_ = Tokenizer::TokenType::NUMBER;
        return;
    }
    if (in_->peek() == '+' || in_->peek() == '-') {
        char num_sign = in_->get();
        if (std::isdigit(in_->peek())) {
            int64_t num;
            *in_ >> num;
            number_ = (num_sign == '+') ? num : -num;
            type_ = Tokenizer::TokenType::NUMBER;
        } else {
            symbol_ = num_sign;
            type_ = Tokenizer::TokenType::SYMBOL;
        }
        return;
    }


    switch (in_->peek()) {
        case EOF:
            type_ = Tokenizer::TokenType::END;
            break;

        case '(':
            ++balance_;
            in_->get();
            type_ = Tokenizer::TokenType::OPEN;
            break;

        case ')':
            --balance_;
            if (balance_ < 0) {
                throw SyntaxError("Wrong Syntax (check the parenthesis balance)");
            }
            in_->get();
            type_ = Tokenizer::TokenType::CLOSE;
            break;

        case '.':
            in_->get();
            type_ = Tokenizer::TokenType::DOT;
            break;

        case '\"':
            in_->get();
            string_.clear();
            while (in_->peek() != '\"') {
                if (in_->peek() == EOF) {
                    throw SyntaxError("Wrong string syntax");
                }
                string_ += static_cast<char>(in_->get());
            }
            in_->get();
            type_ = Tokenizer::TokenType::STRING;
            break;

        case '\'':
            in_->get();
            type_ = Tokenizer::TokenType::QUOTE;
            break;

        default:
            symbol_.clear();
            while (in_->peek() != EOF &&
                    !std::isspace(static_cast<unsigned char>(in_->peek())) && in_->peek() != ')') {
                symbol_ += static_cast<char>(in_->get());
            }
            type_ = Tokenizer::TokenType::SYMBOL;
    }
}
