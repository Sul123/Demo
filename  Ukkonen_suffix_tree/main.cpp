#include <iostream>
#include "ukkonen.h"

void OnlineSuffixTreeDemonstration() {
    UkkonenSuffixTree tree;
    std::cout << "Available commands:\n"
        "1. add *string* - extend text with *string*\n"
        "2. sub? *string* - tell if *string* is substring of the text in current moment\n";
    while (!std::cin.eof()) {
        std::string query, text;
        std::cin >> query >> text;
        if (query == "add") {
            for (char c : text) {
                tree.Extend(c);
            }
        } else if (query == "sub?") {
            std::cout <<  (tree.ReadSubstring(text) ? "YES\n" : "NO\n");
        }
    }
}

int main() {

    OnlineSuffixTreeDemonstration();

    return 0;
}

