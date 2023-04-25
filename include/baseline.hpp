#ifndef __BASELINE_HPP__
#define __BASELINE_HPP__
#include "spdlog_common.h"
#include "utils.hpp"
#include <iostream>
#include <iomanip>
#include <eigen3/Eigen/Dense>

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
    Eigen::MatrixXd pheromone;
    // 初始注入量与增速
    double ff_start, bf_end;
    // 增速
    double inc_rate;
    // 当前时刻或迭代次数
    int iter;
    // 起始位置和终止位置
    int start = 0, end = N - 1;

    Baseline(double conv_thold, double decay, int max_iter, int print_period) : conv_thold(conv_thold), decay(decay), max_iter(max_iter), print_period(print_period), pheromone(N, N) {
        adj.resize(N, std::vector<int>(N, 0));
        pheromone = Eigen::MatrixXd::Zero(N, N);

        // 初始化初始注入量和流速
        srand(time(NULL));
        ff_start = ((double) rand() / (RAND_MAX)) * 0.5 + 0.5;
        bf_end = ((double) rand() / (RAND_MAX)) * 0.5 + 0.5;
        // ff_start = 1.0;
        // bf_end = 1.0;
        inc_rate = 1.1;
        // 打印ff_start和bf_end
        SPDLOG_INFO("ff_start: {:03.5f}, bf_end: {:03.5f}", ff_start, bf_end);
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
        int rc;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                int val;
                rc = fscanf(fp, "%d ", &val);
                adj[i][j] = val;
                pheromone(i, j) = (double)val;
            }
            rc = fscanf(fp, "\n");
        }
    }

    void run() {
        // 使用dijkstra计算最短路径
        const auto &path = dijkstra(start, end, adj);
        std::fill(adj.begin(), adj.end(), std::vector<int>(N, 0));
        for (int i = 0; i < path.size() - 1; i++) {
            adj[path[i]][path[i + 1]] = 1;
        }

        // 初始化节点流
        Eigen::MatrixXd ff = Eigen::MatrixXd::Zero(1, N);
        Eigen::MatrixXd bf = Eigen::MatrixXd::Zero(1, N);
        // 节点泄露率
        Eigen::MatrixXd passge = Eigen::MatrixXd::Ones(1, N);
        ff(0, start) = ff_start;
        bf(0, end) = bf_end;

        for (iter = 0; iter < max_iter; iter++) {
            // // 归一化pheromone
            auto f_norm_pher = row_normalize(pheromone);
            Eigen::MatrixXd trans_pher = pheromone.transpose();
            auto b_norm_pher = row_normalize(trans_pher);
            auto f_min = fmin(f_norm_pher);
            auto b_min = bmin(b_norm_pher);

            if (iter % print_period == 0) {
                SPDLOG_INFO("Iteration: {}, {:03.5f}, {:03.5f}", iter, f_min, b_min);
            }

            if (f_min > conv_thold && b_min > conv_thold) {
                SPDLOG_INFO("Iteration: {}, {:03.5f}, {:03.5f}", iter, f_min, b_min);
                break;
            }

            Eigen::MatrixXd ff_new = ff * f_norm_pher;
            Eigen::MatrixXd bf_new = bf * b_norm_pher;
            ff_new(0, start) = ff_start * inc_rate;
            bf_new(0, end) = bf_end * inc_rate;
            ff_new(0, end) = 0.0;
            bf_new(0, start) = 0.0;

            Eigen::MatrixXd ff_trans = ff.transpose();
            Eigen::MatrixXd bf_trans = bf.transpose();

            pheromone = pheromone + element_wise_multiply(f_norm_pher, ff_trans) + element_wise_multiply(b_norm_pher, bf_trans).transpose();
            pheromone *= decay;

            // for (int i = 0; i < N; i++) {
            //     for (int j = 0; j < N; j++) {
            //         std::cout << std::fixed << std::setprecision(2) << pheromone(i, j) << " ";
            //     }
            //     std::cout << std::endl;
            // }
            // std::cout << std::endl;
            ff = ff_new / inc_rate;
            bf = bf_new / inc_rate;
            pheromone = pheromone / inc_rate;
        }
    }

    // 归一化
    Eigen::MatrixXd row_normalize(Eigen::MatrixXd &mat) {
        Eigen::MatrixXd res = mat;
        for (int i = 0; i < N; i++) {
            double sum = res.row(i).sum();
            sum = sum == 0 ? 1e-300 : sum;
            res.row(i) /= sum;
        }
        return res;
    }

    Eigen::MatrixXd element_wise_multiply(Eigen::MatrixXd &mat1, Eigen::MatrixXd &mat2) {
        Eigen::MatrixXd res(N, N);
        for (int i = 0; i < N; i++) {
            double product = mat2(i, 0);
            for (int j = 0; j < N; j++) {
                res(i, j) = mat1(i, j) * product;
            }
        }
        return res;
    }

    // 查找符合条件的前向矩阵中的最小值
    double fmin(Eigen::MatrixXd &mat) {
        double res = 1e300;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (adj[i][j] == 1) {
                    res = std::min(res, mat(i, j));
                }
            }
        }
        return res;
    }

    // 查找符合条件的反向矩阵中的最小值
    double bmin(Eigen::MatrixXd &mat) {
        double res = 1e300;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (adj[j][i] == 1) {
                    res = std::min(res, mat(i, j));
                }
            }
        }
        return res;
    }

};

#endif // __BASELINE_HPP__