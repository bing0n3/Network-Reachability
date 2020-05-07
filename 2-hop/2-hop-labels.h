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
    vector<string> attributes;
    LabelSet ls;

    Node(int data) {
        this->data = data;
        this->tarjan = this->bfs = this->reverse_bfs = false;
        this->firIn = this->firOut = nullptr;
    }

    Node(int data, vector<string> &attributes) {
        this->data = data;
        this->tarjan = this->bfs = this->reverse_bfs = false;
        this->firIn = this->firOut = nullptr;
        this->attributes = attributes;
    }
};

struct Graph {
    int nodeNum, edgeNum;
    map<string, int> numberMap;
    map<int, string> nameMap;
    map<int, Node*> nodes;
};

struct LabeledGraphList {
    map<string, Graph> graphs;
};

void two_hop_label(Graph*);

void generate_one_label_index(LabeledGraphList &graphs);
void tarjan(Graph*,Node*, map<int, int>&, map<int, int>&, stack<int>&, vector<vector<int> >&, map<int, bool>&);
void combine_scc_node(Graph*, vector<vector<int> >&);
bool query(Graph*, int outNodeNum, int inNodeNum);
void search_out_node(Graph*, Node*);
void search_in_node(Graph*, Node*);
void read_labeling_graph(char*);
void input_and_query(Graph *graph);
bool constrainedQuery(Graph &, LabeledGraphList &, int outNodeNum, int inNodeNum, vector<string>& labels);
vector<int> neighbors_with_diff_label(Graph& graph, Node* node, vector<string>&);

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

inline Node* init_inexistent_label_node(Graph &graph, int nodeNum, vector<string>& attributes)
{
    if (graph.nodes[nodeNum] == nullptr) {
        graph.nodes[nodeNum] = new Node(nodeNum, attributes);
    }
    return graph.nodes[nodeNum];
}

int cal_graph_size(Graph &graph);

vector<pair<pair<int, int>, vector<string>>> read_query(string file_name);
#endif //N_2_HOP_LABELS_H