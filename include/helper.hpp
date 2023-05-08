#ifndef __HELPER_HPP__
#define __HELPER_HPP__

#include "numpy.hpp"
#include "utils.hpp"

int simulate(Matrix adj, Vector passage, float incRate, float decay, Matrix optPath,
             Matrix pher, float ffStart = 1, float bfEnd = 1, float thold = 0.999, Decision *decision = new Linear(),
             int printPeriod = 1000, int convCheckPeriod = 1000, int maxIter = 10000) {
    
    unsigned long n = adj.shape().first;
    Vector ff(n, 0);
    Vector bf(n, 0);
    ff[0] = ffStart;
    bf[n - 1] = bfEnd;
    for (int i = 0; i < maxIter; i++) {
        Matrix fNormPher = rowNormalize(decision->decide(pher, adj));
        Matrix bNormPher = rowNormalize(decision->decide(pher.transpose(), adj.transpose()));

        if (i % printPeriod == 0 && !optPath.isNull()) {
            SPDLOG_INFO("Iteration: {}", i);
        }

        if (i > 0 && i % convCheckPeriod == 0) {
            if (checkConvergence(pher, adj, thold, optPath, decision)) {
                return i;
            }
        }

        Vector ffNew = vectorMul(VectorDotMatrix(ff, fNormPher), passage);
        Vector bfNew = vectorMul(VectorDotMatrix(bf, bNormPher), passage);

        ffNew[0] += ffStart * std::pow(incRate, i + 2);
        bfNew[n - 1] += bfEnd * std::pow(incRate, i + 2);

        fNormPher = matrixMulVector(fNormPher, ff, 0);
        bNormPher = matrixMulVector(bNormPher, bf, 0).transpose();

        pher = matrixAdd(pher, matrixAdd(fNormPher, bNormPher));
        pher = matrixMulScalar(pher, decay);

        ff = ffNew;
        bf = bfNew;
    }
    return -1;
}

#endif // __HELPER_HPP__