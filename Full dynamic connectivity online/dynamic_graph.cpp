#include <cmath>
#include "dynamic_graph.h"


DynamicGraph::DynamicGraph(int vertex_count)
    : vertex_count_(vertex_count),
      components_num_(vertex_count),
      levels_num_(static_cast<int>(std::log2(vertex_count)) + 1) {}

DynamicGraph::~DynamicGraph() {
    for (int level = 0; level < levels_num_; ++level) {
        for (auto& item : level_vertex_occurrence_[level]) {
            delete item.second;
        }

        for (auto& item : level_edge_occurrence_[level]) {
            delete item.second;
        }
    }
}

void DynamicGraph::AddEdge(int start, int end) {

    if (start < 0 || end < 0 || start >= vertex_count_ || end >= vertex_count_) {
        return;
    }
    if (edge_level_.find({start, end}) != edge_level_.end()) {
        return; // Now it's safe to add edges that already exists
    }

    Node* start_occurrence;
    Node* end_occurrence;

    try {
        start_occurrence = level_vertex_occurrence_[levels_num_ - 1].at(start);
    } catch (const std::out_of_range&) {
        start_occurrence = new Node({start, start}, 0, 0);
        level_vertex_occurrence_[levels_num_ - 1][start] = start_occurrence;
    }

    try {
        end_occurrence = level_vertex_occurrence_[levels_num_ - 1].at(end);
    } catch (const std::out_of_range&) {
        end_occurrence = new Node({end, end}, 0, 0);
        level_vertex_occurrence_[levels_num_ - 1][end] = end_occurrence;
    }

    edge_level_[{start, end}] = levels_num_ - 1;
    edge_level_[{end, start}] = levels_num_ - 1;
//        std::cerr << "Edge " << start << " " << end << " set to level " << levels_num_ - 1 << "\n";

    if (InSameTree(start_occurrence, end_occurrence)) {
        // now end_occurrence is the root of it's ET-tree
        leveled_nontree_neighbours_[start][levels_num_ - 1].insert(end);
        leveled_nontree_neighbours_[end][levels_num_ - 1].insert(start);
        ChangeAndUpdateNonTreeCounter(end_occurrence, true);
        ChangeAndUpdateNonTreeCounter(start_occurrence, true);
        return;
    }
    Node* middle_part_root = ReRootTo(end_occurrence);
    // now "end" is the start of its Euler-tour
    Node* last_part_root = ExcludingSplit(start_occurrence);
    // now start_occurrence is the root of the first_part tree
    // so we need to merge in order:
    // start_occurrence - {start, end} - middle_part_root - {end, start} - last_part_root
    Node* forward_edge = new Node(VertEdge{start, end}, 1, 0);
    level_edge_occurrence_[levels_num_ - 1][{start, end}] = forward_edge;
    Node* back_edge = new Node(VertEdge{end, start}, 1, 0);
    level_edge_occurrence_[levels_num_ - 1][{end, start}] = back_edge;

    Node* current_root = start_occurrence;
    current_root = Add(current_root, forward_edge);
    current_root = Merge(current_root, middle_part_root);
    current_root = Add(current_root, back_edge);
    Merge(current_root, last_part_root);

    --components_num_;
}

void DynamicGraph::RemoveEdge(int start, int end) {
    VertEdge removing_edge{start, end};
    int starting_level;
    try {
        starting_level = edge_level_.at(removing_edge);
    } catch (const std::out_of_range& ) {
        return; // now it's safe to remove non-existing edges
    }

    Node* start_occurrence = level_vertex_occurrence_[starting_level].at(start);
    Node* end_occurrence = level_vertex_occurrence_[starting_level].at(end);

    /////////////////Simple case///////////////////
    if (leveled_nontree_neighbours_[start][starting_level].find(end) !=
        leveled_nontree_neighbours_[start][starting_level].end()) {
        // it means that removing edge is not in forest
        leveled_nontree_neighbours_[start][starting_level].erase(end);
        leveled_nontree_neighbours_[end][starting_level].erase(start);
        ChangeAndUpdateNonTreeCounter(end_occurrence, false);
        ChangeAndUpdateNonTreeCounter(start_occurrence, false);
        edge_level_.erase(removing_edge); // global remove, edge_level is gone
        edge_level_.erase({removing_edge.end, removing_edge.start}); // don't forget about backward
        return;
    }
    //////////////Now hard case////////////////
    for (int current_level = starting_level; current_level < levels_num_; ++current_level) {
        NodePair tree_roots = RemoveEdgeFromForest(start, end, current_level);
        Node *smaller_root = tree_roots.first, *bigger_root = tree_roots.second;
        if (smaller_root->subtree_size > bigger_root->subtree_size) {
            std::swap(smaller_root, bigger_root);
        }
        //////just check///////
        if (smaller_root->highest_spanning_edges_below < 0) {
            std::cerr << "SpanCounter < 0 , idiot!";
            exit(100);
        }
        //////////////////////
        std::vector<int> spans;
        while (smaller_root->highest_spanning_edges_below > 0) {
            spans.push_back(smaller_root->highest_spanning_edges_below);
            while (true) {
                VertEdge edge = smaller_root->data;
                if (edge.start != edge.end && edge_level_.at(edge) == current_level) {
                    PushSpanningOnLevel(edge.start, edge.end,
                                        current_level - 1); // we pushed both
                    ChangeAndUpdateSpanningCounter(smaller_root, false); // but it's not over
                    // we must also change counter in backward_edge node
                    smaller_root = level_edge_occurrence_[current_level].at({edge.end,
                                                                             edge.start});
                    ChangeAndUpdateSpanningCounter(smaller_root, false);
                    // now smaller root is root again
                    break;
                }
                if (smaller_root->left
                    && smaller_root->left->highest_spanning_edges_below > 0) {
                    smaller_root = smaller_root->left;
                } else if (smaller_root->right) {
                    smaller_root = smaller_root->right;
                } else {
                    std::cerr << "lol\n";
                    for (int s : spans) {
                        std::cerr << s << " ";
                    }
                    exit(1);
                }
            }
        }
        ///////////looks like we're done with pushing down spanning edges///////////

        while (smaller_root->highest_nontree_edges_below > 0) {
            while (true) {
                VertEdge vertex = smaller_root->data;
                if (vertex.start == vertex.end &&
                    !leveled_nontree_neighbours_[vertex.start][current_level].empty()) {
                    int en = *leveled_nontree_neighbours_[vertex.start][current_level].begin();
                    Node *en_occurrence = level_vertex_occurrence_[current_level].at(en);
                    // nontree edge will be deleted from this level anyway
                    leveled_nontree_neighbours_[vertex.start][current_level].erase(en);
                    leveled_nontree_neighbours_[en][current_level].erase(vertex.start);
                    if (!InSameTree(en_occurrence, bigger_root)) {
                        // now end_occurrence is root of our tree
                        PushNonTreeOnLevel(vertex.start, en, current_level - 1);
                        ChangeAndUpdateNonTreeCounter(en_occurrence, false);
                        // we started with backward, smaller root is still on the start
                        ChangeAndUpdateNonTreeCounter(smaller_root, false);
                        // now smaller_root is root again
                    } else {
                        // replacement edge found: {vertex.start, en}
                        // we also need to change counter - it will no longer be nontree edge
                        ChangeAndUpdateNonTreeCounter(en_occurrence, false);
                        ChangeAndUpdateNonTreeCounter(smaller_root, false);
                        PushSpanningOnLevel(vertex.start, en, current_level, true);
                        for (int rest_level = current_level + 1; rest_level < levels_num_;
                             ++rest_level) {
                            RemoveEdgeFromForest(start, end, rest_level);
                            PushSpanningOnLevel(vertex.start, en, rest_level, false);
                        }
                        edge_level_.erase(removing_edge); // global remove, edge_level is gone
                        edge_level_.erase({removing_edge.end, removing_edge.start}); // don't forget about backward
                        return;
                    }
                    break;
                }
                if (smaller_root->left
                    && smaller_root->left->highest_nontree_edges_below > 0) {
                    smaller_root = smaller_root->left;
                } else {
                    smaller_root = smaller_root->right;
                }
            }
        }
        if (smaller_root->subtree_size == 1) {
            level_vertex_occurrence_[current_level].erase(smaller_root->data.start);
            delete smaller_root;
        }
        if (bigger_root->subtree_size == 1) {
            level_vertex_occurrence_[current_level].erase(bigger_root->data.start);
            delete bigger_root;
        }
    }
    // this is fiasko, bratan
    edge_level_.erase(removing_edge); // global remove, edge_level is gone
    edge_level_.erase({removing_edge.end, removing_edge.start}); // don't forget about backward
    ++components_num_;
}

int DynamicGraph::GetComponentsNumber() const {
    return components_num_;
}

bool DynamicGraph::Connected(int start, int end) {
    Node* start_occurrence = nullptr;
    Node* end_occurrence = nullptr;
    try {
        start_occurrence = level_vertex_occurrence_[levels_num_ - 1].at(start);
        end_occurrence = level_vertex_occurrence_[levels_num_ - 1].at(end);
    } catch (const std::out_of_range&) {}

    return InSameTree(start_occurrence, end_occurrence);
}

void DynamicGraph::PushSpanningOnLevel(int start, int end, int level, bool set_edge_level) {
    Node* start_occurrence;
    Node* end_occurrence;
    try {
        start_occurrence = level_vertex_occurrence_[level].at(start);
    } catch (const std::out_of_range&) {
        start_occurrence = new Node({start, start}, 0, 0);
        level_vertex_occurrence_[level][start] = start_occurrence;
    }
    try {
        end_occurrence = level_vertex_occurrence_[level].at(end);
    } catch (const std::out_of_range&) {
        end_occurrence = new Node({end, end}, 0, 0);
        level_vertex_occurrence_[level][end] = end_occurrence;
    }
    if (set_edge_level) {
        edge_level_[{start, end}] = level; // this block repeats start of previous function
        edge_level_[{end, start}] = level;
    }
    // and this is also just repeating
    Node* middle_part_root = ReRootTo(end_occurrence);
    Node* last_part_root = ExcludingSplit(start_occurrence);
    Node* forward_edge = new Node(VertEdge{start, end}, 1, 0);
    if (!set_edge_level) {
        forward_edge->highest_spanning_edges_below = 0;
    }
    level_edge_occurrence_[level][{start, end}] = forward_edge;
    Node* back_edge = new Node(VertEdge{end, start}, 1, 0);
    if (!set_edge_level) {
        back_edge->highest_spanning_edges_below = 0;
    }
    level_edge_occurrence_[level][{end, start}] = back_edge;

    Node* current_root = start_occurrence;
    current_root = Add(current_root, forward_edge);
    current_root = Merge(current_root, middle_part_root);
    current_root = Add(current_root, back_edge);
    Merge(current_root, last_part_root);
}

void DynamicGraph::PushNonTreeOnLevel(int start, int end, int level) { // must be guaranteed that nodes exist

    Node* start_occurrence = level_vertex_occurrence_[level].at(start);
    Node* end_occurrence = level_vertex_occurrence_[level].at(end);
    edge_level_[{start, end}] = level;
    edge_level_[{end, start}] = level; // repeating again...

    leveled_nontree_neighbours_[start][level].insert(end);
    leveled_nontree_neighbours_[end][level].insert(start);
    ChangeAndUpdateNonTreeCounter(end_occurrence, true);
    ChangeAndUpdateNonTreeCounter(start_occurrence, true);
}

NodePair DynamicGraph::RemoveEdgeFromForest(int start, int end, int forest_level) { // returns two ET-trees of
    // two resulting trees after removing edge

    ReRootTo(level_vertex_occurrence_[forest_level].at(start));
    VertEdge forward{start, end};
    VertEdge back{end, start};
    Node* forward_edge = level_edge_occurrence_[forest_level].at(forward);
    Node* back_edge = level_edge_occurrence_[forest_level].at(back);
    level_edge_occurrence_[forest_level].erase(forward);
    level_edge_occurrence_[forest_level].erase(back);
    return GetRidOfEdge(forward_edge, back_edge);
}
