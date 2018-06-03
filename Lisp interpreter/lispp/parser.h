#pragma once
#include <vector>
#include <memory>
#include "tokenizer.h"
#include "base_object.h"


ObjectPtr ParseExpression(Tokenizer* tok);
ObjectPtr ParsePair(Tokenizer* tok);
ObjectPtr Parse(Tokenizer* tok);
