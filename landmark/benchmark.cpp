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


vector<pair<pair<int, int>,  LabelSet>> read_query(string file_name){
    string line;
    int V, E;
    fstream newfile;
    newfile.open(file_name,ios::in);

    vector<pair<pair<int, int>, LabelSet>> query_set = vector<pair<pair<int, int>, LabelSet>>();

    for (int i = 0; i < 100000; i++) {
        int q1, q2;
        unsigned int n;
        stringstream ss;
        getline(newfile, line);
        ss << line;
        ss >> q1 >> q2;
        pair<int, int> q_pair = make_pair(q1, q2);
        LabelSet ls;
        while(ss >> n) {
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
    vector<LabelSet> label_sets = read_label(
            prefix + "-label.data");

    cout << "Finish ReadFile" << endl;

    clock_t start = clock();

    LandmarkLabeling l =  LandmarkLabeling(graph, label_sets, 1250 + 163);
    clock_t end = clock();
    cout << "Total time: " << float( (end-start)) /CLOCKS_PER_SEC<< " seconds" << endl;

    cout << "Get Index Size" << l.getIndexSize()  << endl;
    cout << l.query(149, 3, labelIDToLabelSet(0)) << endl;
    cout << l.query(149, 3, labelIDToLabelSet(0)|labelIDToLabelSet(4)|labelIDToLabelSet(3)|labelIDToLabelSet(2)|labelIDToLabelSet(1)) << endl;

    vector<pair<pair<int, int>, LabelSet>> q_set = read_query(
            prefix + "-query-5");


    start = clock();


    stringstream ss;
    stringstream fs;
    stringstream rs;

    int i = 0;
    for (; i < q_set.size(); i++) {
        if (i % 1000 == 0) {
            clock_t end = clock();
            ss << "Run " << i <<  " Total time: " << float( (end-start)) /CLOCKS_PER_SEC<< " seconds\n";
            fs << i << " " << float((end - start)) / CLOCKS_PER_SEC << "\n";
        }
        pair<int, int> q_pair = q_set[i].first;
        LabelSet ls = q_set[i].second;
        rs << l.query(q_pair.first, q_pair.second, ls) << "\n";
    }
    end = clock();
    ss << "Run " << i <<  " Total time: " << float( (end-start)) /CLOCKS_PER_SEC<< " seconds\n";
    fs << i << " " << float((end - start)) / CLOCKS_PER_SEC << "\n";
    cout << ss.str() << endl;

    fstream result_filel;
    result_filel.open("landmark-"+prefix+".time", ios::out);
    result_filel << fs.str();
    result_filel.close();

    result_filel.open("landmark-"+ prefix +".result", ios::out);
    result_filel << rs.str();
    result_filel.close();
}
