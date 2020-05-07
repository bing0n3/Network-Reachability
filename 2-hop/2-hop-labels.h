#ifndef N_2_HOP_LABELS_H
#define N_2_HOP_LABELS_H

#include <iostream>
#include <fstream>

#include <vector>
#include <stack>
#include <map>
#include <queue>
#include <algorithm>
#include <set>
#include <string>

#include <time.h>

using namespace std;

unsigned int typedef LabelSet;

struct Edge {
    int headVex, tailVex;
    Edge *headLink, *tailLink;

    Edge(int headVex, int tailVex) {
        this->headVex = headVex;
        this->tailVex = tailVex;
        this->headLink = this->tailLink = nullptr;
    }
};


class Node {
public:
    bool tarjan;
    bool bfs;
    bool reverse_bfs;
    int data;
    vector<int> inNodes, outNodes;
    Edge *firIn, *firOut;
    LabelSet ls;

    Node(int data) {
        this->data = data;
        this->tarjan = this->bfs = this->reverse_bfs = false;
        this->firIn = this->firOut = nullptr;
    }

    Node(int data, LabelSet ls) {
        this->data = data;
        this->tarjan = this->bfs = this->reverse_bfs = false;
        this->firIn = this->firOut = nullptr;
        this->ls = ls;
    }
};

struct Graph {
    map<int, Node*> nodes;
};

map<LabelSet, Graph> typedef LabeledGraphList;

void two_hop_label(Graph*);

void tarjan(Graph*,Node*, map<int, int>&, map<int, int>&, stack<int>&, vector<vector<int> >&, map<int, bool>&);
void combine_scc_node(Graph*, vector<vector<int> >&);
bool query(Graph*, int outNodeNum, int inNodeNum);
void search_out_node(Graph*, Node*);
void search_in_node(Graph*, Node*);

inline void insert_edge(Node* node, Edge* edge)
{
    Edge *last_edge = node->firOut;
    node->firOut = edge;
    edge->headLink = last_edge;
}

inline void insert_reverse_edge(Node* node, Edge* edge)
{

    Edge *last_edge = node->firIn;
    node->firIn = edge;
    edge->tailLink = last_edge;
}

inline Node* init_inexistent_node(Graph &graph, int nodeNum)
{
    if (graph.nodes[nodeNum] == nullptr) {
        graph.nodes[nodeNum] = new Node(nodeNum);
    }

    return graph.nodes[nodeNum];
}

inline Node* init_inexistent_label_node(Graph &graph, int nodeNum, LabelSet ls)
{
    if (graph.nodes[nodeNum] == nullptr) {
        graph.nodes[nodeNum] = new Node(nodeNum, ls);
    }
    return graph.nodes[nodeNum];
}

inline int cal_graph_size(Graph &graph) {
    int total;
    for (auto node: graph.nodes) {
        for (Edge *edge = node.second->firOut; edge != nullptr; edge = edge->headLink) {
            total += sizeof(int);
        }
    }
    return total;
}


class HopLabling {
public:
    Graph G;
    LabeledGraphList G_set;
    HopLabling(vector<vector<int>> graph, vector<LabelSet> ls_set, vector<LabelSet> ls_kind){
        buildGraph(graph, ls_set, ls_kind);
        generate_one_label_index();
    }
    void buildGraph(vector<vector<int>>, vector<LabelSet>, vector<LabelSet>);
    int cal_graph_index_size();
    bool queryNaive(int, int, LabelSet);
private:
    vector<LabelSet> labels;
    void generate_one_label_index();

    static void two_hop_label(Graph* graph);
    vector<int> neighbors_with_diff_label(Node *node, LabelSet ls);
};

// is  l2 is a subset of l1
inline bool isLabelSubset(LabelSet ls1, LabelSet ls2){
return ((ls1& ls2) == ls1);
}

inline LabelSet labelIDToLabelSet(unsigned int id)
{
    // used by a graph to change labelIDs from input to a Labelset
    // e.g. (LabelID) 0 -> (LabelSet) 0001
    return (1 << id);
}


#endif //N_2_HOP_LABELS_H