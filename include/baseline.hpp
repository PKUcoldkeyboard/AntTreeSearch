#ifndef __BASELINE_HPP__
#define __BASELINE_HPP__
#define N 4
#include "spdlog_common.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

class Baseline {
public:
    // 收敛阈值
    float conv_thold;
    // 信息素衰减因子
    float decay;
    // 最大迭代次数
    int max_iter;
    // 信息素矩阵
    float pheromone[N][N];
    // 归一化的信息素矩阵
    float norm_pheromone[N][N];
    // 节点流
    float ff_node[N], bf_node[N];
    // 边流
    float ff_edge[N][N], bf_edge[N][N];
    // 初始注入量
    float ff_start = 1.0f, bf_start = 1.0f;
    // 当前时刻或迭代次数t
    int t;
    // 起始位置和终止位置
    int start = 0, end = N - 1;

    Baseline(float conv_thold, float decay, int max_iter) : conv_thold(conv_thold), decay(decay), max_iter(max_iter) {}

    ~Baseline() {}

    void run() {
        for (t = 0; t < max_iter; t++) {
            iterate();
            // if (converged()) {
            //     break;
            // }
        }
        // 打印当前的pheromone矩阵到tmp.txt文件中
        std::ofstream outfile("tmp.txt");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                // 设置3位小数
                outfile << std::fixed << std::setprecision(2) << pheromone[i][j] << " ";
            }
            outfile << std::endl;
        }
        // 打印当前的ff_node和bf_node到tmp.txt文件中
        outfile << std::endl;
        for (int i = 0; i < N; i++) {
            outfile << std::fixed << std::setprecision(2) << ff_node[i] << " ";
        }
        outfile << std::endl;
        outfile << std::endl;
        for (int i = 0; i < N; i++) {
            outfile << std::fixed << std::setprecision(2) << bf_node[i] << " ";
        }
        outfile << std::endl;
        outfile << std::endl;
        // 打印当前的ff_edge和bf_edge到tmp.txt文件中
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                outfile << std::fixed << std::setprecision(2) << ff_edge[i][j] << " ";
            }
            outfile << std::endl;
        }
        outfile << std::endl;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                outfile << std::fixed << std::setprecision(2) << bf_edge[i][j] << " ";
            }
            outfile << std::endl;
        }
        outfile.close();
    }

    // 读取地图文件，初始化信息素矩阵和蚂蚁流矩阵
    void init(std::string filename) {
        std::ifstream infile(filename);
        if (!infile) {
            SPDLOG_ERROR("Unable to open file {}", filename);
            return;
        }

        std::stringstream buffer;
        buffer << infile.rdbuf();
        std::string file_contents = buffer.str();
        std::istringstream iss(file_contents);

        // 初始化信息素矩阵
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                int val;
                iss >> val;
                pheromone[i][j] = (float)val; 
            }
        }

        // 初始化蚂蚁节点流
        memset(ff_node, 0.0f, sizeof(ff_node));
        memset(bf_node, 0.0f, sizeof(bf_node));
        ff_node[start] = ff_start;
        bf_node[end] = bf_start;

        // 初始化蚂蚁边流矩阵
        memset(ff_edge, 0.0f, sizeof(ff_edge));
        memset(bf_edge, 0.0f, sizeof(bf_edge));
    }

    // 前向信息素矩阵归一化
    void normalize_front_pheromone() {
        memset(norm_pheromone, 0.0f, sizeof(norm_pheromone));
        for (int i = 0; i < N; i++) {
            float sum = 0.0f;
            for (int j = 0; j < N; j++) {
                sum += pheromone[i][j];
            }
            if (sum == 0.0f) {
                continue;
            }
            for (int j = 0; j < N; j++) {
                norm_pheromone[i][j] = pheromone[i][j] / sum;
            }
        }
    }

    // 后向信息素矩阵归一化
    void normalize_backward_pheromone() {
        memset(norm_pheromone, 0.0f, sizeof(norm_pheromone));
        for (int j = 0; j < N; j++) {
            float sum = 0.0f;
            for (int i = 0; i < N; i++) {
                sum += pheromone[i][j];
            }
            if (sum == 0.0f) {
                continue;
            }
            for (int i = 0; i < N; i++) {
                norm_pheromone[i][j] = pheromone[i][j] / sum;
            }
        }
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
    }

    // 更新前向边flow
    void update_front_edge_flow() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                ff_edge[i][j] = ff_node[i] * norm_pheromone[i][j];
            }
        }
    }

    // 更新反向边flow
    void update_backward_edge_flow() {
        for (int j = 0; j < N; j++) {
            for (int i = 0; i < N; i++) {
                bf_edge[i][j] = bf_node[j] * norm_pheromone[i][j];
            }
        }
    }

    // 迭代一次
    void iterate() {
        normalize_front_pheromone();
        update_front_edge_flow();
        normalize_backward_pheromone();
        update_backward_edge_flow();
        update_front_node_flow();
        update_backward_node_flow();
        update_pheromone();
    }

    bool converged() {
        return true;
    }

};

#endif // __BASELINE_HPP__