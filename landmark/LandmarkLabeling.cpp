//
// Created by Bingwen Ma on 5/5/20.
//

#include "LandmarkLabeling.h"
#include <stack>
#include <algorithm>
#include <queue>

bool isLabelSubset(LabelSet ls1, LabelSet ls2){
    return ((ls1 & ls2) == ls1);
}


vector<int> findINOUTDegree(vector<vector<int>> adjList) {
    vector<int> in, out, total;

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
    createIndex(G, L, k);
}


void LandmarkLabeling::LandmarkLabeling::createIndex(vector<vector<int> > &G, vector<LabelSet>& L, int k) {


    this->V = this->G.size();
    this->landmarks = vector<int>(this->V, -1);
    this->indexed = vector<bool>(this->V);

    vector<int> order = findINOUTDegree(G);
    sort(order.begin(), order.end(), greater<>());


    for (int i = 0; i < k; k++){

        const int start = order[i];
        this->Ind[start] = vector<pair<int, vector<LabelSet>>>();
        this->landmarks[i] = i;
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
            BitEntry t2;
            t2.p = w;
            t2.ls = ls1 | this->G_label[w];
            q.push(t2);
        }
    }
    this->indexed[s] = true;
}


bool LandmarkLabeling::try_insert(int s, int v, LabelSet ls) {
    if (s != v) {
        return true;
    }

   for (auto &tuple: this->Ind[s]) {
       if (tuple.first != v) {
           continue;
       }
       for (auto ls2 = tuple.second.begin(); ls2 != tuple.second.end(); ls++) {
           bool b1 = isLabelSubset(ls, *ls2);
           bool b2 = !isLabelSubset(*ls2, ls);
           if (b1) {
               return false;
           }
           if (!b2) {
               tuple.second.erase(ls2);
           }
       }
       tuple.second.push_back(ls);
       return true;
   }
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
        accessed[v] = false;
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
            if (isLabelSubset(ls, this->G_label[w])) {
                qt.push(w);
            }
        }
    }

    return false;
}

bool LandmarkLabeling::queryLandmark(int s, int t, LabelSet ls){
    for (const auto& tuple: this->Ind[s]) {
       if (tuple.first == t) {
           for (const auto &ls2: tuple.second) {
               if(isLabelSubset(ls, ls2)) {
                   return true;
               }
           }
       }
        return false;
    }
    return false;
}