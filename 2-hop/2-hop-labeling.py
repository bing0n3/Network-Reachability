class Edge(object):
    def __init__(self, headVex, tailVex):
        super().__init__()
        self.headVex = headVex
        self.tailVex = tailVex
        self.headLink = None
        self.tailLink = None


class Node(object):
    def __init__(self, data, attribute=None):
        super().__init__()

        self.tarjan = False
        self.bfs = False
        self.reverse_bfs = False
        self.data = data
        self.inNodes = []
        self.outNodes = []
        self.firIn = None
        self.firOut = None
        self.attribute = attribute


class Graph(object):
    def __init__(self):
        super().__init__()
        self.nodeNum = 0
        self.edgeNum = 0
        # <string, int>
        self.numberMap = {}
        # <int, string>
        self.nameMap = {}
        # <int, Node>
        self.nodes = {}


def insert_edge(node, edge):
    last_edge = node.firOut
    node.firOut = edge
    edge.headLink = last_edge


def insert_reverse_edge(node, edge):
    last_edge = node.firIn
    node.firIn = edge
    edge.tailLink = last_edge


def init_inexsistent_node(graph, node_num, attribute=None):
    if not graph.nodes[node_num]:
        graph.nodes[node_num] = Node(node_num, attribute=None)


def generate_scc(graph: Graph):
    defi = {}
    low = {}
    stack_sign = {}
    st = []
    scc = []
    for k, v in graph.nodes:
        if not v.tarjan:
            tarjan(graph, v, defi, low, st, scc, stack_sign)

    # Combining scc..
    combine_scc_node(graph, scc)

    # "Building 2-hops-label's data structure...
    for k, v in graph.nodes.items():
        if k == v.data:
            search_out_node(graph, v)
            search_in_node(graph, v)


def search(labeled_graph, outnodeID, innodeID):
    outNodeNum = labeled_graph.numberMap[outnodeID]
    inNodeNum = labeled_graph.numberMap[innodeID]
    return query(outNodeNum, inNodeNum)


def combine_scc_node(graph, scc):
    for s in scc:
        if len(s) < 2:
            continue
        first = scc[0]
        scc_sign = {}
        accessed = {}

        for i in s:
            scc_sign[i] = True

        for i, v in enumerate(s):
            if i == 0:
                continue
            r = v
            edge = graph.nodes[r].firOut
            while edge:
                tmp = edge
                edge = edge.headLink
                if not scc_sign[tmp.tailVex] and not accessed[tmp.tailVex]:
                    tmp.headVex = first
                    insert_edge(graph.nodes[first], tmp)
                    accessed[tmp.tailVex] = True

            edge = graph.nodes[r].firIn
            while edge:
                tmp = edge
                edge = edge.headLink
                if not scc_sign[tmp.tailVex] and not accessed[tmp.tailVex]:
                    tmp.headVex = first
                    insert_reverse_edge(graph.nodes[first], tmp)
                    accessed[tmp.tailVex] = True

            graph.nodes[r].data = first
            graph.nodes[r].firIn = None
            graph.nodes[r].firOut = None


def tarjan(graph, node, defi, low, st, scc, stack_sign):
    if not node:
        print("In tarjan function: the variable 'node' is nullptr")
        exit(1)

    index = 0
    index += 1
    defi[node.data] = low[node.data] = index
    st.append(node.data)
    stack_sign[node.data] = True
    node.tarjan = True

    edge = node.firOut
    while edge is not None:
        next_node = graph.nodes[edge.tailVex]
        if not next_node.tarjan:
            tarjan(next_node, defi, low, st, scc, stack_sign)
            low[node.data] = min(low[node.data], low[next_node.data])
        elif stack_sign[next_node.data]:
            low[node.data] = min(low[node.data], defi[next_node.data])

        edge = edge.headLink

    if defi[node.data] == low[node.data]:
        new_scc = []
        while True:
            top_data = st.pop()
            stack_sign[top_data] = False
            new_scc.append(top_data)

            if top_data == node.data:
                break

        scc.append(new_scc)


def search_out_node(graph, node):
    q = []
    accessed = {}
    cur_node = node

    edge = cur_node.firOut
    while edge:
        q.append(edge.tailVex)
        edge = edge.headLink

    while q:
        cur_node = graph.nodes[q[0]]
        q.pop(0)

        if (
            node.data != cur_node.data
            and not cur_node.bfs
            and not query(graph, node.data, cur_node.data)
        ):
            edge = cur_node.firOut
            while edge:
                if not accessed[edge.tailVex]:
                    q.append(edge.tailVex)
                edge = edge.headLink

            if not accessed[cur_node.data]:
                node.outNodes.append(cur_node.data)
                cur_node.inNodes.append(node.data)
                accessed[cur_node.data] = True
    node.bfs = True


def search_in_node(graph, node):
    q = []
    accessed = {}
    cur_node = node

    edge = cur_node.firIn
    while edge:
        q.append(edge.headVex)
        edge = edge.tailLink

    while q:
        cur_node = graph.nodes[q.front()]
        q.pop(0)
        if (
            node.data != cur_node.data
            and not cur_node.reverse_bfs
            and not query(graph, node.data, cur_node.data)
        ):
            edge = cur_node.firIn
            while edge:
                if not accessed[edge.headVex]:
                    q.append(edge.headVex)
                edge = edge.tailLink

            if not accessed[cur_node.data]:
                node.inNodes.append(cur_node.data)
                cur_node.outNodes.append(node.data)
                accessed[cur_node.data] = True

    node.reverse_bfs = True


def query(graph, outnodeNum, inNodeNum):
    outNode = graph.nodes[graph.nodes[outnodeNum].data]
    inNode = graph.nodes[graph.nodes[inNodeNum].data]

    out_vec = outNode.outNodes
    out_vec.append(graph.nodes[outnodeNum].data)

    sorted(out_vec)

    in_vec = inNode.inNodes
    in_vec.append(graph.nodes[inNodeNum].data)
    sorted(in_vec)

    result = intersection(in_vec, out_vec)

    return not len(result) == 0


def intersection(lst1, lst2):

    # Use of hybrid method
    temp = set(lst2)
    lst3 = [value for value in lst1 if value in temp]
    return lst3
