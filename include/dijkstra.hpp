//
// Created by 10740 on 2023/5/4.
//
#ifndef __DIJKSTRA_HPP__
#define __DIJKSTRA_HPP__
#include "numpy.hpp"
#include <queue>

class PathInfo: public NullAble {
public:
    PathInfo() {
        setNull();
    }

    PathInfo(std::vector<int> nodes, float totalCost) {
        this->nodes = nodes;
        this->totalCost = totalCost;
    }

    std::vector<int> nodes;
    float totalCost;
};

// 链式前向星
class Graph {
public:
    int n;
    struct Edge {
        int to;
        int next;
        float weight;
    };
    Graph(int n) : n(n) {
        head.resize(n, -1);
    }
    std::vector<Edge> edges;
    std::vector<int> head;

    void add_edge(int u, int v, float weight) {
        edges.emplace_back(Edge{v, head[u], weight});
        head[u] = edges.size() - 1;
    }
};

Graph create(Matrix adj) {
    unsigned long n = adj.shape().first;
    Graph g(n);
    for (int u = 0; u < n; u++) {
        for (int v = 0; v < n; v++) {
            if (adj.get(u, v) != 0) {
                g.add_edge(u, v, adj.get(u, v));
            }
        }
    }
    return g;
}

PathInfo findPath(Matrix adj, int start, int end) {
    // 使用堆优化的dijkstra获取最短路径
    unsigned long n = adj.shape().first;

    std::vector<float> totalCosts(n, FLT_MAX);
    std::vector<int> prev(n, -1);
    std::vector<int> nodes;
    std::vector<bool> sure(n, false);

    Graph g = create(adj);
    totalCosts[start] = 0;
    
    // 自定义dist的比较函数的优先队列，每次pop最小的
    auto cmp = [&totalCosts](int u, int v) {
        return totalCosts[u] > totalCosts[v];
    };
    std::priority_queue<int, std::vector<int>, decltype(cmp)> pq(cmp);

    pq.push(start);
    while (!pq.empty()) {
        int u = pq.top();
        pq.pop();
        if (sure[u]) {
            continue;
        }
        sure[u] = true;
        for (int i = g.head[u]; i != -1; i = g.edges[i].next) {
            int v = g.edges[i].to;
            float weight = g.edges[i].weight;
            if (sure[v]) {
                continue;
            }
            if (totalCosts[v] > totalCosts[u] + weight) {
                totalCosts[v] = totalCosts[u] + weight;
                // 记录路径
                prev[v] = u;
                pq.push(v);
            }
        }
        if (u == end) {
            break;
        }
    }
    if (totalCosts[end] == FLT_MAX) {
        return PathInfo();
    }
    int curr = end;
    while (curr != -1) {
        nodes.push_back(curr);
        curr = prev[curr];
    
    }
    std::reverse(nodes.begin(), nodes.end());
    PathInfo pathInfo = PathInfo(nodes, totalCosts[end]);
    return pathInfo;
}

#endif // __DIJKSTRA_HPP__