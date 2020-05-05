#include "2-hop-labels.h"
#include <sstream>


// constructor for graphlist for each label/
// build 2 hop index for each label



int main(int argc, char *argv[]) {
    if (argv[1] == nullptr) {
        cout << "Usage: ./a.out <filename>" << endl;
        exit(1);
    }

//    Graph graph = Graph();
//
//    ifstream fin(argv[1]);
//
//    cout << "Loading data from file " << argv[1] << "..." << endl;
//    fin >> graph.nodeNum >> graph.edgeNum;
//
//    int numberCount = 0;
//    string inNodeID, outNodeID;
//    int inNodeNum, outNodeNum;
//    Node *inNode = nullptr, *outNode = nullptr;
//
//
//    for (int j = 0; j < graph.edgeNum; j++) {
//        fin >> outNodeID >> inNodeID;
//        if (graph.numberMap[outNodeID] == 0) {
//            outNodeNum = graph.numberMap[outNodeID] = ++numberCount;
//            graph.nameMap[outNodeNum] = outNodeID;
//        } else {
//            outNodeNum = graph.numberMap[outNodeID];
//        }
//
//        if (graph.numberMap[inNodeID] == 0) {
//            inNodeNum = graph.numberMap[inNodeID] = ++numberCount;
//            graph.nameMap[inNodeNum] = inNodeID;
//        } else {
//            inNodeNum = graph.numberMap[inNodeID];
//        }
//
//        outNode = init_inexistent_node(graph, outNodeNum);
//        inNode = init_inexistent_node(graph, inNodeNum);
//        Edge *edge = new Edge(outNodeNum, inNodeNum);
//
//        insert_edge(outNode, edge);
//        insert_reverse_edge(inNode, edge);
//    }
//
//    // generate labeling
//    two_hop_label(&graph);
//    input_and_query(&graph);
//

    // with label
//    LabeledGraphList lableling_graphs =  LabeledGraphList(vector<string>({"1", "2", "3"}));
//    vector<string> labels({"1", "2"});
//    generata_one_label_index(lableling_graphs);
//    constrainedQuery(graph, lableling_graphs, 1, 2, labels);
//    cout << "hi";
    read_labeling_graph(argv[1]);
}

void read_labeling_graph(char *argv) {
    Graph graph;
    Node *inNode = nullptr, *outNode = nullptr;
    LabeledGraphList graphsList;
    fstream newfile;
    newfile.open(argv, ios::in);
    cout << "Loading data from file " << argv << "..." << endl;
    if (newfile.is_open()) {
        string tp;
        while (getline(newfile, tp)) {
            stringstream ssin(tp);

            string arr[3];
            int i = 0;
            while (ssin.good() && i < 3){
                ssin >> arr[i];
                ++i;
            }

            if (arr[0] == "v") {
                vector<string> label({arr[2]});
//                cout << arr[0] << endl;
                // initial node with label
                if (graphsList.graphs.count(arr[2]) == 0) {
                    Graph new_graph =  Graph();
                    graphsList.graphs[arr[2]] = new_graph;
                    init_inexistent_label_node(new_graph, stoi(arr[1]), label);
                    init_inexistent_label_node(new_graph, stoi(arr[2]), label);
                } else {
                    init_inexistent_label_node(graphsList.graphs[arr[2]], stoi(arr[1]), label);
                    init_inexistent_label_node(graphsList.graphs[arr[2]], stoi(arr[2]), label);
                }
                init_inexistent_label_node(graph, stoi(arr[1]), label);
            } else if (arr[0] == "e") {
                // add edge to graph
                int outNodeNum = stoi(arr[1]);
                int inNodeNum = stoi(arr[2]);

                outNode = graph.nodes[outNodeNum];
                inNode = graph.nodes[inNodeNum];

                Edge *edge = new Edge(outNodeNum , inNodeNum);
                insert_edge(graph.nodes[outNodeNum], edge);
                insert_reverse_edge(graph.nodes[inNodeNum], edge);
                // iterate labels

                vector<string> out_vec(outNode->attributes);
                vector<string> in_vec(inNode->attributes);
                vector<string> result;

                set_intersection(in_vec.begin(), in_vec.end(),
                        out_vec.begin(), out_vec.end(),
                        back_inserter(result));


                if (!result.empty()) {
                    for (const auto& label: result) {
                        edge = new Edge(outNodeNum , inNodeNum);
                        insert_edge(graphsList.graphs[label].nodes[outNodeNum], edge);
                        insert_reverse_edge(graphsList.graphs[label].nodes[inNodeNum], edge);
                    }
                }
            }
        }
    }
//    Graph *old_graph = &graph;
//    two_hop_label(&graph);
    generata_one_label_index(graphsList);
    vector<string> queryLabel({"1"});
    for (auto const &pair: graphsList.graphs) {
        cout << pair.first << " ";
    }
    cout << endl << "Result is " << constrainedQuery(graph, graphsList, 5, 6, queryLabel) << endl << "??";
//    cout << query(&graphsList.graphs["1"], 4, 6) << endl;

}

void generata_one_label_index(LabeledGraphList &graphs) {
    for (auto& graph: graphs.graphs) {
        two_hop_label(&graph.second);
    }
}

void two_hop_label(Graph* graph) {
    clock_t start = clock();
    cout << "Finding scc..." << endl;
    map<int, int> def;
    map<int, int> low;
    map<int, bool> stack_sign;
    stack<int> st;
    vector<vector<int> > scc;

    for (auto & node : graph->nodes) {
        if (!node.second->tarjan) {
            tarjan(graph, node.second, def, low, st, scc, stack_sign);
        }
    }

    cout << "Combining scc..." << endl;
    combine_scc_node(graph, scc);

    cout << "Building 2-hops-label's data structure..." << endl;
    for (auto & node : graph->nodes) {
        if (node.first == node.second->data) {
            search_out_node(graph, node.second);
            search_in_node(graph, node.second);
        }
    }

    clock_t finish = clock();

    cout << "Total time: " << (finish-start)/CLOCKS_PER_SEC << " seconds" << endl;
}


void input_and_query(Graph* graph) {
    cout << "Please enter query data(e.g. from 1 to 2 => 1 2)" << endl;
    int inNodeNum, outNodeNum;
    string inNodeID, outNodeID;
    while (cin >> outNodeID >> inNodeID) {
        outNodeNum = graph->numberMap[outNodeID];
        inNodeNum = graph->numberMap[inNodeID];

        cout << query(graph, outNodeNum, inNodeNum) << endl;
        cout << "Please enter query data(e.g. from 1 to 2 => 1 2)" << endl;
    }
}

void tarjan(Graph* graph, Node *node, map<int, int> &dfn, map<int, int> &low, stack<int> &st,
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

void combine_scc_node(Graph* graph, vector<vector<int> > &scc) {
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
bool constrainedQuery(Graph &graph, LabeledGraphList &graphs, int outNodeNum, int inNodeNum, vector<string>& labels) {
    stack<int> st = stack<int>({outNodeNum});
    map<int,bool> accessed;
    accessed[graph.nodes[outNodeNum]->data] = true;

    while(!st.empty()) {
        int vNum = st.top();
        st.pop();

        for (auto label : labels) {
            // get graph
            Graph &cur_graph = graphs.graphs[label];
            Node *outNode = cur_graph.nodes[cur_graph.nodes[outNodeNum]->data];

            vector<int> out_vec(outNode->outNodes);

            for (auto vec : out_vec) {
                if(!accessed[vec]) {
                    accessed[vec] = true;
                    st.push(vec);
                }

                if(accessed[cur_graph.nodes[inNodeNum]->data]) {
                    return true;
                }
            }
        }

    }
    return false;
}