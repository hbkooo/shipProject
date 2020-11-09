# Verifier

<!-- TOC -->

- [Verifier](#verifier)
    - [1 概述](#1-概述)
    - [2 使用](#2-使用)
        - [2.1 安装依赖](#21-安装依赖)
        - [2.2 编译运行](#22-编译运行)
        - [2.3 测试和安装](#23-测试和安装)
        - [2.4 打包发布](#24-打包发布)
            - [2.4.1 不加密打包](#241-不加密打包)
            - [2.4.2 加密程序及模型打包](#242-加密程序及模型打包)
        - [2.5 版本发布](#25-版本发布)
    - [3 文档](#3-文档)
        - [3.1 项目文档](#31-项目文档)
        - [3.2 参考文档](#32-参考文档)
        - [3.3 Todo List](#33-todo-list)
    - [4 依赖项目](#4-依赖项目)
    - [5 算法扩充](#5-算法扩充)

<!-- /TOC -->

## 1 概述

|目录|说明|
|:--:|:--:|
|cmake|cmake脚本文件|
|deps|项目依赖文件|
|docs|项目相关文档|
|model|caffe模型文件|
|scripts|项目相关的脚本|
|src|项目源码（按模块组织）|
|tools|项目相关的工具|
|test|项目测试使用的脚本|

## 2 使用

### 2.1 安装依赖

**PS:** 系统中不能同时存在`OpenCV 2`和`OpenCV 3`，如果存在需要修改`ROOT/CMakeLists.txt`中`set(CMAKE_PREFIX_PATH "home/hbk/opencv-3.4.0/build" ${CMAKE_PREFIX_PATH})`设置自己的安装路径
- opencv3.4
- python2

### 2.2 编译运行

### 2.2.1 编译caffe
编译caffe时要求python版本为python2.7
```bash
$ cd ROOT
$ cd deps/caffe_ship/caffe_fast_rcnn
$ mkdir build
$ cd build
$ cmake ..
$ make -j8
$ make pycaffe
$ cd ../lib
$ make
$ cd rotation
$ cp rotate_polygon_nms.so librotate_polygon_nms.so
```

### 2.2.2 编译yolo
```bash
$ cd ROOT
$ cd deps/yolo
$ make -j8
```

### 2.2.3 编译verifier
首先修改[cmake/ProjectCaffeShip.cmake](cmake/ProjectCaffeShip.cmake)文件中的个人的python路径，要求python版本为Python2
```bash
$ cd ROOT
$ mkdir build
$ cd build
$ cmake ..
$ make -j8
```

### 2.2.4 运行服务
```bash
$ cd ROOT/build
$ ./bin/verifier
```

### 2.2.5 运行路由
```bash
$ cd ROOT/build
$ ./bin/router
```

### 2.2.6 测试
```bash
# test, cmake with -DBUILD_TESTS=ON ..
$ cd ROOT/build
$ ./bin/zmqClient
```

**PS:** 当配置文件不存在时（首次运行），会使用默认的配置参数，并在当前目录下自动生成配置文件`config.json`

### 2.5 版本发布

- 修改版本号：修改最外层`CMakeLists.txt`文件中的`VERSION_MAJOR.VERSION_MINOR.VERSION_PATCH`值为相应版本号
- 合并分支：发起PR将当前分支合并到develop分支,测试通过后合并develop到master分支。
- 添加标签：以相应的版本号新建标签。

>**PS**:[版本号命名规则](https://semver.org/)

## 3 文档

### 3.1 项目文档

```bash
# 生成文档位于build/documents
$ make doc
```

1. [代码规范](docs/Code-Style.md)
1. [API说明](docs/API.md)
1. [配置说明](docs/config.md)
1. [性能说明](docs/Performance.md)
1. [框架图](docs/Framework.vsdx)

### 3.2 参考文档

1. [Git-LFS使用说明](docs/Git-LFS-usage.md) 
1. [Semantic Versioning](https://semver.org/)
1. [nginx使用指南](docs/nginx.md)
1. [12-Factor标准](https://12factor.net/zh_cn/)

### 3.3 Todo List

- [ ] GPU检索接口初步实现
- [ ] GPU检索接口性能优化
- [ ] 升级模块处理模型升级与软件升级
- [ ] Docker镜像优化与部署(硬件锁)
- [ ] Kubernets部署尝试(集群化)
- [ ] EASYLOG的规范使用
- [ ] HTTPS支持
- [ ] API性能瓶颈与优化

## 4 依赖项目

- [`redis/hiredis`](https://github.com/redis/hiredis)
- [`BVLC/caffe`](https://github.com/BVLC/caffe)
- [`cpp-netlib/cpp-netlib`](https://github.com/cpp-netlib/cpp-netlib)
- [`SRombauts/SQLiteCpp`](https://github.com/SRombauts/SQLiteCpp)
- [`oktal/pistache`](https://github.com/oktal/pistache)
- [`boost`](http://www.boost.org/)
- [`open-source-parsers/jsoncpp`](https://github.com/open-source-parsers/jsoncpp)
- [`muflihun/easyloggingpp`](https://github.com/muflihun/easyloggingpp)

## 5 算法扩充

### [common](src/common)

- 文件[common.h](src/common/common.h)中，在枚举变量*INTERFACEID*中新增新的算法端口用于客户端的请求端口。
比如INTERFACEID_×× = ×，注意里面的*INTERFACEID_COUNT*始终是最大值，新增的 INTERFACEID_×× 始终为INTERFACEID_COUNT减一。
- 文件[configParams.h](src/common/ConfigParams.h)中，新增新的成员变量int detect××PoolSize = 1;可以直接放在detectYoloPoolSize变量下面。

### [libAlgorithm](src/libAlgorithm)

- 在*src/libAlgorithm*文件夹下新增新的算法实现××.cpp和××.h文件
- 如果算法有依赖新的库需要在该文件夹下的CMakeLists.txt文件中新增对应的依赖的库语句。

### [libTaskModel](src/libTaskModel)

- 在*src/libTaskModel*文件夹下新增*Handler××.cpp*和*Handler××.h*文件,仿照*HandlerShip.cpp*和*HandlerShip.h*的内容进行代码编写，调用在*libAlgorithm*文件夹下创建的新的算法*××.h*和*××.cpp*。
- 在*src/libTaskModel*文件夹下的*HandlerBase.h*文件中，在最上面利用define命令宏定义一个算法方法*××_METHOD_ID*，用于标识在verifier内部进行网络加载时的区别，作为算法的*handler_id*，宏定义的此变量会在*HandlerFactory.cpp*中使用到。
- 在*src/libTaskModel*文件夹下的*HandlerFactory.cpp*文件的*create函数*中，在*switch语句块中新增case单元*，指定在*HandlerBase.h*文件中定义的新的算法的标识*××_METHOD_ID*，并按照对应的格式返回结果。

### [zmqServer](src/zmqServer)

- 文件[ZmqServer.cpp](src/zmqServer/ZmqServer.cpp)中，在*ZmqServer*对象的构造函数中，根据舰船、红外、民用的检测调用过程创建对应的新的算法队列*××Queue*，创建这个队列时需要指定要创建的算法标识，即在*HandlerBase.h*文件中宏定义的*××_METHOD_ID*，然后将该算法队列的*INTERFACE_××*与*dispatcher*消息分发者绑定。




