#include "2-hop-labels.h"
#include <sstream>
#include <utility>


// constructor for graphlist for each label/
// build 2 hop index for each label



void HopLabling::buildGraph(vector<vector<int> > graph, vector<LabelSet> ls_set, vector<LabelSet> ls_kind) {
    this->labels = std::move(ls_kind);
    // build G_set
    for (auto l: labels) {
        this->G_set[l] = Graph();
    }

    // build vertex
    for (int i = 0; i < ls_set.size(); i++) {
        init_inexistent_label_node(this->G, i, ls_set[i]);
        for (auto l :labels) {
            // if vertex have labels
            if (isLabelSubset(l, ls_set[i])) {
                init_inexistent_label_node(this->G_set[l], i, ls_set[i]);
            }
        }
    }

    // build edge
    for (int i = 0; i < graph.size(); i++) {
        int outNodeNum = i;
        for (int inNodeNum : graph[i]) {
            Node *outNode = this->G.nodes[outNodeNum];
            Node *inNode = this -> G.nodes[inNodeNum];

            Edge *edge = new Edge(outNodeNum, inNodeNum);
            insert_edge(outNode,  edge);
            insert_reverse_edge(inNode, edge);


            LabelSet combine_ls = outNode->ls & inNode->ls;

            if (combine_ls == 0) {
                continue;
            }
            for (auto ls: this->labels) {
                if (isLabelSubset(ls, combine_ls)) {
                    edge = new Edge(outNodeNum, inNodeNum);
                    insert_edge(this->G_set[ls].nodes[outNodeNum], edge);
                    insert_reverse_edge(this->G_set[ls].nodes[inNodeNum], edge);
                }
            }
        }
    }
}


void HopLabling::generate_one_label_index() {
    int total = 0;
    time_t start = clock();
    for (auto &graph: this->G_set) {
        this->two_hop_label(&graph.second);
        cout << "Finish Build one Index" << endl;

    }
    time_t end = clock();
    cout << "Build Index Total Time: " << (float)(end-start)/CLOCKS_PER_SEC << " seconds" << endl;
}

int HopLabling::cal_graph_index_size() {
    int total = 0;
    for (auto &graph: this->G_set) {
        for (auto &cur_node: graph.second.nodes) {
            for (Edge *edge = cur_node.second->firOut; edge != nullptr; edge = edge->headLink)
                total += sizeof(int);
        }
    }
    return total;
}


void HopLabling::two_hop_label(Graph *graph) {
    clock_t start = clock();
//    cout << "Finding scc..." << endl;
    map<int, int> def;
    map<int, int> low;
    map<int, bool> stack_sign;
    stack<int> st;
    vector<vector<int> > scc;

    for (auto &node : graph->nodes) {
        if (!node.second->tarjan) {
            tarjan(graph, node.second, def, low, st, scc, stack_sign);
        }
    }

//    cout << "Combining scc..." << endl;
    combine_scc_node(graph, scc);

//    cout << "Building 2-hops-label's data structure..." << endl;
    for (auto &node : graph->nodes) {
        if (node.first == node.second->data) {
            search_out_node(graph, node.second);
            search_in_node(graph, node.second);
        }
    }

    clock_t finish = clock();

//    cout << "Total time: " << (finish-start)/CLOCKS_PER_SEC << " seconds" << endl;
}


void tarjan(Graph *graph, Node *node, map<int, int> &dfn, map<int, int> &low, stack<int> &st,
            vector<vector<int> > &scc, map<int, bool> &inStack) {
    if (node == nullptr) {
        cout << "In tarjan function: the variable 'node' is nullptr" << endl;
        exit(1);
    }

    static int index = 0;
    // root vertex
    dfn[node->data] = low[node->data] = ++index;
    st.push(node->data);
    inStack[node->data] = true;
    node->tarjan = true;

    Edge *edge = node->firOut;
    while (edge != nullptr) {
        Node *next_node = graph->nodes[edge->tailVex];
        if (!next_node->tarjan) {
            tarjan(graph, next_node, dfn, low, st, scc, inStack);
            low[node->data] = min(low[node->data], low[next_node->data]);
        } else if (inStack[next_node->data]) {
            low[node->data] = min(low[node->data], dfn[next_node->data]);
        }

        edge = edge->headLink;
    }

    if (dfn[node->data] == low[node->data]) {
        vector<int> new_scc;
        while (true) {
            int top_data = st.top();
            inStack[top_data] = false;
            new_scc.push_back(top_data);
            st.pop();
            if (top_data == node->data) {
                break;
            }
        }
        scc.push_back(new_scc);
    }
}

void combine_scc_node(Graph *graph, vector<vector<int> > &scc) {
    for (auto &s : scc) {
        if (s.size() < 2) {
            continue;
        }
        int first = *(s.begin());
        map<int, bool> scc_sign;
        map<int, bool> accessed;
        for (auto &i: s)
            scc_sign[i] = true;

        for (auto beg = s.begin(); beg != s.end(); ++beg) {
            if (beg == s.begin()) {
                continue;
            }
            int r = *beg;
            for (Edge *edge = graph->nodes[r]->firOut; edge != nullptr;) {
                auto temp = edge;
                edge = edge->headLink;
                if (!scc_sign[temp->tailVex] && !accessed[temp->tailVex]) {
                    temp->headVex = first;
                    insert_edge(graph->nodes[first], temp);
                    accessed[temp->tailVex] = true;
                }
            }

            for (Edge *edge = graph->nodes[r]->firIn; edge != nullptr;) {
                auto temp = edge;
                edge = edge->tailLink;
                if (!scc_sign[temp->headVex] && !accessed[temp->headVex]) {
                    temp->tailVex = first;
                    insert_reverse_edge(graph->nodes[first], temp);
                    accessed[temp->headVex] = true;
                }
            }
            graph->nodes[r]->data = first;
            graph->nodes[r]->firIn = nullptr;
            graph->nodes[r]->firOut = nullptr;
        }
    }
}

void search_out_node(Graph *graph, Node *node) {
    queue<int> q;
    map<int, bool> accessed;
    Node *cur_node = node;

    for (Edge *edge = cur_node->firOut; edge != nullptr; edge = edge->headLink)
        q.push(edge->tailVex);

    while (!q.empty()) {
        cur_node = graph->nodes[q.front()];
        q.pop();

        if (node->data != cur_node->data && !cur_node->bfs && !query(graph, node->data, cur_node->data)) {
            for (Edge *edge = cur_node->firOut; edge != nullptr; edge = edge->headLink) {
                if (!accessed[edge->tailVex]) {
                    q.push(edge->tailVex);
                }
            }

            if (!accessed[cur_node->data]) {
                node->outNodes.push_back(cur_node->data);
                cur_node->inNodes.push_back(node->data);
                accessed[cur_node->data] = true;
            }
        }
    }

    node->bfs = true;
}

void search_in_node(Graph *graph, Node *node) {
    queue<int> q;
    map<int, bool> accessed;
    Node *cur_node = node;

    for (Edge *edge = cur_node->firIn; edge != nullptr; edge = edge->tailLink)
        q.push(edge->headVex);

    while (!q.empty()) {
        cur_node = graph->nodes[q.front()];
        q.pop();

        if (node->data != cur_node->data && !cur_node->reverse_bfs && !query(graph, node->data, cur_node->data)) {
            for (Edge *edge = cur_node->firIn; edge != nullptr; edge = edge->tailLink) {
                if (!accessed[edge->headVex]) {
                    q.push(edge->headVex);
                }
            }

            if (!accessed[cur_node->data]) {
                node->inNodes.push_back(cur_node->data);
                cur_node->outNodes.push_back(node->data);
                accessed[cur_node->data] = true;
            }
        }
    }

    node->reverse_bfs = true;
}

bool query(Graph *graph, int outNodeNum, int inNodeNum) {
    Node *outNode = graph->nodes[graph->nodes[outNodeNum]->data];
    Node *inNode = graph->nodes[graph->nodes[inNodeNum]->data];

    vector<int> out_vec(outNode->outNodes);
    out_vec.push_back(graph->nodes[outNodeNum]->data);
    sort(out_vec.begin(), out_vec.end());

    vector<int> in_vec(inNode->inNodes);
    in_vec.push_back(graph->nodes[inNodeNum]->data);
    sort(in_vec.begin(), in_vec.end());

    vector<int> result;

    set_intersection(in_vec.begin(), in_vec.end(),
                     out_vec.begin(), out_vec.end(),
                     back_inserter(result));

    return !result.empty();
}


// queryNaive
bool HopLabling::queryNaive(int outNodeNum, int inNodeNum, LabelSet ls) {
    stack<int> st = stack<int>({outNodeNum});

    map<int, bool> accessed;
    accessed[outNodeNum] = true;


    if (!isLabelSubset(this->G.nodes[outNodeNum]->ls,ls)) {
        return false;
    }

    while (!st.empty()) {
        // pop first num
        int vNum = st.top();
        st.pop();


        if (vNum == inNodeNum) {
            return true;
        }


        // add neighboor
        vector<int> neighbors = this->neighbors_with_diff_label(this->G.nodes[vNum], ls);
        for (auto neighbor: neighbors) {
            if (accessed[neighbor]) {
                continue;
            }
            st.push(neighbor);
            accessed[neighbor] = true;
            if (accessed[inNodeNum]) {
                return true;
            }
        }

        // for loop find l in ls
        for (auto l: this->labels) {
            // if l is not ls's subset continue
            if (!isLabelSubset(l, ls)) {
                continue;
            }
            // get graph
            Graph &cur_graph = this->G_set[l];


            // convert to neighbor in label set
            // for loop here u in L_out[label]
            if (cur_graph.nodes[vNum] == nullptr) {
                continue;
            }
            for (Edge *edge = cur_graph.nodes[cur_graph.nodes[vNum]->data]->firOut;
                 edge != nullptr; edge = edge->headLink) {
                int u = edge->tailVex;
                if (accessed[u]) {
                    continue;
                }
                // u in tail
                st.push(u);
                accessed[u] = true;
                if (accessed[inNodeNum]) {
                    return true;
                }
            }

        }

    }
    return false;
}


vector<int> HopLabling::neighbors_with_diff_label(Node *node, LabelSet ls) {
    vector<int> result;
    for (Edge *edge = node->firOut; edge != nullptr; edge = edge->headLink) {
        LabelSet cur_node_ls = this->G.nodes[edge->tailVex]->ls;
        //  current node label is a subset of label_set
        if (isLabelSubset(cur_node_ls, ls)) {
            result.push_back(edge->tailVex);
        }
    }

    return result;
}
