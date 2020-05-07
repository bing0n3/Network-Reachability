import random
import math
import numpy as np


def read_file(file_name, L, limit=None):
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
            if limit and v_dict[first] >= limit:
                continue

            if v_dict[first] not in adjList.keys():
                adjList[v_dict[first]] = []

            if limit and v_dict[second] >= limit:
                continue
            adjList[v_dict[first]].append(v_dict[second])

            # add to edge list
            edgeList.append([v_dict[first], v_dict[second]])

        # print(v_dict, countNum)
        gen_adj_list(countNum, adjList, file_name, limit)
        labelList = gen_label_list(countNum, L, file_name, limit)
        gen_query(file_name, countNum, L, limit)


def gen_adj_list(countNum, adjList, filename, limit=None):
    lists = []

    for i in range(min(countNum, limit) if limit else countNum):
        if i in adjList.keys():
            lists.append(adjList[i])
        else:
            lists.append([])

    with open(
        filename + ("-" + str(limit) + "-" if limit else "-") + "adj.data", "w"
    ) as f:
        f.write("%d\n" % (min(countNum, limit) if limit else countNum))
        for l in lists:
            out_str = " ".join(str(x) for x in l)
            f.write("%s\n" % out_str)
    return lists


def gen_label_list(vertex_num, L, filename, limit):
    random.seed()
    mean = int(math.floor(L / 2))
    sd = int(max(1, math.floor(L / 4)))

    v_l = []

    for i in range(min(vertex_num, limit) if limit else vertex_num):
        label = random.normalvariate(mean, sd)
        label = max(0, label)
        label = min(L - 1, label)
        label = int(math.floor(label))
        v_l.append(label)

    with open(
        filename + ("-" + str(limit) + "-" if limit else "-") + "label.data", "w"
    ) as f:
        f.write("%d\n" % (min(vertex_num, limit) if limit else vertex_num))
        for c_l in v_l:
            f.write("%d\n" % c_l)

    return v_l


def gen_query(filename, countNum, L, limit=None):

    q1 = np.random.randint(min(countNum, limit) if limit else countNum, size=100000)
    q2 = np.random.randint(min(countNum, limit) if limit else countNum, size=100000)

    exist = random.uniform(0, 1)

    with open(
        filename + ("-" + str(limit) + "-" if limit else "-") + "query-" + str(L), "w"
    ) as f:
        for i in range(len(q1)):
            line = "%d %d" % (q1[i], q2[i])
            ls = []
            for j in range(L):
                exist = random.uniform(0, 1)
                if exist > 0.5:
                    ls.append(str(j))

            f.write("%s %s\n" % (line, " ".join(ls)))


def try_insert(v_dict, v, countNum):
    if v not in v_dict:
        v_dict[v] = countNum
        countNum += 1

    return countNum


if __name__ == "__main__":
    read_file("dataset/soc-epinions.edges", 5)
