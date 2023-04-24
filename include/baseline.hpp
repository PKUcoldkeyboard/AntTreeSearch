#ifndef __BASELINE_HPP__
#define __BASELINE_HPP__
#define N 100
#include "spdlog_common.h"
#include "utils.hpp"
#include <iostream>
#include <iomanip>

class Baseline {
public:
    // 收敛阈值
    double conv_thold;
    // 信息素衰减因子
    double decay;
    // 最大迭代次数
    int max_iter;
    // 打印周期
    int print_period;
    // 邻接矩阵
    std::vector<std::vector<int>> adj;
    // 信息素矩阵
    std::vector<std::vector<double>> pheromone;
    // 节点流
    std::vector<double> ff_node, bf_node;
    // 边流
    std::vector<std::vector<double>> ff_edge, bf_edge;
    // 初始注入量
    double ff_start = 1.0, bf_end = 1.0;
    // 当前时刻或迭代次数
    int iter;
    // 起始位置和终止位置
    int start = 0, end = N - 1;

    Baseline(double conv_thold, double decay, int max_iter, int print_period) : conv_thold(conv_thold), decay(decay), max_iter(max_iter), print_period(print_period) {
        adj.resize(N, std::vector<int>(N, 0));
        pheromone.resize(N, std::vector<double>(N, 0.0));
        ff_edge.resize(N, std::vector<double>(N, 0.0));
        bf_edge.resize(N, std::vector<double>(N, 0.0));
        ff_node.resize(N, 0.0);
        bf_node.resize(N, 0.0);
    }

    ~Baseline() {}

    /**
     * 读取数据集文件
    */
    void init(const std::string &path) {
        FILE* fp = fopen(path.c_str(), "r");
        if (fp == nullptr) {
            SPDLOG_ERROR("Failed to open file: {}", path);
            exit(-1);
        }
        // 初始化adj与pheromone
        int rd;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                int val;
                rd = fscanf(fp, "%d ", &val);
                adj[i][j] = val;
                pheromone[i][j] = (double)val;
            }
            rd = fscanf(fp, "\n");
        }
        fclose(fp);
    }

    void run() {
        // 使用dijkstra计算最短路径
        const auto &path = dijkstra(start, end, adj);
        std::fill(adj.begin(), adj.end(), std::vector<int>(N, 0));
        for (int i = 0; i < path.size() - 1; i++) {
            adj[path[i]][path[i + 1]] = 1;
        }
        ff_node[start] = ff_start;
        bf_node[end] = bf_end;
        for (iter = 0; iter < max_iter; iter++) {
            auto f_norm_pher = normalize_front_pheromone();
            auto b_norm_pher = normalize_backward_pheromone();
            auto f_min = min(f_norm_pher);
            auto b_min = min(b_norm_pher);

            if (iter % print_period == 0) {
                SPDLOG_INFO("Iteration {}, {:03.5f}, {:03.5f}", iter, f_min, b_min);
            }

            if (f_min > conv_thold && b_min > conv_thold) {
                SPDLOG_INFO("Converged at iteration {}", iter);
                break;
            } 
            
            update_front_edge_flow(f_norm_pher);
            update_backward_edge_flow(b_norm_pher);
            update_front_node_flow();
            update_backward_node_flow();
            update_pheromone();

        }
    }

    // 最短路径上对应的信息素最小值
    double min(std::vector<std::vector<double>> &norm_pheromone) {
        double res = std::numeric_limits<double>::max();
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (adj[i][j] == 1) {
                    res = std::min(res, norm_pheromone[i][j]);
                }
            }
        }
        return res;
    }

    // 前向信息素矩阵归一化
    std::vector<std::vector<double>> normalize_front_pheromone() {
        std::vector<std::vector<double>> res(N, std::vector<double>(N, 0.0));
        for (int i = 0; i < N; i++) {
            float sum = 0.0f;
            for (int j = 0; j < N; j++) {
                sum += pheromone[i][j];
            }
            if (sum == 0.0f) {
                continue;
            }
            for (int j = 0; j < N; j++) {
                res[i][j] = pheromone[i][j] / sum;
            }
        }
        return res;
    }

    // 后向信息素矩阵归一化
    std::vector<std::vector<double>> normalize_backward_pheromone() {
        std::vector<std::vector<double>> res(N, std::vector<double>(N, 0.0));
        for (int j = 0; j < N; j++) {
            float sum = 0.0f;
            for (int i = 0; i < N; i++) {
                sum += pheromone[i][j];
            }
            if (sum == 0.0f) {
                continue;
            }
            for (int i = 0; i < N; i++) {
                res[i][j] = pheromone[i][j] / sum;
            }
        }
        return res;
    }

    // 更新信息素矩阵
    void update_pheromone() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                pheromone[i][j] = decay * (pheromone[i][j] + ff_edge[i][j] + bf_edge[i][j]);
            }
        }
    }

    // 更新前向节点flow
    void update_front_node_flow() {
        for (int j = 0; j < N; j++) {
            float ff_sum = 0.0f;
            for (int i = 0; i < N; i++) {
                ff_sum += ff_edge[i][j];
            }
            ff_node[j] = ff_sum;
        }   
        ff_node[start] = ff_start;
        ff_node[end] = 0.0;
    }

    // 更新反向节点flow
    void update_backward_node_flow() {
        for (int i = 0; i < N; i++) {
            float bf_sum = 0.0f;
            for (int j = 0; j < N; j++) {
                bf_sum += bf_edge[i][j];
            }
            bf_node[i] = bf_sum;
        }
        bf_node[end] = bf_end;
        bf_node[start] = 0.0;
    }

    // 更新前向边flow
    void update_front_edge_flow(std::vector<std::vector<double>> &norm_pheromone) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                ff_edge[i][j] = ff_node[i] * norm_pheromone[i][j];
            }
        }
    }

    // 更新反向边flow
    void update_backward_edge_flow(std::vector<std::vector<double>> &norm_pheromone) {
        for (int j = 0; j < N; j++) {
            for (int i = 0; i < N; i++) {
                bf_edge[i][j] = bf_node[j] * norm_pheromone[i][j];
            }
        }
    }
};

#endif // __BASELINE_HPP__