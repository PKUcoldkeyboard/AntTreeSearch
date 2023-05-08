#ifndef __HELPER_HPP__
#define __HELPER_HPP__

#include "numpy.hpp"
#include "utils.hpp"

int simulate(Matrix adj, Vector passage, float incRate, float decay, Matrix optPath,
             Matrix pher, float ffStart = 1, float bfEnd = 1, float thold = 0.999, Decision *decision = new Linear(),
             int printPeriod = 500, int convCheckPeriod = 10, int maxIter = 5000) {
    
    unsigned long n = adj.shape().first;
    Vector ff(n, 0);
    Vector bf(n, 0);
    ff[0] = ffStart;
    bf[n - 1] = bfEnd;
    for (int i = 0; i < maxIter; i++) {
        Matrix fNormPher = rowNormalize(decision->decide(pher, adj));
        Matrix bNormPher = rowNormalize(decision->decide(pher.transpose(), adj.transpose()));

        if (i % printPeriod == 0 && !optPath.isNull()) {
            float fmin = pherArgmin(fNormPher, optPath);
            float bmin = pherArgmin(bNormPher, optPath);
            SPDLOG_INFO("Iteration: {}, {:03.5f}, {:03.5f}", i, fmin, bmin);
        }

        if (i > 0 && i % convCheckPeriod == 0) {
            if (checkConvergence(pher, adj, thold, optPath, decision)) {
                return i;
            }
        }

        Vector ffNew = vectorMul(VectorDotMatrix(ff, fNormPher), passage);
        Vector bfNew = vectorMul(VectorDotMatrix(bf, bNormPher), passage);

        ffNew[0] = ffStart * incRate;
        ffNew[n - 1] = 0.0;
        bfNew[0] = 0.0;
        bfNew[n - 1] = bfEnd * incRate;

        fNormPher = matrixMulVector(fNormPher, ff, 0);
        bNormPher = matrixMulVector(bNormPher, bf, 0).transpose();

        pher = matrixAdd(pher, matrixAdd(fNormPher, bNormPher));
        pher = matrixMulScalar(pher, decay);

        ff = ffNew;
        bf = bfNew;

        ff = vectorByScalar(ffNew, incRate);
        bf = vectorByScalar(bfNew, incRate);
        pher = matrixByScalar(pher, incRate);
    }
    return -1;
}

Matrix getShortestPathAdj(Matrix adj) {
    PathInfo pathInfo = findPath(adj, 0, adj.shape().first - 1);
    std::vector<int> pathNodes = pathInfo.nodes;
    Matrix adjM(adj.shape().first, adj.shape().second, 0);
    for (int i = 0; i < pathNodes.size() - 1; i++) {
        adjM.set(pathNodes[i], pathNodes[i + 1], 1);
    }
    return adjM;
}

Matrix getMinLeakagePathAdj(Matrix adj, Vector passage) {
    PathInfo pathInfo = minLeakagePath(adj, passage, 0, adj.shape().first - 1);
    std::vector<int> pathNodes = pathInfo.nodes;
    Matrix adjM(adj.shape().first, adj.shape().second, 0);
    for (int i = 0; i < pathNodes.size() - 1; i++) {
        adjM.set(pathNodes[i], pathNodes[i + 1], 1);
    }
    return adjM;
}

#endif // __HELPER_HPP__