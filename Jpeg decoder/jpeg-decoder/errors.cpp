#include "errors.h"

void Expect(bool condition, const std::string& error_message) {
    if (!condition) {
        throw std::runtime_error(error_message);
    }
}
