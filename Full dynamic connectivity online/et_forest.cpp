#include "et_forest.h"

void LeftRotation(Node* node) { // node goes left and down, node->right replaces it
    if (!node) {
        throw std::runtime_error("trying to rotate null");
    }
    Node* replacer = node->right;
    if (!replacer) {
        std::cerr << "smth strange here";
        return; // OMG
    }
    Node* replacer_left = replacer->left;
    Node* node_parent = node->parent;
    if (replacer_left) {
        replacer_left->parent = node;
    }

    node->right = replacer_left;
    node->subtree_size += SubtreeSize(replacer_left) - SubtreeSize(replacer);
    node->highest_spanning_edges_below += SpanningBelow(replacer_left) - SpanningBelow(replacer);
    node->highest_nontree_edges_below += NontreeBelow(replacer_left) - NontreeBelow(replacer);
    replacer->parent = node_parent;

    if (node_parent) {
        if (node == node_parent->right) {
            node_parent->right = replacer;
        } else {
            node_parent->left = replacer;
        }
    }

    replacer->left = node;
    replacer->subtree_size += SubtreeSize(node) - SubtreeSize(replacer_left);
    replacer->highest_spanning_edges_below += SpanningBelow(node) - SpanningBelow(replacer_left);
    replacer->highest_nontree_edges_below += NontreeBelow(node) - NontreeBelow(replacer_left);
    node->parent = replacer;
}

void RightRotation(Node* node) { // node goes right and down, node->left replaces it
    if (!node) {
        throw std::runtime_error("trying to rotate null");
    }
    Node* replacer = node->left;
    if (!replacer) {
        std::cerr << "smth strange here";
        return; // OMG
    }
    Node* replacer_right = replacer->right;
    Node* node_parent = node->parent;
    if (replacer_right) {
        replacer_right->parent = node;
    }
    // node->subtree_size -= SubtreeSize(replacer);
    node->left = replacer_right;
    node->subtree_size += SubtreeSize(replacer_right) - SubtreeSize(replacer);
    node->highest_spanning_edges_below += SpanningBelow(replacer_right) - SpanningBelow(replacer);
    node->highest_nontree_edges_below += NontreeBelow(replacer_right) - NontreeBelow(replacer);
    replacer->parent = node_parent;

    if (node_parent) {
        if (node == node_parent->right) {
            node_parent->right = replacer;
        } else {
            node_parent->left = replacer;
        }
    }
    // replacer->subtree_size -= SubtreeSize(replacer_right);
    replacer->right = node;
    replacer->subtree_size += SubtreeSize(node) - SubtreeSize(replacer_right);
    replacer->highest_spanning_edges_below += SpanningBelow(node) - SpanningBelow(replacer_right);
    replacer->highest_nontree_edges_below += NontreeBelow(node) - NontreeBelow(replacer_right);
    node->parent = replacer;
}


void SubtreeSplay(Node* node) {
    if (!node || !node->parent) {
        return;
    }
    while (node->parent) {
        // Zig-step
        if (!node->parent->parent) { // it means that node->parent is a root
            if (node->parent->left == node) {
                RightRotation(node->parent);
            } else {
                LeftRotation(node->parent);
            }
            // now Zig-Zig steps both left children:
        } else if (node->parent->left == node && node->parent->parent->left == node->parent) {
            RightRotation(node->parent->parent);
            RightRotation(node->parent); // Zig-Zig, both right children:
        } else if (node->parent->right == node && node->parent->parent->right == node->parent) {
            LeftRotation(node->parent->parent);
            LeftRotation(node->parent); // Zig-Zag, we are right son, parent is left son:
        } else if (node->parent->right == node && node->parent->parent->left == node->parent) {
            LeftRotation(node->parent);
            RightRotation(node->parent); // Zig-Zag, we are left son, parent is right son:
        } else {
            RightRotation(node->parent);
            LeftRotation(node->parent);
        }
    }
}


Node* SubtreeMax(Node* subtree_root) { // guaranteed that subtree_root != nullptr
    Node* finder = subtree_root;       // and subtree_root->parent == nullptr
    while (finder->right) {
        finder = finder->right;
    }
    SubtreeSplay(finder);
    return finder;
}


Node* Merge(Node* first_root, Node* second_root) { // returns the root of merge-result
    if (!first_root) {
        return second_root;
    }
    auto new_first_root = SubtreeMax(first_root);
    if (second_root) {
        second_root->parent = new_first_root;
    }
    new_first_root->right = second_root;
    new_first_root->subtree_size += SubtreeSize(second_root);
    new_first_root->highest_spanning_edges_below += SpanningBelow(second_root);
    new_first_root->highest_nontree_edges_below += NontreeBelow(second_root);
    return new_first_root;
}


Node* Add(Node* root, Node* new_max) { // adding the new max-node and returns the root of result
    root = SubtreeMax(root);
    new_max->parent = root;
    root->right = new_max;
    root->subtree_size += 1;
    root->highest_spanning_edges_below += SpanningBelow(new_max);
    root->highest_nontree_edges_below += NontreeBelow(new_max);
    return root;
}


void ChangeAndUpdateNonTreeCounter(Node* changing_node, bool increased) {
    int change = increased ? 1 : -1;

    SubtreeSplay(changing_node);
    changing_node->highest_nontree_edges_below += change;
}


void ChangeAndUpdateSpanningCounter(Node* changing_node, bool increased) {
    int change = increased ? 1 : -1;

    SubtreeSplay(changing_node);
    changing_node->highest_spanning_edges_below += change;
}


Node* IncludingSplit(Node* future_right_root) { // split by right-part root and
    // returning left-part root
    SubtreeSplay(future_right_root);
    if (!future_right_root->left) {
        return nullptr;
    }
    auto left_root = future_right_root->left;
    left_root->parent = nullptr;
    future_right_root->subtree_size -= SubtreeSize(left_root);
    future_right_root->highest_spanning_edges_below -= SpanningBelow(left_root);
    future_right_root->highest_nontree_edges_below -= NontreeBelow(left_root);
    future_right_root->left = nullptr;
    return left_root;
}


Node* ExcludingSplit(Node* future_left_root) { // split by future_left_root which will be the max
    // of left_part
    // and also the root of left-part tree.
    // returns the right-part root
    SubtreeSplay(future_left_root);
    if (!future_left_root->right) {
        return nullptr;
    }
    auto right_root = future_left_root->right;
    right_root->parent = nullptr;
    future_left_root->subtree_size -= SubtreeSize(right_root);
    future_left_root->highest_spanning_edges_below -= SpanningBelow(right_root);
    future_left_root->highest_nontree_edges_below -= NontreeBelow(right_root);
    future_left_root->right = nullptr;
    return right_root;
}


NodePair GetRidOfEdge(Node* forward_edge, Node* back_edge) { // we use this function after re-root
    // so it's guaranteed that "forward_edge" is before the "back_edge" in ET-string and they are
    // really edges. Returns pair of roots of resulting two trees after edge removing
    Node* first_left_part = IncludingSplit(forward_edge);
    // now we need to cut off the "forward edge"
    Node* middle_begin = forward_edge->right;
    middle_begin->parent = nullptr;
    delete forward_edge;

    Node* first_right_part = ExcludingSplit(back_edge);
    // now we need to cut off the "back edge"
    Node* middle_end = back_edge->left;
    middle_end->parent = nullptr;
    delete back_edge;
    // middle_end must be the root of the middle_part now
    //let's merge first_left and first_right
    Node* first_root = Merge(first_left_part, first_right_part);
    return {first_root, middle_end};
}


Node* ReRootTo(Node* new_root) { // new_root is NOT the root of resulting BST in general!
    // however, function returns the root of BST
    Node* left_part_root = IncludingSplit(new_root);
    if (!left_part_root) { // but yes, if new_root was the beginning of the ET-string it becomes
        // the root of his BST
        return new_root;
    }
    return Merge(new_root, left_part_root);
}


bool InSameTree(Node* first, Node* second) {
    if (!first || !second) {
        return false;
    }
    SubtreeSplay(first);
    Node* second_root_finder = second;
    while (second_root_finder->parent) {
        second_root_finder = second_root_finder->parent;
    }
    if (second_root_finder == first) {
        SubtreeSplay(second);
        return true;
    }
    SubtreeSplay(second);
    return false;
}

int SubtreeSize(Node* node) {
    return node ? node->subtree_size : 0;
}

int SpanningBelow(Node* node) {
    return node ? node->highest_spanning_edges_below : 0;
}

int NontreeBelow(Node* node) {
    return node ? node->highest_nontree_edges_below : 0;
}


