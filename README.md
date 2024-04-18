# EasyCache

## 1.概述

EasyCache主要用于研究缓存技术的原理和编码实现
- 以源代码形式引入
- 以服务模式部署
    - 可单机部署
    - 多机分布式部署

## 2.支持平台
- linux

## 3.依赖
- gcc>=4.8.1(支持c++11)
- git
- cmake

## 4.编译运行
```
git clone --recurse-submodules https://github.com/HLhuanglang/EasyCache.git
cmake -S . -B build
cmake --build build
```

## 5.目录说明

| 目录         | 作用                                               |
| ------------ | -------------------------------------------------- |
| .vscode      | vscode开发环境配置                                 |
| docs         | 开发文档                                           |
| easycache     | 缓存库/服务       |
| examples     | 使用easycache编写的demo       |
| test         | 测试代码                                           |
| build        | 编译的中间产物、可执行程序等     |
| third_party      | 依赖使用的第三方库                                 |


## 6.参考项目/文章/论文

[后续补充]

## 7.授权许可
本项目采用 MIT 开源授权许可证，完整的授权说明已放置在 [LICENSE](LICENSE) 文件中。
