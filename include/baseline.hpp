#ifndef __BASELINE_HPP__
#define __BASELINE_HPP__
#include "spdlog_common.h"
#include "utils.hpp"
#include <iostream>
#include <random>
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
    // 判断是否收敛周期
    int convergence_check_period;
    // 邻接矩阵
    Eigen::MatrixXd adj;
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
    // 初始化种子
    int seed;
    // 是否有泄漏
    bool min_leakage;

    Baseline(double conv_thold, double decay, int max_iter, int print_period, int convergence_check_period, int start_seed, bool min_leakage) : conv_thold(conv_thold), decay(decay), max_iter(max_iter), print_period(print_period), convergence_check_period(convergence_check_period), seed(start_seed), min_leakage(min_leakage), adj(N, N), pheromone(N, N) {
        adj = Eigen::MatrixXd::Zero(N, N);
        pheromone = Eigen::MatrixXd::Zero(N, N);
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
                adj(i, j) = val;
                pheromone(i, j) = (double)val;
            }
            rc = fscanf(fp, "\n");
        }
        // 初始化初始注入量和流速
        srand(seed);
        ff_start = ((double) rand() / (RAND_MAX)) * 0.5 + 0.5;
        bf_end = ((double) rand() / (RAND_MAX)) * 0.5 + 0.5;
    }

    void run() {

        // 初始化节点流
        Eigen::MatrixXd ff = Eigen::MatrixXd::Zero(1, N);
        Eigen::MatrixXd bf = Eigen::MatrixXd::Zero(1, N);

        // 节点泄露因子
        Eigen::MatrixXd passage;
        if (min_leakage) {
            // [0, 1)上均匀分布的随机数
            // std::random_device rd;
            // std::mt19937 gen(rd());
            // std::uniform_real_distribution<> dis(0.0, 1.0);
            // passage = Eigen::MatrixXd::NullaryExpr(1, N, [&]() { return dis(gen); });
            // passage = 0.1 * passage.array() + 0.9;
            passage = Eigen::MatrixXd::Ones(1, N);
            inc_rate = 1.1;
        } else {
            passage = Eigen::MatrixXd::Ones(1, N);
            inc_rate = 1.1;
        }

        ff(0, start) = ff_start;
        bf(0, end) = bf_end;

        // 使用dijkstra计算最短路径
        std::vector<int> path;
        if (min_leakage) {
            // path = min_leakage_dijkstra(start, end, adj, passage);
            path = dijkstra(start, end, adj);
        } else {
            path = dijkstra(start, end, adj);
        }

        for (iter = 0; iter < max_iter; iter++) {
            // // 归一化pheromone
            auto f_norm_pher = row_normalize(pheromone);
            Eigen::MatrixXd trans_pher = pheromone.transpose();
            auto b_norm_pher = row_normalize(trans_pher);
            auto f_min = fmin(f_norm_pher, path);
            auto b_min = bmin(b_norm_pher, path);

            if (iter % convergence_check_period == 0 && f_min > conv_thold && b_min > conv_thold) {
                SPDLOG_INFO("Iteration: {}, {:03.5f}, {:03.5f}", iter, f_min, b_min);
                break;
            }

            if (iter % print_period == 0) {
                SPDLOG_INFO("Iteration: {}, {:03.5f}, {:03.5f}", iter, f_min, b_min);
            }

            Eigen::MatrixXd ff_new = element_wise_multiply(ff * f_norm_pher, passage);
            Eigen::MatrixXd bf_new = element_wise_multiply(bf * b_norm_pher, passage);
            ff_new(0, start) = ff_start * inc_rate;
            bf_new(0, end) = bf_end * inc_rate;
            ff_new(0, end) = 0.0;
            bf_new(0, start) = 0.0;

            Eigen::MatrixXd ff_trans = ff.transpose();
            Eigen::MatrixXd bf_trans = bf.transpose();

            pheromone = pheromone + element_wise_multiply(f_norm_pher, ff_trans) + element_wise_multiply(b_norm_pher, bf_trans).transpose();
            pheromone *= decay;
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

    // 查找符合条件的前向矩阵中的最小值
    double fmin(Eigen::MatrixXd &mat, std::vector<int> &path) {
        double res = 1e300;
        for (int i = 0; i < path.size() - 1; i++) {
            res = std::min(res, mat(path[i], path[i + 1]));
        }
        return res;
    }

    // 查找符合条件的反向矩阵中的最小值
    double bmin(Eigen::MatrixXd &mat, std::vector<int> &path) {
        double res = 1e300;
        for (int i = 0; i < path.size() - 1; i++) {
            res = std::min(res, mat(path[i + 1], path[i]));
        }
        return res;
    }

};

#endif // __BASELINE_HPP__