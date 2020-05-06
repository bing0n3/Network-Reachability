//
// Created by Bingwen Ma on 5/5/20.
//

#ifndef NETWORK_LANDMARKLABELING_H
#define NETWORK_LANDMARKLABELING_H

#include <vector>
#include <map>

using namespace std;

typedef unsigned int LabelSet;

class LandmarkLabeling {
private:
    vector<vector<int> > G;
    vector<LabelSet> G_label;
    long long V;
    vector<map<int, vector<LabelSet>>> Ind;
    vector<bool> indexed;
    vector<int> landmarks;
    void LabeledBFSPerVertex(int);
    bool try_insert(int, int, LabelSet);
    void forwardprop(int s, int v, LabelSet ls);
public:
    LandmarkLabeling();
    LandmarkLabeling(vector<vector<int> >, vector<LabelSet>, int);
//    virtual ~LandmarkLabeling();
    virtual void createIndex(vector<vector<int> >&, vector<LabelSet>&, int );
    virtual bool query(int, int, LabelSet);
    virtual bool queryLandmark(int, int, LabelSet);
};



inline int label_number_count(LabelSet ls){
    int c;
    for ( c =0; ls; ls >>=1) // 循环移位
        c += ls &1 ; // 如果当前位是1，则计数器加1
    return c;
}

typedef struct
{
    LabelSet ls;
    int p;
}BitEntry;

struct PQBitEntries
{
    bool operator()(BitEntry const & t1, BitEntry const & t2)
    {
        // return "true" if "p1" is ordered before "p2", for example:
        return label_number_count(t1.ls) > label_number_count(t2.ls);
    }
};



void getSCCGraph(vector<vector<int> > &G, vector<vector<int> > *SCCGraph, vector<int> *table);


// is  l2 is a subset of l1
inline bool isLabelSubset(LabelSet ls1, LabelSet ls2){
    return ((ls2 & ls1) == ls2);
}
#endif //NETWORK_LANDMARKLABELING_H
