#pragma once
#include <memory>
#include <optional>
#include <vector>
#include "bit_reader.h"

class BitReader;

struct Node {
    Node() = default;

    ~Node() {
        delete left;
        delete right;
    }

    std::optional<uint8_t> container = std::nullopt;
    Node *left = nullptr, *right = nullptr;
    Node* parent = nullptr;
};

struct HuffmanTree {
    HuffmanTree() = default;

    ~HuffmanTree() { delete root; }

    Node* root = nullptr;
};

class HuffmanDecoder {
public:
    HuffmanDecoder() = default;

    // codes_number[i] - number of codes of length i + 1
    // there might be zeros, but only if there is non-zero value somewhere next
    // codes_values contains only existing values, so size of these vectors
    // may be different
    void BuildTree(const std::vector<int>& codes_number,
                   const std::vector<uint8_t>& codes_values);

    uint8_t DecodeSymbol(BitReader* bit_reader) const;

private:
    HuffmanTree tree_;
};
