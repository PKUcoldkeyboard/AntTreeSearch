#ifndef __UTILS_HPP__
#define __UTILS_HPP__
#define N 100
#define INF 0x3f3f3f3f
#include <vector>
#include <queue>
#include <algorithm>

// 链式前向星
class Graph {
public:
    int n;
    struct Edge {
        int to;
        int next;
    };
    Graph(int n) : n(n) {
        head.resize(n, -1);
    }
    std::vector<Edge> edges;
    std::vector<int> head;

    void add_edge(int u, int v) {
        edges.emplace_back(Edge{v, head[u]});
        head[u] = edges.size() - 1;
    }
};

// 使用dijkstra算法计算最短路径，返回路径
std::vector<int> dijkstra(int start, int end, std::vector<std::vector<int>> &graph) {
    // 使用链式前向星重新存图
    Graph g(N);
    for (int u = 0; u < N; u++) {
        for (int v = 0; v < N; v++) {
            if (graph[u][v] != 0) {
                g.add_edge(u, v);
            }
        }
    }

    // 初始化
    std::vector<int> path;
    std::vector<int> prev(N, -1);
    std::vector<int> dist(N, INF);
    std::vector<bool> visited(N, false);
    std::priority_queue<int> pq;

    // 使用优先队列的dijkstra
    dist[start] = 0;
    pq.push(start);
    while (!pq.empty()) {
        int u = pq.top();
        pq.pop();
        if (visited[u]) {
            continue;
        }
        visited[u] = true;
        for (int i = g.head[u]; i != -1; i = g.edges[i].next) {
            int v = g.edges[i].to;
            if (dist[v] > dist[u] + 1) {
                dist[v] = dist[u] + 1;
                prev[v] = u;
                pq.push(v);
            }
        }
    }

    // 计算路径
    int curr = end;
    while (curr != -1) {
        path.push_back(curr);
        curr = prev[curr];
    }
    std::reverse(path.begin(), path.end());
    return path;
}

#endif // __UTILS_HPP__