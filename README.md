# C-Arboreal-Ants

本项目是根据论文[1] Garg S, Shiragur K, Gordon D M, et al. Distributed algorithms from arboreal ants for the shortest path problem[J]. Proceedings of the National Academy of Sciences, 2023, 120(6): e2207959120，基于C语言复现受树栖蚂蚁行为启发的用于解决最短路径问题的分布式算法————蚂蚁树搜索（ATS）算法。

ATS使用的一些关键概念包括信息素轨迹，即蚂蚁四处移动时留下的化学信号；以及树木分解，即将图表分解成较小的子图或 “树” 以使其更易于导航；路径泄漏，即在最短路径通过节点时丢失了多少有关最短路径的信息。

为了实现 ATS，论文作者使用网络系统中节点之间的消息传递。每个节点代表一个蚁群，它们在图表中沿着不同的路径寻找食物。当每个蚁群利用有关其周围环境的本地信息找到较短的路径并相应地更新费洛蒙（信息素）时，其他蚁群可以沿着这些路径寻求更好的解决方案。即使在处理大型图表或复杂网络时，这种方法也具有可扩展性和高效性，而传统的集中式方法可能不起作用。

本项目的实现是基于C语言，可以直接下载源代码并编译运行。详细的使用方法请参考项目中的 README 文件。

## 算法原理

### 图族类型

1. 从随机图模型$G(n,p)$中采样，其中 n 是顶点数，p 是任意两个顶点之间存在边的概率。
    - 第一种类型：只允许在 i < j 时从顶点 i 到 j 的边，从而生成有向无环图(DAG)，论文中称为$G(n, p)$ DAG
    - 第二种类型：允许边缘双向移动，从而生成有向图, 论文中称为$G(n, p)$ local DAG
2. 从随即图模型$G(n,p)$中采样，但还有一个额外限制，即只有当 i 和 j 之间的绝对差小于或等于参数 k 时，边才能存在于顶点 i 和 j 之间。此约束确保图在源顶点和目标顶点之间有长路径，分别编号为 1 和 n。
    - 第一种类型：只允许在 i < j 时从顶点 i 到 j 的边，从而生成有向无环图(DAG)，论文中称为$G(n, p)$ DAG
    - 第二种类型：允许边缘双向移动，从而生成有向图, 论文中称为$G(n, p)$ local DAG
3. 网格图

### 线性规则

- 固定流量泄露：
- 无泄漏的增加流量：

### 非线性规则

- Quadratic: 该规则按信息素水平的平方比例分配流量。
- 1.1 Power: 该规则按照信息素水平的1.1次方比例分配流量。
- Quadratic-with-offset: Quadratic规则的一个轻微变体，该规则将一个固定的正常数c加到信息素水平上，并按照偏移信息素水平的平方比例分配流量。
- Rank-edge: 该规则将边缘按照信息素水平从高到低排序。如果多条边具有相同的信息素水平，则它们获得相同的排名。

## 如何使用

You can start by cloning our repository and following the steps below.

## 项目协作者

- @PKUcoldkeyboard: the owner of the project
- @putao537: one developer of the project
- @wangweidacv: one developer of the project
- @flower3d: one developer of the project
- @homzer: one developer of the project
- @ninmiin: one developer of the project

## 许可证

This project is licensed under the MIT License. See the LICENSE file for more information.

## 参考文献

[1] Garg S, Shiragur K, Gordon D M, et al. Distributed algorithms from arboreal ants for the shortest path problem[J]. Proceedings of the National Academy of Sciences, 2023, 120(6): e2207959120.


