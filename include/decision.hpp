#ifndef __DECISION_HPP__
#define __DECISION_HPP__
#include <cmath>
#define N 100
#define K 1

void quadratic(float **pheromone) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            pheromone[i][j] = pheromone[i][j] * pheromone[i][j];
        }
    }
}

void slight(float **pheromone) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            pheromone[i][j] = pow(pheromone[i][j], 1.1f);
        }
    }
}

void quadratic_with_offset(float **pheromone) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            pheromone[i][j] = (pheromone[i][j] + K) * (pheromone[i][j] + K);
        }
    }
}

#endif // __DECISION_HPP__