import random
import math


def read_file(file_name, L):
    v_dict = {}
    adjList = {}
    edgeList = []
    labelList = []
    countNum = 0
    with open(file_name) as f:
        while True:
            line = f.readline()
            if not line:
                break
            if line.startswith("%"):
                continue
            vs = line.strip().split(" ")

            first = vs[0]
            second = vs[1]
            if first == second:
                continue

            countNum = try_insert(v_dict, vs[0], countNum)
            countNum = try_insert(v_dict, vs[1], countNum)

            # build adjList
            if v_dict[first] not in adjList.keys():
                adjList[v_dict[first]] = []

            adjList[v_dict[first]].append(v_dict[second])

            # add to edge list
            edgeList.append([v_dict[first], v_dict[second]])

        # print(v_dict, countNum)
        gen_adj_list(countNum, adjList, file_name)
        gen_label_list(countNum, L, file_name)


def gen_for_2_hop():
    pass


def gen_adj_list(countNum, adjList, filename):
    lists = []

    for i in range(countNum):
        if i in adjList.keys():
            lists.append(adjList[i])
        else:
            lists.append([])

    with open(filename + "_adj.data", "w") as f:
        f.write("%d\n" % countNum)
        for l in lists:
            out_str = " ".join(str(x) for x in l)
            f.write("%s\n" % out_str)
    return lists


def gen_label_list(vertex_num, L, filename):
    random.seed()
    mean = int(math.floor(L / 2))
    sd = int(max(1, math.floor(L / 4)))

    v_l = []

    for i in range(vertex_num):
        label = random.normalvariate(mean, sd)
        label = max(0, label)
        label = min(L - 1, label)
        label = int(math.floor(label))
        v_l.append(label)

    with open(filename + "_label.data", "w") as f:
        f.write("%d\n" % vertex_num)
        for c_l in v_l:
            f.write("%d\n" % c_l)

    return v_l


def gen_query(countNum, L):

    pass


def try_insert(v_dict, v, countNum):
    if v not in v_dict:
        v_dict[v] = countNum
        countNum += 1

    return countNum


if __name__ == "__main__":
    read_file("./soc-advogato.edges", 5)
