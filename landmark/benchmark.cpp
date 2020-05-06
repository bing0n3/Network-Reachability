//
// Created by Bingwen Ma on 5/5/20.
//

#include <LandmarkLabeling.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

vector<vector<int> > read_graph(string file_name) {
    string line;
    int V, E;
    fstream newfile;
    newfile.open(file_name,ios::in);
    stringstream ss;
    getline(newfile, line);
    ss << line;
    ss >> V;

    vector<vector<int> > G(V);

    for (int i = 0; i < V; i++) {
        stringstream ss;
        int n;
        getline(newfile, line);
        ss << line;

        while(ss >> n) {
            n;
            G[i].push_back(n);
        }
    }

    newfile.close();
    return G;
}

vector<LabelSet> read_label(string file_name) {
    string line;
    int V, E;
    fstream newfile;
    newfile.open(file_name,ios::in);
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
    }

    newfile.close();
    return labels;
}



int main() {

    cout << "Start to read file..." << endl;
    vector<vector<int>> graph;

    graph = read_graph("/Users/bing0ne/Dropbox/Dev/network/dataset/soc-advogato.edges_adj.data");
    vector<LabelSet> label_sets = read_label(
            "/Users/bing0ne/Dropbox/Dev/network/dataset/soc-advogato.edges_label.data");

    cout << "Finish ReadFile" << endl;

    LandmarkLabeling l =  LandmarkLabeling(graph, label_sets, 3);

    // node 1: 2,3
//    graph.push_back(vector<int>({1, 2}));
//    // node 2: 7,8
//    graph.push_back(vector<int>({6, 7}));
//    // node 3: 4
//    graph.push_back(vector<int>({3}));
//    // node 4: 5, 7
//
//    graph.push_back(vector<int>({4, 6}));
//    // node 5: 6
//    graph.push_back(vector<int>({5}));
//    //node 6: 4
//    graph.push_back(vector<int>({3}));
//    //node 7:
//    graph.push_back(vector<int>({}));
//    //node 8:
//    graph.push_back(vector<int>({}));
//
//    vector<LabelSet> labesets = vector<LabelSet>({
//       0b01, 0b01, 0b01, 0b11, 0b01, 0b01, 0b01, 0b01
//    });
//
//    LandmarkLabeling l =  LandmarkLabeling(graph, labesets, 3);



//    cout << l.query(0, 3, 0b11) << endl;
}
