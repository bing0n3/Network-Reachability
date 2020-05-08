//
// Created by Bingwen Ma on 5/7/20.
//

#include "2-hop-labels.h"

#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

vector<vector<int>> read_graph(string file_name) {
    string line;
    int V, E;
    fstream newfile;
    newfile.open(file_name, ios::in);
    stringstream ss;
    getline(newfile, line);
    ss << line;
    ss >> V;

    vector<vector<int>> G(V);

    for (int i = 0; i < V; i++) {
        stringstream ss;
        int n;
        getline(newfile, line);
        ss << line;

        while (ss >> n) {
            G[i].push_back(n);
        }
    }

    newfile.close();
    return G;
}

vector<LabelSet> read_label(string file_name, set<LabelSet> &kinds) {
    string line;
    int V, E;
    fstream newfile;
    newfile.open(file_name, ios::in);
    stringstream ss;
    getline(newfile, line);
    ss << line;
    ss >> V;

    vector<LabelSet> labels(V);

    for (int i = 0; i < V; i++) {
        unsigned int n;
        stringstream ss;
        getline(newfile, line);
        ss << line;
        ss >> n;
        labels[i] = labelIDToLabelSet(n);
        kinds.insert(labels[i]);
    }

    newfile.close();
    return labels;
}


vector<pair<pair<int, int>, LabelSet>> read_query(string file_name) {
    string line;
    int V, E;
    fstream newfile;
    newfile.open(file_name, ios::in);

    vector<pair<pair<int, int>, LabelSet>>
            query_set = vector<pair<pair<int,
            int>, LabelSet>>();

    for (int i = 0;i < 10000; i++) {
        int q1, q2;
        unsigned int n;
        stringstream ss;
        getline(newfile, line
        );
        ss <<line;
        ss >> q1 >>q2;
        pair<int, int> q_pair = make_pair(q1, q2);
        LabelSet ls;
        while (ss >> n) {
            ls = ls | labelIDToLabelSet(n);
        }
        query_set.emplace_back(q_pair, ls);
    }

    newfile.close();
    return query_set;
}

int main(int argc, char *argv[]) {
    if (argv[1] == nullptr) {
        cout << "Usage: ./a.out <filename>" << endl;
        exit(1);
    }

    string prefix = argv[1];

    cout << "Start to read file..." << endl;
    vector<vector<int>> graph;

    graph = read_graph(prefix + "-adj.data");
    set<LabelSet> kinds;
    vector<LabelSet> label_sets = read_label(
            prefix + "-label.data", kinds);

    cout << "Finish ReadFile" << endl;


    HopLabling l = HopLabling(graph, label_sets, vector<LabelSet>(kinds.begin(), kinds.end()));


    vector<pair<pair<int, int>, LabelSet>> q_set = read_query(
            prefix + "-query-5");

    int size = l.cal_graph_index_size();
    cout << "Total Size is " << size  << endl;


    time_t start = clock();

    stringstream ss;
    stringstream fs;
    stringstream rs;

    cout << "Starting query..." << endl;

    int i = 0;
    for (; i < q_set.size(); i++) {
        if (i % 1000 == 0) {
            clock_t end = clock();
            ss << "Run " << i <<  " Total time: " << float( (end-start)) /CLOCKS_PER_SEC<< " seconds\n";
            fs << i << " " << float((end - start)) / CLOCKS_PER_SEC << "\n";
        }
        pair<int, int> q_pair = q_set[i].first;
        LabelSet ls = q_set[i].second;
        rs << l.queryNaive(q_pair.first, q_pair.second, ls) << "\n";
    }
    time_t end = clock();
    ss << "Run " << i <<  " Total time: " << float( (end-start)) /CLOCKS_PER_SEC<< " seconds\n";


    fs << i << " " << float((end - start)) / CLOCKS_PER_SEC << "\n";
    cout << ss.str() << endl;


    fstream result_filel;
    result_filel.open("2-hop-"+prefix+".time", ios::out);
    result_filel << fs.str();
    result_filel.close();

    result_filel.open("2-hop-"+prefix+".result", ios::out);
    result_filel << rs.str();
    result_filel.close();
}

