//
// Created by Bingwen Ma on 5/5/20.
//

#include "LandmarkLabeling.h"
#include <stack>
#include <algorithm>
#include <queue>
#include <iostream>



vector<int> findINOUTDegree(vector<vector<int>> adjList) {
    vector<int> in(adjList.size()), out(adjList.size()), total(adjList.size());

    for (int i = 0; i < adjList.size(); i++) {
        vector<int> list = adjList.at(i);
        out[i] = list.size();
        for (int j : list) {
            in[j]++;
        }
    }

    for (int i = 0; i < in.size(); i++) {
        total[i] = in[i] + out[i];
    }

    return total;
}

LandmarkLabeling::LandmarkLabeling(vector<vector<int> > G, vector<LabelSet> L, int k){
    this->G_label = vector<LabelSet>(L);
    createIndex(G, L, k);
}


void LandmarkLabeling::LandmarkLabeling::createIndex(vector<vector<int> > &G, vector<LabelSet>& L, int k) {

    this->G = G;
    this->V = this->G.size();
    this->landmarks = vector<int>(this->V, -1);
    this->indexed = vector<bool>(this->V, false);
    vector<int> order = findINOUTDegree(G);
    sort(order.begin(), order.end(), greater<>());

    this->Ind = vector<map<int, vector<LabelSet>>>(this->V);
    for (int i = 0; i < k; i++){
        cout << i << endl;
        const int start = order[i];
        this->Ind[start] = map<int, vector<LabelSet>>();
        this->landmarks[start] = i;
        // LabeledBFSPerVertex
        LabeledBFSPerVertex(start);
    }
}

void LandmarkLabeling::LabeledBFSPerVertex(int s) {
    priority_queue<BitEntry, vector<BitEntry>, PQBitEntries> q;
    BitEntry t;
    t.p = s;
    t.ls = 0;
    q.push(t);

    while (!q.empty()) {
        BitEntry tr = q.top();
        int v1 = tr.p;
        int ls1 = tr.ls;
        q.pop();

        if (!try_insert(s, v1, ls1)) {
            continue;
        }
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
            t2.p = w;
            t2.ls = ls1 | this->G_label[w];
            q.push(t2);
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
            // join labelset
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