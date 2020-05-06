//
// Created by Bingwen Ma on 5/5/20.
//

#include <LandmarkLabeling.h>
#include <vector>
#include <iostream>

using namespace std;

int main() {
    vector<vector<int>> graph;

    // node 1: 2,3
    graph.push_back(vector<int>({1, 2}));
    // node 2: 7,8
    graph.push_back(vector<int>({6, 7}));
    // node 3: 4
    graph.push_back(vector<int>({3}));
    // node 4: 5, 7
    graph.push_back(vector<int>({4, 6}));
    // node 5: 6
    graph.push_back(vector<int>({5}));
    //node 6: 4
    graph.push_back(vector<int>({3}));
    //node 7:
    graph.push_back(vector<int>({}));
    //node 8:
    graph.push_back(vector<int>({}));

    vector<LabelSet> labesets = vector<LabelSet>({
       1, 1, 1, 1, 4, 1, 1, 1
    });

    LandmarkLabeling l =  LandmarkLabeling(graph, labesets, 2);



    cout << l.query(0, 5, 1|4) << endl;
}

