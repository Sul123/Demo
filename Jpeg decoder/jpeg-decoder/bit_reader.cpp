#include "bit_reader.h"
#include "errors.h"

int BitReader::Get() {
    if (bit_pos_ == 8) {
        Expect(in_stream_.peek() != EOF, "Reached the end of file");
        uint8_t old_byte = cur_byte_;
        cur_byte_ = in_stream_.get();
        if (skip_rule_ && old_byte == byte_before_ && cur_byte_ == skip_byte_) {
            cur_byte_ = in_stream_.get();
        }
        bit_pos_ = 0;
    }
    int bit = (cur_byte_ >> (7 - bit_pos_)) & 1;
    ++bit_pos_;
    return bit;
}

int BitReader::Get(int len) {
    int res = 0;
    for (int ind = 0; ind < len; ++ind) {
        res = (res << 1) + Get();
    }
    return res;
}

void BitReader::SetSkipRule(uint8_t byte_before, uint8_t skip_byte) {
    skip_rule_ = true;
    byte_before_ = byte_before;
    skip_byte_ = skip_byte;
}