#include "errors.h"

void RuntimeExpect(bool condition, const std::string& error_message) {
    if (!condition) {
        throw RuntimeError(error_message);
    }
}

void SyntaxExpect(bool condition, const std::string& error_message) {
    if (!condition) {
        throw SyntaxError(error_message);
    }
}
