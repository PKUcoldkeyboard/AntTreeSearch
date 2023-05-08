/**
 * 封装了矩阵和向量操作的类
*/
#ifndef __NUMPY_HPP__
#define __NUMPY_HPP__
#include <vector>
#include <cfloat>
#include <iostream>
#include "spdlog_common.h"
#include "random.hpp"

typedef std::vector<float> Vector;

class NullAble {
private:
    bool null = false;
public:
    bool isNull() {
        return null;
    }
    void setNull() {
        null = true;
    }
};

class Matrix: public NullAble {
private:
    std::vector<std::vector<float>> data;
    unsigned long dim1;
    unsigned long dim2;
public:
    Matrix() {
        setNull();
    }

    Matrix(unsigned long dim1, unsigned long dim2) {
        // 值全为0的矩阵
        this->dim1 = dim1;
        this->dim2 = dim2;
        std::vector<std::vector<float>> result(dim1, Vector(dim2));
        data = result;
    }

    Matrix(unsigned long dim1, unsigned long dim2, float value) {
        // 值全为value的矩阵
        this->dim1 = dim1;
        this->dim2 = dim2;
        std::vector<std::vector<float>> result(dim1, Vector(dim2));
        for (int i = 0; i < dim1; i++) {
            for (int j = 0; j < dim2; j++) {
                result[i][j] = value;
            }
        }
        data = result;
    }

    void set(int i, int j, float v) {
        data[i][j] = v;
    }

    float get(int i, int j) {
        return data[i][j];
    }

    Vector get(int i) {
        // 获取行向量
        return data[i];
    }

    std::pair<unsigned long, unsigned long> shape() {
        return std::make_pair(dim1, dim2);
    }

    void print() {
        SPDLOG_INFO("打印矩阵");
        SPDLOG_INFO("---------------");
        for (int i = 0; i < dim1; i++) {
            for (int j = 0; j < dim2; j++) {
                std::cout << data[i][j] << " ";
            }
            std::cout << std::endl;
        }
        SPDLOG_INFO("---------------");
    }

    Matrix transpose() {
        // 矩阵转置
        Matrix res(dim1, dim2);
        for (int i = 0; i < dim1; i++) {
            for (int j = 0; j < dim2; j++) {
                res.set(j, i, data[i][j]);
            }
        }
        return res;
    }

    Matrix copy() {
        // 矩阵拷贝
        Matrix res(dim1, dim2);
        for (int i = 0; i < dim1; i++) {
            for (int j = 0; j < dim2; j++) {
                res.set(i, j, data[i][j]);
            }
        }
    }

    float reduceSum() {
        float sum = 0;
        for (int i = 0; i < dim1; i++) {
            for (int j = 0; j < dim2; j++) {
                sum += get(i, j);
            }
        }
        return sum;
    }
};

Matrix randMatrix(unsigned long dim1, unsigned long dim2) {
    // 值为0到1之间随机浮点数的矩阵
    Matrix matrix(dim1, dim2);
    Random random;
    for (int i = 0; i < dim1; i++) {
        for (int j = 0; j < dim2; j++) {
            matrix.set(i, j, random.randn());
        }
    }
    return matrix;
}

Vector randVector(unsigned long dim) {
    // 值为0到1之间随机浮点数的向量
    Vector res(dim);
    Random random;
    for (int i = 0; i < dim; i++) {
        res[i] = random.randn();
    }
    return res;
}

float arrayMax(Vector arr) {
    float res = FLT_MIN;
    for (int i = 0; i < arr.size(); i++) {
        res = std::max(res, arr[i]);
    }
    return res;
}

int arrayArgmax(Vector arr) {
    float max = FLT_MIN;
    int index = 0;
    for (int i = 0; i < arr.size(); i++) {
        if (arr[i] > max) {
            max = arr[i];
            index = i;
        }
    }
    return index;
}

Vector VectorDotMatrix(Vector v, Matrix m) {
    assert(v.size() == m.shape().first);
    Vector res(m.shape().second, 0);
    for (int j = 0; j < m.shape().second; j++) {
        for (int i = 0; i < m.shape().first; i++) {
            res[j] += v[i] * m.get(i, j);
        }
    }
    return res;
}

Vector vectorMul(Vector vec1, Vector vec2) {
    assert(vec1.size() == vec2.size());
    Vector res(vec1.size(), 0);
    for (int i = 0; i < res.size(); i++) {
        res[i] = vec1[i] * vec2[i];
    }
    return res;
}

Matrix matrixAdd(Matrix mat1, Matrix mat2) {
    assert(mat1.shape() == mat2.shape());
    Matrix res(mat1.shape().first, mat1.shape().second);
    for (int i = 0; i < mat1.shape().first; i++) {
        for (int j = 0; j < mat1.shape().second; j++) {
            res.set(i, j, mat1.get(i, j) + mat2.get(i, j));
        }
    }
    return res;
}

Matrix matrixMulVector(Matrix mat, Vector vec, int axis) {
    assert(axis == 0 || axis == 1);
    if (axis == 0) {
        for (int j = 0; j < mat.shape().second; j++) {
            for (int i = 0; i < mat.shape().first; i++) {
                mat.set(i, j, mat.get(i, j) * vec[i]);
            }
        }
        return mat;
    }
    
    if (axis == 1) {
        for (int i = 0; i < mat.shape().first; i++) {
            for (int j = 0; j < mat.shape().second; j++) {
                mat.set(i, j, mat.get(i, j) * vec[j]);
            }
        }
    }

    return mat;
}

Matrix matrixMulScalar(Matrix mat, float sca) {
    for (int i = 0; i < mat.shape().first; i++) {
        for (int j = 0; j < mat.shape().second; j++) {
            mat.set(i, j, mat.get(i, j) * sca);
        }
    }
    return mat;
}

Vector vectorMulScalar(Vector vec, float sca) {
    for (int i = 0; i < vec.size(); i++) {
        vec[i] *= sca;
    }
    return vec;
}

Vector vectorAddScalar(Vector vec, float sca) {
    for (int i = 0; i < vec.size(); i++) {
        vec[i] += sca;
    }
    return vec;
}

#endif // __NUMPY_HPP__