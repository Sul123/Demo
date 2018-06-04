#pragma once

#include <vector>
#include <map>
#include <string>


class UkkonenSuffixTree {

    struct Node {
        Node() = default;
        explicit Node(const std::string& str_t);
        Node(int start_t, int end_t, int parent_t, const std::string& str_t);
        Node(const Node& other) = delete;
        Node(Node&& other);

        int Start() const;
        int End() const;
        int Len() const;

        int start;
        int end;

        int parent;
        int suffix_link = -2;
        std::map<char, int> child;
        const std::string& str;
    };


    struct Location {
        Location() = default;
        Location(int node_above_t, char symbol_t, int depth_t);

        int node_above = 0;
        int depth = 0;
        char symbol = '\0';
    };

    Location SkipDown(int node, int start, int end) const;
    Location SuffixLink(const Location& loc) const;

    bool HasSymbolBelow(const Location& loc, char symbol) const;

    Location GoDown(const Location& loc, char symbol) const;

    bool IsLeaf(int node) const;
    bool IsRoot(const Location& loc) const;

    void Attach(int child_ind, int parent_ind, int child_start, int child_end);
    void AttachNewLeaf(int parent_ind);

    // splits edge in loc if loc is implicit
    // returns index of inserted, or old node
    int Split(const Location& loc);

public:
    UkkonenSuffixTree();
    void Extend(char new_symbol);
    bool ReadSubstring(const std::string& sub_str) const;

private:
    std::vector<Node> tree_;
    std::string str_;
    Location phase_start_;
    static const int kLeafEnd_ = -1;
    static const int kRootIndex_ = 0;
};
