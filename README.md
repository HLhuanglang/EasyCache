# EasyCache

## 概述

EasyCache主要用于研究缓存技术的原理和编码实现
- local_cache分支采用cpp实现，作为程序的一部分
- remote_cache采用go实现，可单独部署，作为缓存服务与逻辑服务进行通信。

## 关于缓存的理解

缓存并不是说指代某一个具体的软件，而是关于存储问题/性能问题的一种解决方案。

## 缓存分类

### 宿主层次

按宿主层次分类的话，缓存一般可以分为**本地 Cache**、**进程间 Cache** 和**远程 Cache**。

- 本地 Cache 是指业务进程内的缓存，这类缓存由于在业务系统进程内，所以读写性能超高且无任何网络开销，但不足是会随着业务系统重启而丢失。
- 进程间 Cache 是本机独立运行的缓存，这类缓存读写性能较高，不会随着业务系统重启丢数据，并且可以大幅减少网络开销，但不足是业务系统和缓存都在相同宿主机，运维复杂，且存在资源竞争。
- 远程 Cache 是指跨机器部署的缓存，这类缓存因为独立设备部署，容量大且易扩展，在互联网企业使用最广泛。不过远程缓存需要跨机访问，在高读写压力下，带宽容易成为瓶颈。

本地 Cache 的缓存组件有 Ehcache、Guava Cache 等，开发者自己也可以用 Map、Set 等轻松构建一个自己专用的本地 Cache。
**进程间 Cache 和远程 Cache 的缓存组件相同**，只是部署位置的差异罢了，这类缓存组件有 Memcached、Redis、Pika 等。

### 存储介质

还有一种常见的分类方式是按存储介质来分，这样可以分为内存型缓存和持久化型缓存。

- **内存型缓存**将数据存储在内存，读写性能很高，但缓存系统重启或 Crash 后，内存数据会丢失。
- **持久化型缓存**将数据存储到 SSD/Fusion-IO 硬盘中，相同成本下，这种缓存的容量会比内存型缓存大 1 个数量级以上，而且数据会持久化落地，重启不丢失，但读写性能相对低 1～2 个数量级。Memcached 是典型的内存型缓存，而 Pika 以及其他基于 RocksDB 开发的缓存组件等则属于持久化型缓存。


## 缓存模式

![img](https://hl1998-1255562705.cos.ap-shanghai.myqcloud.com/Img/%E4%BC%81%E4%B8%9A%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_16769758055025.png)

### Cache Aside（旁路缓存）

<font color='red'>核心点在于DB侧</font>，数据以DB为准：

- 读：读cache未命中后读DB，并将DB的数据写到cache
- 写：更新DB，删除cache，由db驱动cache更新
- 特点：Lazy计算，以DB数据为准
- 适用场景：更强一致性；Cache数据构建复杂

### Read/Write Through（读写穿透）

对于 Cache Aside 模式，业务应用需要同时维护 cache 和 DB 两个数据存储方，过于繁琐，于是就有了 Read/Write Through 模式。在这种模式下，业务应用只关注一个存储服务即可。<font color='red'>核心点在于封装成存储服务，屏蔽内部细节</font>。

<img src="https://hl1998-1255562705.cos.ap-shanghai.myqcloud.com/Img/image-20230221185836329.png" alt="image-20230221185836329" style="zoom:50%;" />

### Write Behind Caching（异步缓存写入）

Write Behind Caching 模式与 Read/Write Through 模式类似，也由数据存储服务来管理 cache 和 DB 的读写。不同点是，数据更新时，Read/write Through 是同步更新 cache 和 DB，而 Write Behind Caching 则是只更新缓存，不直接更新 DB，而是改为异步批量的方式来更新 DB。**该模式的特点是，数据存储的写性能最高，非常适合一些变更特别频繁的业务，特别是可以合并写请求的业务**，比如对一些计数业务，一条 Feed 被点赞 1万 次，如果更新 1万 次 DB 代价很大，而合并成一次请求直接加 1万，则是一个非常轻量的操作。但**这种模型有个显著的缺点，即数据的一致性变差，甚至在一些极端场景下可能会丢失数据**。比如系统 Crash、机器宕机时，如果有数据还没保存到 DB，则会存在丢失的风险。所以这种读写模式适合变更频率特别高，但对一致性要求不太高的业务，这样写操作可以异步批量写入 DB，减小 DB 压力。<font color='red'>核心点在于异步批量更新DB数据</font>

<img src="https://hl1998-1255562705.cos.ap-shanghai.myqcloud.com/Img/image-20230221190301284.png" alt="image-20230221190301284" style="zoom:50%;" />

## 缓存经典问题

<img src="https://hl1998-1255562705.cos.ap-shanghai.myqcloud.com/Img/image-20230221192038522.png" alt="image-20230221192038522" style="zoom:67%;" />

## 参考项目/文章/论文

[后续补充]

## 授权许可
本项目采用 MIT 开源授权许可证，完整的授权说明已放置在 [LICENSE](LICENSE) 文件中。
