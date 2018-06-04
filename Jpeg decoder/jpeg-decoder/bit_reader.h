#pragma once
#include <fstream>

class BitReader {
public:
    explicit BitReader(std::istream& in_stream)
        : in_stream_(in_stream), cur_byte_(in_stream_.get()){};

    int Get();
    int Get(int len);  // returns number stacked from next len bits
    void SetSkipRule(uint8_t byte_before, uint8_t skip_byte);

private:
    std::istream& in_stream_;
    uint8_t cur_byte_;
    int bit_pos_ = 0;
    bool skip_rule_ = false;
    uint8_t byte_before_, skip_byte_;
};
