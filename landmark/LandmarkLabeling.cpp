//
// Created by Bingwen Ma on 5/5/20.
//

#include "LandmarkLabeling.h"
#include <stack>
#include <algorithm>
#include <queue>
#include <iostream>



vector<pair<int, int>> findINOUTDegree(vector<vector<int>> adjList) {
    vector<int> in(adjList.size()), out(adjList.size());
    vector<pair<int, int>> total;

    for (int i = 0; i < adjList.size(); i++) {
        vector<int> list = adjList.at(i);
        out[i] = list.size();
        for (int j : list) {
            in[j]++;
        }
    }

    for (int i = 0; i < in.size(); i++) {
        total.push_back(make_pair(i, in[i] + out[i]));
    }

    return total;
}

LandmarkLabeling::LandmarkLabeling(vector<vector<int> > G, vector<LabelSet> L, int k){
    this->G_label = vector<LabelSet>(L);
    createIndex(G, L, k);
}
struct sort_pred {
    bool operator()(const std::pair<int,int> &left, const std::pair<int,int> &right) {
        return left.second > right.second;
    }
};

void LandmarkLabeling::LandmarkLabeling::createIndex(vector<vector<int> > &G, vector<LabelSet>& L, int k) {

    this->G = G;
    this->V = this->G.size();
    this->landmarks = vector<int>(this->V, -1);
    this->indexed = vector<bool>(this->V, false);
    vector<pair<int, int>> order = findINOUTDegree(G);
    sort(order.begin(), order.end(), sort_pred());

    this->Ind = vector<map<int, vector<LabelSet>>>(this->V);
    for (int i = 0; i < k; i++){

        const int start = order[i].first;
        this->Ind[start] = map<int, vector<LabelSet>>();
        this->landmarks[start] = i;
        // LabeledBFSPerVertex
        LabeledBFSPerVertex(start);
        auto& a = this->Ind[149];
//        cout << "build" << i << "th index" << endl;
    }
}

void LandmarkLabeling::LabeledBFSPerVertex(int s) {
    priority_queue<BitEntry, vector<BitEntry>, PQBitEntries> q;
    vector<map<LabelSet, bool>> accessded(this->V);

    BitEntry t;
    t.p = s;
    t.dist = 0;
    t.ls = 0;
    q.push(t);
    accessded[t.p][t.ls] = true;

    while (!q.empty()) {
        BitEntry tr = q.top();
        int v1 = tr.p;
        int ls1 = tr.ls;
        q.pop();

        if (s != v1 ) {

            if (!try_insert(s, v1, ls1)) {
                continue;
            }
        }
        // if v1 has been indexed forwadprop
        if (this->indexed[v1]) {
            //forwardprop
            this->forwardprop(s, v1, ls1);
            continue;
        }
        for (auto w:this->G[v1]) {
            if (w == s) {
                continue;
            }
            BitEntry t2;
            LabelSet ls2 = this->G_label[w];
            LabelSet ls3 =ls1 | ls2;
            int dist = tr.dist;

            if (ls3 != ls1 || ls3 != ls2) {
                dist += 1;
            }

            t2.dist = dist;

            t2.p = w;
            t2.ls = ls3;

            if (accessded[w].find(ls3) != accessded[w].end()) {
                // exist
                continue;
            }
            q.push(t2);
            accessded[w][ls3] = true;
        }
    }
    this->indexed[s] = true;
}


bool LandmarkLabeling::try_insert(int s, int v, LabelSet ls) {
    if (s == v) {
        return true;
    }

    if (this->Ind[s].find(v) != this->Ind[s].end()) {

        for (int k = 0; k < Ind[s][v].size(); k++) {
            LabelSet ls2 = this->Ind[s][v].at(k);
            bool b1 = isLabelSubset(ls, ls2);
            bool b2 = isLabelSubset(ls2, ls);
//            cout << "tryInsertLabelSet loop ls=" <<ls << ",ls2=" << *ls2 << endl;
//            cout << "tryInsertLabelSet loop b=" << b1 << ",b2=" << b2 << endl;
            if (b2) {
                return false;
            }
            if (!b1) {
                this->Ind[s][v].erase(this->Ind[s][v].begin()+k);
                k--;
            }
        }
    }

    this->Ind[s][v].push_back(ls);
    return true;
}

void LandmarkLabeling::forwardprop(int s, int v, LabelSet ls) {
    for (auto &tuple: this->Ind[v]) {
        int w = tuple.first;
        vector<LabelSet> ls_vector = tuple.second;
        for (auto ls2: ls_vector) {

            try_insert(s, w, ls | ls2);
        }
    }
}


bool LandmarkLabeling::query(int s, int t, LabelSet ls) {
    if (this->landmarks[s] != -1) {
        return queryLandmark(s, t, ls);
    }
    vector<bool> accessed(this->V, false);
    queue<int> qt;
    qt.push(s);

    while (!qt.empty()) {



        int v = qt.front();
        qt.pop();
        accessed[v] = true;
        if (v == t) {
            return true;
        }
        if (this->landmarks[v] != -1) {
            if (queryLandmark(v, t, ls)) {
                return true;
            }
            continue;
        }
        for(auto w: this->G[v]) {
            if (accessed[w]) {
                continue;
            }
            if (isLabelSubset(this->G_label[w], ls)) {
                qt.push(w);
            }
        }
    }
    return false;
}

bool LandmarkLabeling::queryLandmark(int s, int t, LabelSet ls){
    vector<LabelSet> ls_vec = this->Ind[s][t];
    for (const auto &ls2: ls_vec) {
        if(isLabelSubset(ls2, ls)) {
            return true;
        }
    }

    return false;
}

int LandmarkLabeling::getIndexSize() {

    int total =0;
    for (auto pair : this->Ind){
        for  (auto &kv: pair) {
            total += kv.second.size() * sizeof(LabelSet);
        }
    }
    return total;
}