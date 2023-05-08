#include "helper.hpp"
#include "input.hpp"
#include "inifile.hpp"
#include <iostream>

Random r;

std::pair<int, int> run(Matrix adj, bool minLeakage, unsigned long n, float decay, float convThold, std::string rule) {
    Vector passage;
    float incRate;
    Matrix optPath;

    if (minLeakage) {
        passage = vectorAddScalar(vectorMulScalar(randVector(n), 0.1), 0.9);
        incRate = 1;
        optPath = getMinLeakagePathAdj(adj, passage);
    } else {
        passage = std::vector<float>(n, 1.0);
        incRate = 1.1;
        optPath = getShortestPathAdj(adj);
    }
    
    Decision* decision;
    if (rule == "linear") {
        decision = new Linear();
    } else if (rule == "slight") {
        decision = new Slight();
    } else if (rule == "quadratic") {
        decision = new Quadratic();
    } else if (rule == "quaratic-with-offset") {
        decision = new QuadraticWithOffset();
    } else {
        // rank-edge
    }

    Matrix pher = adj.copy();

    float ffStart = (float) (0.5 + r.randn() * 0.5);
    float bfEnd = (float) (0.5 + r.randn() * 0.5);

    int wo = simulate(
            adj.copy(),
            std::vector<float>(n, 1),
            1,
            decay,
            optPath,
            pher.copy(),
            ffStart,
            bfEnd,
            convThold,
            decision
    );

    int w = simulate(
            adj.copy(),
            passage,
            incRate,
            decay,
            optPath,
            pher.copy(),
            ffStart,
            bfEnd,
            convThold,
            decision
    );

    // 释放内存
    delete decision;

    return std::make_pair(w, wo);
}

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        SPDLOG_ERROR("Usage: {} <decision_rule>", argv[0]);
        exit(-1);
    }

    std::string decisionRule = argv[1];
    std::string dataPath = "dataset/data/";

    std::vector<std::string> graphTypes;
    graphTypes.push_back("gnp_w_loop_");
    graphTypes.push_back("gnp_wo_loop_");
    graphTypes.push_back("gnpLoc_w_loop_");
    graphTypes.push_back("gnpLoc_wo_loop_");
    graphTypes.push_back("grid_wo_loop_");

    std::ofstream file("result.txt", std::ios::app);
    
    for (int i = 0; i < graphTypes.size(); i++) {
        for (int j = 0; j < 2; j++) {
            std::string filename = graphTypes[i];
            // 有无leakage
            if (j == 0) {
                filename += "wo_leakage";
            } else {
                filename += "w_leakage";
            }

            std::string graphName = filename;

            auto minLeakage = true;
            float decay = 0.9;
            float convThold = 0.99;
            int n = 100;

            float countW = 0;
            float countWO = 0;
            float IterCountW = 0;
            float IterCountWO = 0;
            
            // 100个case
            int k;
            for (k = 0; k < 100; k++) {
                SPDLOG_INFO("Case {}:", k);
                std::stringstream ss;
                ss << k;
                filename = graphTypes[i];
                if (j == 0) {
                    filename += "wo_leakage_";
                } else {
                    filename += "w_leakage_";
                }
                filename = filename + ss.str() + ".txt";

                // 读取文件
                Matrix adj = readFromFile(dataPath + filename);

                // 处理结果
                std::pair<int, int> result = run(adj, minLeakage, n, decay, convThold, decisionRule);

                if (result.first > 0) {
                    IterCountW += result.first;
                    countW += 1;
                }

                if (result.second > 0) {
                    IterCountWO += result.second;
                    countWO += 1;
                }
            }
            SPDLOG_INFO("WO: Converged Rate: {:03.5f}, Avg Converged iterations: {}", countWO / k, IterCountWO / countWO);
            SPDLOG_INFO("W: Converged Rate: {:03.5f}, Avg Converged iterations: {}", countW / k, IterCountW / countW);

            file << graphName << " - WO: Converged Rate: " << countWO / k << ", Avg Converged iterations: " << IterCountWO / countWO << std::endl;
            file << graphName << " - W: Converged Rate: " << countW / k << ", Avg Converged iterations: " << IterCountW / countW << std::endl;     
        }
    }

    file.close();
    return 0;
}