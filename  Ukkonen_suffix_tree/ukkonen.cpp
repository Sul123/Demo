#include <algorithm>
#include "ukkonen.h"

UkkonenSuffixTree::Location UkkonenSuffixTree::SkipDown(int node, int start, int end) const {
    char symbol;
    int prev_node;
    while (start < end) {
        prev_node = node;
        symbol = str_[start];
        node = tree_[node].child.at(symbol);
        const auto& next_node = tree_[node]; // node is already new
        start += next_node.End() - next_node.Start();
    }
    // height from the node might be start - end now
    if (start == end) {
        return Location(node, '\0', 0);
    }
    return Location(prev_node, symbol, tree_[node].Len() - (start - end));
}

UkkonenSuffixTree::Location UkkonenSuffixTree::SuffixLink(
    const UkkonenSuffixTree::Location& loc) const {

    // if location is explicit
    if (loc.depth == 0) {
        return Location(tree_[loc.node_above].suffix_link, '\0', 0);
    } // btw, what if node was a root?

    // now to implicit locations
    int start = tree_[tree_[loc.node_above].child.at(loc.symbol)].Start(); // u crazy
    int end = start + loc.depth;
    // must work for root too, because suffix link must lead to itself
    int descent_start = tree_[loc.node_above].suffix_link;
    // special case if parent was root
    if (loc.node_above == kRootIndex_) {
        ++start;
    }

    return SkipDown(descent_start, start, end);
}


bool UkkonenSuffixTree::HasSymbolBelow(const UkkonenSuffixTree::Location& loc, char symbol) const {
    const auto& node_above = tree_[loc.node_above];
    if (loc.depth == 0) {
        return node_above.child.count(symbol);
    }
    const auto& node_below = tree_[node_above.child.at(loc.symbol)];
    return str_[node_below.Start() + loc.depth] == symbol;
}


UkkonenSuffixTree::Location UkkonenSuffixTree::GoDown(const UkkonenSuffixTree::Location& loc,
                                                      char symbol) const {
    // both cases for explicit and for implicit locations
    const auto& node_above = tree_[loc.node_above];
    const int node_below_ind = (loc.depth == 0) ? node_above.child.at(symbol)
                                                : node_above.child.at(loc.symbol);
    const auto& node_below = tree_[node_below_ind];
    if (node_below.Len() > loc.depth + 1) { // enough place to another implicit location
        char new_loc_symbol = (loc.depth == 0) ? symbol : loc.symbol;
        return Location(loc.node_above, new_loc_symbol, loc.depth + 1);
    }
    // new location is explicit
    return Location(node_below_ind, '\0', 0);
}


bool UkkonenSuffixTree::IsLeaf(int node) const {
    return tree_[node].end == kLeafEnd_;
}


bool UkkonenSuffixTree::IsRoot(const UkkonenSuffixTree::Location& loc) const {
    return loc.node_above == kRootIndex_ && loc.depth == 0;
}


void UkkonenSuffixTree::Attach(int child_ind, int parent_ind, int child_start, int child_end) {
    if (child_ind == static_cast<int>(tree_.size())) {
        tree_.emplace_back(str_);
    }
    auto& child = tree_[child_ind];
    child.parent = parent_ind;
    child.start = child_start;
    child.end = child_end;
    tree_[parent_ind].child[str_[child_start]] = child_ind;
}


int UkkonenSuffixTree::Split(const UkkonenSuffixTree::Location& loc) {
    // explicit case, do nothing
    if (loc.depth == 0) {
        return loc.node_above;
    }

    // now to implicit case
    const int node_below_ind = tree_[loc.node_above].child.at(loc.symbol);

    const int new_node_ind = static_cast<int>(tree_.size());
    Attach(new_node_ind, loc.node_above,
           tree_[node_below_ind].start, tree_[node_below_ind].start + loc.depth);
    Attach(node_below_ind, new_node_ind,
           tree_[node_below_ind].start + loc.depth, tree_[node_below_ind].end);

    return new_node_ind;
}


UkkonenSuffixTree::UkkonenSuffixTree()
    : phase_start_(kRootIndex_, '\0', 0) {
    tree_.emplace_back(-1, 0, -1, str_);
    tree_[0].suffix_link = 0;
}


void UkkonenSuffixTree::Extend(char new_symbol) {
    new_symbol = new_symbol;
    // add symbol to string
    str_ += new_symbol;

    // starting from phase start location
    Location cur_loc = phase_start_;
    // node which we construct link from
    int prev_node_ind = -1;
    do {
        if (HasSymbolBelow(cur_loc, new_symbol)) {
            if (cur_loc.depth == 0 && prev_node_ind != -1) {
                tree_[prev_node_ind].suffix_link = cur_loc.node_above;
            }
            phase_start_ = GoDown(cur_loc, new_symbol);
            return;
        }

        Location next_loc = SuffixLink(cur_loc);
        int leaf_base_ind = Split(cur_loc);
        AttachNewLeaf(leaf_base_ind);
        if (prev_node_ind != -1) {
            tree_[prev_node_ind].suffix_link = leaf_base_ind;
        }

        prev_node_ind = leaf_base_ind;
        cur_loc = next_loc;
    } while (!IsRoot(cur_loc) || !HasSymbolBelow(cur_loc, new_symbol));
    tree_[prev_node_ind].suffix_link = cur_loc.node_above;

    const int descent_candidate_ind = tree_[cur_loc.node_above].child.at(new_symbol);
    const auto& descent_candidate = tree_[descent_candidate_ind];
    if (!IsLeaf(descent_candidate_ind) || descent_candidate.Len() > 1) {
        cur_loc = GoDown(cur_loc, new_symbol);
    }
    phase_start_ = cur_loc;
}


bool UkkonenSuffixTree::ReadSubstring(const std::string& sub_str) const {
    if (sub_str.size() > str_.size()) {
        return false;
    }
    size_t sub_str_ind = 0;
    int cur_node_ind = kRootIndex_;
    while (sub_str_ind < sub_str.size()) {
        if (tree_[cur_node_ind].child.count(sub_str[sub_str_ind]) == 0) {
            return false;
        }
        cur_node_ind = tree_[cur_node_ind].child.at(sub_str[sub_str_ind]);
        const auto& cur_node = tree_[cur_node_ind];
        for (int ind = cur_node.Start(); ind < cur_node.End(); ++ind, ++sub_str_ind) {
            if (sub_str_ind == sub_str.size()) {
                return true;
            }
            if (str_[ind] != sub_str[sub_str_ind]) {
                return false;
            }
        }
    }
    return true;
}


void UkkonenSuffixTree::AttachNewLeaf(int parent_ind) {
    const int child_ind = static_cast<int>(tree_.size());
    Attach(child_ind, parent_ind, static_cast<int>(str_.size() - 1), kLeafEnd_);
}


UkkonenSuffixTree::Node::Node(const std::string& str_t)
    : str(str_t) {}


UkkonenSuffixTree::Node::Node(int start_t, int end_t, int parent_t, const std::string& str_t)
    : start(start_t), end(end_t), parent(parent_t), str(str_t) {}


UkkonenSuffixTree::Node::Node(UkkonenSuffixTree::Node&& other)
    : start(other.start), end(other.end),
      parent(other.parent), suffix_link(other.suffix_link),
      child(std::move(other.child)), str(other.str) {}


int UkkonenSuffixTree::Node::Start() const {
    return start;
}


int UkkonenSuffixTree::Node::End() const {
    return (end == kLeafEnd_) ? static_cast<int>(str.size()) : end;
}


int UkkonenSuffixTree::Node::Len() const {
    return End() - Start();
}


UkkonenSuffixTree::Location::Location(int node_above_t, char symbol_t, int depth_t)
    : node_above(node_above_t), depth(depth_t), symbol(symbol_t) {}
