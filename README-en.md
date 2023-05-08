# AntTreeSearch
![](https://img.shields.io/badge/Version-0.1-orange.svg)

[English](README-en.md) | [简体中文](README.md)

This project is a C++ implementation of the distributed algorithm inspired by arboreal ant behavior for solving the shortest path problem, known as the Ant Tree Search (ATS) algorithm, based on the paper [1] Garg S, Shiragur K, Gordon D M, et al. Distributed algorithms from arboreal ants for the shortest path problem[J]. Proceedings of the National Academy of Sciences, 2023, 120(6): e2207959120.

Some key concepts used by ATS include pheromone trails, which are chemical signals left behind by ants as they move about; tree decomposition, which involves breaking down graphs into smaller subgraphs or "trees" to make them easier to navigate; and path leakage, which is the amount of information about the shortest path that is lost when the shortest path passes through a node.

To implement ATS, the authors of the paper use message passing between nodes in a network system. Each node represents an ant colony that searches for food along different paths in the graph. As each ant colony finds shorter paths using local information about their surrounding environment and updates pheromones (chemical signals) accordingly, other ant colonies can follow these paths to seek better solutions. This approach is scalable and efficient even when dealing with large graphs or complex networks, where traditional centralized methods may not work.

The implementation of this project is based on C++ and can be directly downloaded, compiled, and run. For detailed usage instructions, please refer to the README file in the project.

## Runtime Environment
- C++11
- CMake 3.22.1
- Ubuntu 20.04

## How to Use
- This project uses CMake for compilation, so CMake needs to be installed.

- Compile
```
cmake -B build && cmake --build build
```

- Run
```
./build/ATS <desicion_rule> <min_leakage>

(eg. ./build/ATS linear 0)

```

## Dataset
- Path: dataset/data
- Contains 1000 graphs. Each graph has 100 nodes, all of which are saved in txt files in the form of adjacency matrices.
- The types of graphs include gnp, gnp_loc, and grid.
  - gnp: Randomly generated edges, n represents the number of nodes, p represents the probability of edges.
  - gnp_loc: Gnp with Local Constraint, prevents the source node from reaching the target node directly, e.g., setting window_size = 10, then node i can generate edges only up to node i+10.
  - grid: Grid graph
- Based on whether the graph has a loop (w_loop/wo_loop) and whether leakage is considered (w_leakage/wo_leakage, not considering leakage means that a unique shortest path is generated in the graph, this type of instance is suitable for testing whether the ant colony can eventually converge to the shortest path; instances considering leakage may have multiple shortest paths, suitable for testing whether the ant colony can eventually converge to the minimum leakage path), the dataset can be further divided into 10 categories, with 100 instances per category.
- For gnp and gnp_loc types of graphs, we set p to 0.1 and 0.5 according to the description in the paper (for the first 50 instances, p is set to 0.1, and for the last 50 instances, p is set to 0.5).

## Project Collaborators
<a href="https://github.com/PKUcoldkeyboard/AntTreeSearch/graphs/contributors">
<img src="https://contrib.rocks./image?repo=PKUcoldkeyboard/AntTreeSearch" />
</a>

Made with contrib.rocks.

## License
This project is licensed under the MIT License. See the LICENSE file for more information.

## References
[1] Garg S, Shiragur K, Gordon D M, et al. Distributed algorithms from arboreal ants for the shortest path problem[J]. Proceedings of the National Academy of Sciences, 2023, 120(6): e2207959120.