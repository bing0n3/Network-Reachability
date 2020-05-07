#include "2-hop-labels.h"
#include <sstream>


// constructor for graphlist for each label/
// build 2 hop index for each label



int main(int argc, char *argv[]) {
    if (argv[1] == nullptr) {
        cout << "Usage: ./a.out <filename>" << endl;
        exit(1);
    }

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
                // initial node with label
                if (graphsList.graphs.find(arr[2]) == graphsList.graphs.end()) {
                    Graph new_graph =  Graph();
                    graphsList.graphs[arr[2]] = new_graph;
                    init_inexistent_label_node(graphsList.graphs[arr[2]], stoi(arr[1]), label);
                } else {
                    init_inexistent_label_node(graphsList.graphs[arr[2]], stoi(arr[1]), label);
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
    clock_t start = clock();
    generate_one_label_index(graphsList);
    clock_t finish = clock();
    cout << "Total time: " << float( (finish-start)) /CLOCKS_PER_SEC<< " seconds" << endl;

//    vector<string> ls({"0", "1", "3", "4"});
//    constrainedQuery(graph, graphsList, 4180, 3294, ls);

    start = clock();

    stringstream ss;
    stringstream fs;
    stringstream rs;
    auto q_set = read_query("/Users/bing0ne/Dropbox/Dev/network/dataset/soc-advogato.edges_query_5");
    int i = 0;
    for (; i < q_set.size(); i++) {
        if (i % 1000 == 0) {
            finish = clock();
            ss << "Run " << i <<  " Total time: " << float( (finish-start)) /CLOCKS_PER_SEC<< " seconds\n";
            fs << i << " " << float((finish - start)) / CLOCKS_PER_SEC << "\n";
        }
        pair<int, int> q_pair = q_set[i].first;
        vector<string> ls = q_set[i].second;

        rs  << constrainedQuery(graph, graphsList, q_pair.first, q_pair.second, ls) << "\n";
    }
    finish = clock();
    ss << "Run " << i <<  " Total time: " << float( (finish-start)) /CLOCKS_PER_SEC<< " seconds\n";
    fs << i << " " << float((finish - start)) / CLOCKS_PER_SEC << "\n";
    cout << ss.str() << endl;

    fstream result_filel;
    result_filel.open("soc-advogato-2-hop.txt", ios::out);
    result_filel << fs.str();
    result_filel.close();

    fstream rs_f;
    rs_f.open("soc-advogato-2-hop.result", ios::out);
    rs_f << rs.str();
    rs_f.close();

}

void generate_one_label_index(LabeledGraphList &graphs) {
    int total = 0;
    for (auto& graph: graphs.graphs) {
        two_hop_label(&graph.second);
        total += cal_graph_size(graph.second);
    }
    cout << "Index size " <<total << endl;
}

int cal_graph_size(Graph& graph) {
    int total = 0;

    for (auto& cur_node: graph.nodes) {
        for (Edge *edge = cur_node.second->firOut; edge != nullptr; edge = edge->headLink)
            total += sizeof(int);
    }

    return total;
}


void two_hop_label(Graph* graph) {
    clock_t start = clock();
//    cout << "Finding scc..." << endl;
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

//    cout << "Combining scc..." << endl;
    combine_scc_node(graph, scc);

//    cout << "Building 2-hops-label's data structure..." << endl;
    for (auto & node : graph->nodes) {
        if (node.first == node.second->data) {
            search_out_node(graph, node.second);
            search_in_node(graph, node.second);
        }
    }

    clock_t finish = clock();

//    cout << "Total time: " << (finish-start)/CLOCKS_PER_SEC << " seconds" << endl;
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

            //whether vertex exist in this graph
            if(cur_graph.nodes.find(vNum) == cur_graph.nodes.end()) {
                continue;
            }


            vector<int> neighbors = neighbors_with_diff_label(graph, graph.nodes[vNum], labels);

            for (auto neighbor: neighbors) {
                if(!accessed[neighbor]) {
                    accessed[neighbor] = true;
                    st.push(neighbor);
                }
            }

            // consider combined ssc
            if (vNum != cur_graph.nodes[vNum]->data) {
                accessed[cur_graph.nodes[vNum]->data] = true;

                vector<int> neibors = neighbors_with_diff_label(graph, graph.nodes[cur_graph.nodes[vNum]->data], labels);
                for (auto neighbor: neibors ){
                    if(!accessed[neighbor]) {
                        accessed[neighbor] = true;
                        st.push(neighbor);
                    }
                }

            }

            Node *outNode = cur_graph.nodes[cur_graph.nodes[vNum]->data];

            vector<int> out_vec(outNode->outNodes);

            if(cur_graph.nodes.find(inNodeNum) !=cur_graph.nodes.end() && accessed[cur_graph.nodes[inNodeNum]->data]) {
                return true;
            }

            for (auto vec : out_vec) {
                if(!accessed[vec]) {
                    accessed[vec] = true;
                    st.push(vec);
                }
                if(cur_graph.nodes.find(inNodeNum) !=cur_graph.nodes.end() && accessed[cur_graph.nodes[inNodeNum]->data]) {
                    return true;
                }
            }
        }

    }
    return false;
}


vector<int> neighbors_with_diff_label(Graph& graph, Node* node,   vector<string> &label_set){
    map<int, bool> accessed;
    vector<int>  result;
    vector<string> node_attributes(node->attributes);

    vector<string> lap, in_set;

    for (Edge *edge = node->firOut; edge != nullptr; edge = edge->headLink) {

        // is label has same lap
        set_intersection(node_attributes.begin(), node_attributes.end(),
                         graph.nodes[edge->tailVex]->attributes.begin(), graph.nodes[edge->tailVex]->attributes.end(),
                         back_inserter(lap));

        // is label in set?
        set_intersection(label_set.begin(), label_set.end(),
                        graph.nodes[edge->tailVex]->attributes.begin(), graph.nodes[edge->tailVex]->attributes.end(),
                        back_inserter(in_set));
        if (!accessed[edge->tailVex] && lap.empty() && !in_set.empty() ) {
            result.push_back(edge->tailVex);
        }
    }

    return result;
}



vector<pair<pair<int, int>,  vector<string>>> read_query(string file_name){
    string line;
    int V, E;
    fstream newfile;
    newfile.open(file_name,ios::in);

    vector<pair<pair<int, int>, vector<string>>> query_set = vector<pair<pair<int, int>, vector<string>>>();

    for (int i = 0; i < 10000; i++) {
        int q1, q2;
        string n;
        stringstream ss;
        getline(newfile, line);
        ss << line;
        ss >> q1 >> q2;
        pair<int, int> q_pair = make_pair(q1, q2);
        vector<string> ls;
        while(ss >> n) {
            ls.push_back(n);
        }
        query_set.emplace_back(q_pair, ls);
    }

    newfile.close();
    return query_set;
}