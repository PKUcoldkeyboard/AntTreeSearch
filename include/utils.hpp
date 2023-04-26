#ifndef __UTILS_HPP__
#define __UTILS_HPP__
#include <vector>
#include <queue>
#include <algorithm>
#include <eigen3/Eigen/Dense>

constexpr int N = 100;

// 链式前向星
class Graph {
public:
    int n;
    struct Edge {
        int to;
        int next;
        double weight;
    };
    Graph(int n) : n(n) {
        head.resize(n, -1);
    }
    std::vector<Edge> edges;
    std::vector<int> head;

    void add_edge(int u, int v, double weight) {
        edges.emplace_back(Edge{v, head[u], weight});
        head[u] = edges.size() - 1;
    }
};

// 使用dijkstra算法计算最短路径，返回路径
std::vector<int> dijkstra(int start, int end, Eigen::MatrixXd &graph) {
    // 使用链式前向星重新存图
    Graph g(N);
    for (int u = 0; u < N; u++) {
        for (int v = 0; v < N; v++) {
            if (graph(u, v) != 0.0) {
                g.add_edge(u, v, graph(u, v));
            }
        }
    }

    // 初始化
    std::vector<int> path;
    std::vector<int> prev(N, -1);
    std::vector<double> dist(N, 1e300);
    std::vector<bool> visited(N, false);

    // 自定义dist的比较函数的优先队列，每次pop最小的
    auto cmp = [&dist](int u, int v) {
        return dist[u] > dist[v];
    };
    std::priority_queue<int, std::vector<int>, decltype(cmp)> pq(cmp);

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
            double weight = g.edges[i].weight;
            if (visited[v]) {
                continue;
            }
            if (dist[v] > dist[u] + weight) {
                dist[v] = dist[u] + weight;
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

// 矩阵mat1的每行分别乘以mat2的对应行
Eigen::MatrixXd element_wise_multiply(Eigen::MatrixXd mat1, Eigen::MatrixXd mat2) {
    int rows = mat1.rows();
    int cols = mat1.cols();
    Eigen::MatrixXd res(rows, cols);
    for (int i = 0; i < rows; i++) {
        double product = mat2(i, 0);
        for (int j = 0; j < cols; j++) {
            res(i, j) = mat1(i, j) * product;
        }
    }
    return res;
}

std::vector<int> min_leakage_dijkstra(int start, int end, Eigen::MatrixXd &graph, Eigen::MatrixXd &passage) {
    Eigen::MatrixXd weights = -passage.array().log();
    Eigen::MatrixXd trans_weights = weights.transpose();
    Eigen::MatrixXd weighted_graph = element_wise_multiply(graph, trans_weights);
    return dijkstra(start, end, weighted_graph);
}

#endif // __UTILS_HPP__