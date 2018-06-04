#include "huffman.h"
#include "bit_reader.h"
#include "errors.h"

void HuffmanDecoder::BuildTree(const std::vector<int>& codes_number,
                               const std::vector<uint8_t>& codes_values) {
    delete tree_.root;
    tree_.root = new Node;

    Node* cur_node = tree_.root;
    int req_depth = 1;
    int cur_depth = 0;
    size_t value_ind = 0;

    for (int number : codes_number) {
        while (number != 0) {
            while (cur_depth != req_depth) {
                if (!cur_node->left) {
                    cur_node->left = new Node;
                    cur_node->left->parent = cur_node;
                    cur_node = cur_node->left;
                    ++cur_depth;
                } else if (!cur_node->right) {
                    cur_node->right = new Node;
                    cur_node->right->parent = cur_node;
                    cur_node = cur_node->right;
                    ++cur_depth;
                } else {
                    Expect(cur_node->parent,
                           "Wrong data for building Huffman tree");
                    cur_node = cur_node->parent;
                    --cur_depth;
                }
            }
            cur_node->container = codes_values[value_ind];
            cur_node = cur_node->parent;
            --cur_depth;
            --number;
            ++value_ind;
        }
        ++req_depth;
    }
}

uint8_t HuffmanDecoder::DecodeSymbol(BitReader* bit_reader) const {
    Node* cur_node = tree_.root;
    while (!cur_node->container.has_value()) {
        if (bit_reader->Get() == 0) {
            Expect(cur_node->left, "Wrong huffman code");
            cur_node = cur_node->left;
        } else {
            Expect(cur_node->right, "Wrong huffman code");
            cur_node = cur_node->right;
        }
    }
    return cur_node->container.value();
}
