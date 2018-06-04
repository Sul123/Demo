#pragma once
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <stdexcept>

struct VertEdge {
    int start;
    int end;

    bool operator<(const VertEdge& other) const {
        return (start == other.start) ? (end < other.end) : (start < other.start);
    }
};

struct Node {
    Node() = default;

    explicit Node(const VertEdge& some_data, int spanning_edges_below, int nontree_edges_below)
            : data(some_data),
              highest_spanning_edges_below(spanning_edges_below),
              highest_nontree_edges_below(nontree_edges_below) {}

    Node(const Node& other) = default;

    VertEdge data;
    int highest_spanning_edges_below;
    int highest_nontree_edges_below;
    Node *parent = nullptr;
    Node *left = nullptr, *right = nullptr;

    int subtree_size = 1;
};

struct NodePair {
    Node* first;
    Node* second;
};

int SubtreeSize(Node* node);

int SpanningBelow(Node *node);
int NontreeBelow(Node *node);

void LeftRotation(Node* node);
void RightRotation(Node* node);

void SubtreeSplay(Node* node);

Node* SubtreeMax(Node* subtree_root);
Node* Merge(Node* first_root, Node* second_root);

Node* Add(Node* root, Node* new_max);

void ChangeAndUpdateNonTreeCounter(Node *changing_node, bool increased);
void ChangeAndUpdateSpanningCounter(Node *changing_node, bool increased);

Node* IncludingSplit(Node *future_right_root);
Node* ExcludingSplit(Node* future_left_root);

NodePair GetRidOfEdge(Node* forward_edge, Node* back_edge);
Node* ReRootTo(Node* new_root);

bool InSameTree(Node* first, Node* second);

