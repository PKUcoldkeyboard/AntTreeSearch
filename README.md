# AntTreeSearch
![](https://img.shields.io/badge/Version-0.1-orange.svg)
![](https://img.shields.io/github/stars/PKUcoldkeyboard/AntTreeSearch?style=social)

[English](README-en.md) | [简体中文](README.md)

本项目是根据论文[1] Garg S, Shiragur K, Gordon D M, et al. Distributed algorithms from arboreal ants for the shortest path problem[J]. Proceedings of the National Academy of Sciences, 2023, 120(6): e2207959120，基于C++复现的受树栖蚂蚁行为启发的用于解决最短路径问题的分布式算法————蚂蚁树搜索（ATS）算法。

ATS使用的一些关键概念包括信息素轨迹，即蚂蚁四处移动时留下的化学信号；以及树木分解，即将图表分解成较小的子图或 “树” 以使其更易于导航；路径泄漏，即在最短路径通过节点时丢失了多少有关最短路径的信息。

为了实现 ATS，论文作者使用网络系统中节点之间的消息传递。每个节点代表一个蚁群，它们在图表中沿着不同的路径寻找食物。当每个蚁群利用有关其周围环境的本地信息找到较短的路径并相应地更新费洛蒙（信息素）时，其他蚁群可以沿着这些路径寻求更好的解决方案。即使在处理大型图表或复杂网络时，这种方法也具有可扩展性和高效性，而传统的集中式方法可能不起作用。

本项目的实现基于C++，可以直接下载源代码并编译运行。详细的使用方法请参考项目中的 README 文件。

## 运行环境
- C++11
- CMake 3.22.1
- Ubuntu 20.04

## 如何使用
- 本项目使用CMake进行编译，因此需要安装CMake。

- 编译
```
cmake -B build && cmake --build build
```
- 运行
```
./build/ATS <desicion_rule> <min_leakage>

(eg. ./build/ATS linear 0)

```

## 数据集
- 路径：dataset/data
- 包含1000张图。每张图有100个结点，都以邻接矩阵的形式保存在txt文件当中。
- 图的类型包括gnp，gnp_loc，grid三种。
  - gnp: 随机生成边，n表示结点数，p表示边的概率。
  - gnp_loc：Gnp with Local Constraint，避免源节点一下子到达目标节点，例如设置window_size = 10，那么编号为i的节点最多只能生成到编号为i+10节点的边。
  - grid：网格图
- 根据图是否有环(w_loop/wo_loop)、是否考虑泄露（w_leakage/wo_leakage, 不考虑泄露表示图中生成了一条唯一的最短路径，这种实例适用于测验蚁群最终能否收敛到最短路径；考虑泄露的实例可能有多条最短路径，适用于测验蚁群最终能否收敛到最小泄露路径），数据集可再细分为10种类别，每种类别有100个实例。
- 对于gnp和gnp_loc这两种图，我们按论文中的描述分别把p设置为0.1和0.5（对于前50个实例p设为0.1，后50个则设为0.5）。

## 项目协作者

<a href="https://github.com/PKUcoldkeyboard/AntTreeSearch/graphs/contributors">
  <img src="https://contrib.rocks./image?repo=PKUcoldkeyboard/AntTreeSearch" />
</a>

Made with [contrib.rocks](https://contrib.rocks.).

## 许可证

This project is licensed under the MIT License. See the LICENSE file for more information.

## 参考文献

[1] Garg S, Shiragur K, Gordon D M, et al. Distributed algorithms from arboreal ants for the shortest path problem[J]. Proceedings of the National Academy of Sciences, 2023, 120(6): e2207959120.


