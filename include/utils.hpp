#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include "numpy.hpp"
#include "dijkstra.hpp"
#include "decision.hpp"
#include <unordered_set>

float pherArgmin(Matrix pher, Matrix optPath) {
    float res = FLT_MAX;
    for (int i = 0; i < pher.shape().first; i++) {
        for (int j = 0; j < pher.shape().second; j++) {
            if (optPath.get(i, j) > 0) {
                res = std::min(res, pher.get(i, j));
            }
        }
    }
    return res;
}

Matrix rowNormalize(Matrix adj) {
    unsigned long n = adj.shape().first;
    Matrix res(n, n);
    for (int i = 0; i < n; i++) {
        float s = 0;
        for (int j = 0; j < n; j++) {
            s += adj.get(i, j);
        }
        for (int j = 0; j < n; j++) {
            res.set(i, j, (float) (adj.get(i, j) / (s + 1e-12)));
        }
    }
    return res;
}

PathInfo minLeakagePath(Matrix adj, Vector passage, int s, int d) {
    for (int i = 0; i < passage.size(); i++) {
        passage[i] = -log(passage[i]);
    }
    for (int i = 0; i < adj.shape().first; i++) {
        for (int j = 0; j < adj.shape().second; j++) {
            adj.set(i, j, adj.get(i, j) * passage[i]);
        }
    }
    return findPath(adj, s, d);
}

Matrix addRandomShortestPath(Matrix adj, int k, int defaultLen) {
    unsigned long n = adj.shape().first;
    PathInfo path = findPath(adj, 0, n - 1);
    int newPathCost = defaultLen - k;
    if (!path.isNull()) {
        newPathCost = int(path.totalCost - k);
    }
    int curNode = 0;
    if (newPathCost <= 0) {
        return adj;
    }
    Random random;
    for (int co = 1; co < newPathCost; co++) {
        float curCostF = 0;
        float curCostB = 0;
        int nextNode = -1;
        while (curCostF <= co || curCostB <= newPathCost - co) {
            nextNode = random.randint(1, n - 1);
            path = findPath(adj, 0, nextNode);
            curCostF = path.isNull()? FLT_MAX : path.totalCost;
            path = findPath(adj, nextNode, n - 1);
            curCostB = path.isNull()? FLT_MAX : path.totalCost;
        }
        adj.set(curNode, nextNode, 1);
        curNode = nextNode;
    }
    adj.set(curNode, n - 1, 1);
    return adj;
}

bool checkConvergence(Matrix pher, Matrix adj, float thold, Matrix optPath, Decision* decision) {
    int n = (int) pher.shape().first;
    Matrix fNormPher = decision->decide(pher, adj);
    fNormPher = rowNormalize(fNormPher);
    Matrix bNormPher = decision->decide(pher.transpose(), adj.transpose());
    bNormPher = rowNormalize(bNormPher);
    int curNode = 0;
    std::unordered_set<int> curSet;
    while (curNode != n - 1 && curSet.find(curNode) == curSet.end()) {
        curSet.insert(curNode);
        if (arrayMax(fNormPher.get(curNode)) == 0) break;
        if (arrayMax(fNormPher.get(curNode)) < thold) return false;
        curNode = arrayArgmax(fNormPher.get(curNode));
    }
    if (curSet.size() > optPath.reduceSum()) {
        return false;
    }
    curNode = n - 1;
    curSet.clear();
    while (curNode != 0 && curSet.find(curNode) == curSet.end()) {
        curSet.insert(curNode);
        if (arrayMax(bNormPher.get(curNode)) == 0) break;
        if (arrayMax(bNormPher.get(curNode)) < thold) return false;
        curNode = arrayArgmax(bNormPher.get(curNode));
    }
    return curSet.size() <= optPath.reduceSum();
}

#endif // __UTILS_HPP__