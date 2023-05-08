#ifndef __DECISION_HPP__
#define __DECISION_HPP__
#include "numpy.hpp"

// Decison类，只能用于继承
class Decision {
public:
    virtual Matrix decide(Matrix pher, Matrix adj) = 0;
};

class Linear : public Decision {
public:
    Matrix decide(Matrix pher, Matrix adj) override {
        Matrix res(pher.shape().first, pher.shape().second);
        res = pher;
        return res;
    }
};

class Quadratic : public Decision {
public:
    Matrix decide(Matrix pher, Matrix adj) override {
        Matrix res(pher.shape().first, pher.shape().second);
        for (int i = 0; i < adj.shape().first; i++) {
            for (int j = 0; j < adj.shape().second; j++) {
                res.set(i, j, adj.get(i, j) * std::pow(pher.get(i, j), 2));
            }
        }
        return res;
    }
};

class QuaraticWithOffset : public Decision {
public:
    Matrix decide(Matrix pher, Matrix adj) override {
        Matrix res(pher.shape().first, pher.shape().second);
        for (int i = 0; i < adj.shape().first; i++) {
            for (int j = 0; j < adj.shape().second; j++) {
                res.set(i, j, adj.get(i, j) * std::pow(pher.get(i, j) + 1e-12, 2));
            }
        }
        return res;
    }
};

class Slight : public Decision {
public:
    Matrix decide(Matrix pher, Matrix adj) override {
        Matrix res(pher.shape().first, pher.shape().second);
        for (int i = 0; i < adj.shape().first; i++) {
            for (int j = 0; j < adj.shape().second; j++) {
                res.set(i, j, adj.get(i, j) * std::pow(pher.get(i, j), 1.1));
            }
        }
        return res;
    }
};

#endif // __DECISION_HPP__