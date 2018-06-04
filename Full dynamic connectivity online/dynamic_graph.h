#pragma once

#include "et_forest.h"
#include <map>
#include <unordered_map>

class DynamicGraph {
public:
    explicit DynamicGraph(int vertex_count);
    ~DynamicGraph();
    void AddEdge(int start, int end);
    void RemoveEdge(int start, int end);
    int GetComponentsNumber() const;
    bool Connected(int start, int end);

private:
    void PushSpanningOnLevel(int start, int end, int level, bool set_edge_level = true);
    void PushNonTreeOnLevel(int start, int end, int level);
    NodePair RemoveEdgeFromForest(int start, int end, int forest_level);

private:
    const int vertex_count_;
    int components_num_;
    const int levels_num_;
    std::unordered_map<int, std::map<int, Node*>> level_vertex_occurrence_;
    std::unordered_map<int, std::map<VertEdge, Node*>> level_edge_occurrence_;
    std::map<int, std::map<int, std::set<int>>> leveled_nontree_neighbours_;
    // leveled_nontree_neighbours[vertex][level] - set of all neighbours to "vertex" that connected
    // by the nontree edge of level "level"
    std::map<VertEdge, int> edge_level_;
};

