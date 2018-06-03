#include <iostream>
#include "parser.h"
#include "tokenizer.h"
#include "number_object.h"
#include "boolean_object.h"
#include "symbol_object.h"
#include "quoted_object.h"
#include "null_object.h"
#include "pair_object.h"
#include "errors.h"
#include "special_forms.h"
#include "string_object.h"
#include "alias.h"

ObjectPtr Parse(Tokenizer* tok) {
    tok->ReadNext();
    auto result = ParseExpression(tok);
    tok->ReadNext();

    SyntaxExpect(tok->GetType() == Tokenizer::TokenType::END, "Wrong syntax");

    return result;
}

ObjectPtr ParseExpression(Tokenizer* tok) {

    switch (tok->GetType()) {
        case Tokenizer::TokenType::END: {
            throw SyntaxError("syntax error, empty string");
        }

        case Tokenizer::TokenType::NUMBER: {
            auto num_obj = std::make_shared<NumberObject>(tok->GetNumber());
            return num_obj;
        }

        case Tokenizer::TokenType::STRING: {
            auto string_obj = std::make_shared<StringObject>(tok->GetString());
            return string_obj;
        }

        case Tokenizer::TokenType::SYMBOL: {
            std::string symbol = tok->GetSymbol();
            if (symbol == "#t" || symbol == "#f") {
                auto boolean_obj = std::make_shared<BooleanObject>(symbol);
                return boolean_obj;
            }
            auto symbol_obj = std::make_shared<SymbolObject>(tok->GetSymbol());
            return symbol_obj;
        }

        case Tokenizer::TokenType::QUOTE: {
            tok->ReadNext();
            SyntaxExpect(tok->GetType() != Tokenizer::TokenType::END,
                         "Wrong syntax, nothing after quote");
            auto expr = ParseExpression(tok);
            return std::make_shared<QuotedObject>(expr);
        }

        case Tokenizer::TokenType::OPEN: {
            tok->ReadNext();
            if (tok->GetType() == Tokenizer::TokenType::CLOSE) {
                return std::make_shared<NullObject>();
            }
            auto pair_obj = ParsePair(tok);
            SyntaxExpect(tok->GetType() == Tokenizer::TokenType::CLOSE, "No ) in the end of pair");

            return pair_obj;
        }

        default:
            break;
    }
    throw SyntaxError("Wrong expression");
}

ObjectPtr ParsePair(Tokenizer* tok) {

    // tmp vector for objects to build it in pairs
    std::vector<ObjectPtr> list;

    while (tok->GetType() != Tokenizer::TokenType::DOT
           && tok->GetType() != Tokenizer::TokenType::CLOSE) {
        SyntaxExpect(tok->GetType() != Tokenizer::TokenType::END, "Wrong syntax, unclosed pair");

        list.push_back(ParseExpression(tok));
        tok->ReadNext();
    }

    if (tok->GetType() == Tokenizer::TokenType::DOT) {
        tok->ReadNext();
        list.push_back(ParseExpression(tok));
        tok->ReadNext();
        SyntaxExpect(tok->GetType() == Tokenizer::TokenType::CLOSE,
                     "Wrong syntax, your fake list is too fake");

    } else if (tok->GetType() == Tokenizer::TokenType::CLOSE) {
        list.push_back(std::make_shared<NullObject>());
    } else {
        throw SyntaxError("I don't understand how i could get here");
    }
    SyntaxExpect(list.size() >= 2, "Wrong syntax");

    // now let's build pairs
    std::vector<ObjectPtr> pairs;
    pairs.push_back(list.back());
    size_t ind = list.size() - 1;
    while (ind > 0) {
        --ind;
        pairs.push_back(std::make_shared<PairObject>(list[ind], pairs.back()));
    }

    auto return_obj_ptr = std::dynamic_pointer_cast<PairObject>(pairs.back());
    if (return_obj_ptr->Car()->IsSymbol() &&
        ToSymbol(return_obj_ptr->Car())->GetSymbol() == "quote") {
        return Special::Quote(return_obj_ptr->Cdr());
    }

    return pairs.back();
}