# Network-Reachability

Fast Estimation of Reachability with Label Constraints Input: A node-labeled directed graph G, a pair of nodes s and t in G, and a set of node labels L.

Question: Design an algorithm to answer the reachability query Q(s, t, G, L), which output “yes” if there exists a path from s to t, and all the nodes it passes has a label from the set L.

2-hop: a eact exact algorithm and make use of 2-hop cover/index to optimize the query.

landmark index: an approximate query processing strategy use landmark index to
opoptimize

## References

- Valstar, Lucien DJ, George HL Fletcher, and Yuichi Yoshida. "Landmark indexing for evaluation of label-constrained reachability queries." In Proceedings of the 2017 ACM International Conference on Management of Data, pp. 345-358. 2017.
  Harvard
- Reachability and Distance Queries via 2-Hop Labels
- [5kbpers/2-hop-labels](https://github.com/5kbpers/2-hop-labels)

