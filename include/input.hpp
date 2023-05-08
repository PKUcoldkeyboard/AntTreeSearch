/**
 * Input handling
 * 用于读取数据文件
*/
#ifndef __INPUT_HPP__
#define __INPUT_HPP__
#include "numpy.hpp"
#include <fstream>
#include <sstream>
#include "spdlog_common.h"

constexpr int N = 100;

Matrix readFromFile(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        SPDLOG_ERROR("Cannot open file {}", filename);
        exit(1);
    }
    Matrix adj((unsigned long) N, (unsigned long) N, 0);
    int i = 0, j = 0, value;
    while (file >> value) {
        if (value == 1) {
            adj.set(i, j, 1);
        }
        j++;
        if (j == N) {
            i++;
            j = 0;
        }
    }
    file.close();
    return adj;
}

#endif // __INPUT_HPP__