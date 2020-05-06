# Fast Estimation of Reachability with Label Constraint

Fast Estimation of Reachability with Label Constraints 

Input: A node-labeled directed graph G, a pair of nodes s and t in G, and a set of node labels L, Q(s, t, G, L).

Output “yes” if there exists a path from s to t, and all the nodes it passes has a label from the set L.


The most of Label Constrainted Reachbility Query Algotihms are designed to solve edge-labeled problem. In this project, we focus on solve node-labled problem. We proposed two soultion based on 2-hop cover and landmark index:

**2-hop**: a eact exact algorithm and make use of 2-hop cover/index to optimize the query.

**landmark index**: an approximate query processing strategy use landmark index to
opoptimize

## References

- Valstar, Lucien DJ, George HL Fletcher, and Yuichi Yoshida. "Landmark indexing for evaluation of label-constrained reachability queries." In Proceedings of the 2017 ACM International Conference on Management of Data, pp. 345-358. 2017.
  Harvard
- Reachability and Distance Queries via 2-Hop Labels
- [5kbpers/2-hop-labels](https://github.com/5kbpers/2-hop-labels)

