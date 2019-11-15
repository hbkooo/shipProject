# 武汉市公安局测试

## 使用说明

### 一键建库脚本

该脚本包括根据图片库生成一个或者两个图片列表、读取列表获取对应图片调用一块或者两块GPU进行特征提取操作。

```bash
$ ./buildStatic.sh
Usage: ./buildStatic.sh [dataset type] [GPUs] [block size]
dataset type   : 'base' for base photo, 'query' for query photo.
use GPUs       : '0' to use 1 GPU, '1' to use 2 GPUs. DEFAULT: 0
block size     : number of features for each file. DEFAULT: 1000

# Example of building base database
$ ./buildStatic.sh base 1 2500000 # use 2 GPUs

# 动态建库
$ ./buildDynamic.sh
```

### 一键检索脚本

执行该脚本即可开始建程序库然后检索,检索完成后生成result.cvs文件。

```bash
#Example
$ ./staticSearch.sh
```

### 基础图片建库规则

在建库脚本中调用建库程序进行建库。建库过程中，生成的特征分块保存在多个文件中。
特征文件存储在特定目录下，目录的生成规则为`[图片列表文件后缀]Feature`。
同时根据机器GPU数量生成一个或两个文本文件用于保存特征值与相应图片的映射关系，映射文件命名规则为`id_name_list_[GPUId]`, 内容为`[id] [iamge_path]`。
特征文件的命名规则为`[编号]_[GPUId].fmem`，文件中的内容按如下规则存储：

- 前20个字节保存文件基本信息：0-3字节保存特征向量维数dim，4-7直接保存该文件中特征向量的数量num，8-11字节保存第一个特征向量对应的图片的编号，12-19字节为0
- 后面保存的是num个dim维的特征向量

### 查询图片建库规则

在建库脚本中调用建库程序进行建库。建库过程中，生成的特征文件存储在特定目录下，目录的生成规则为`[图片列表文件后缀]Feature`。
同时根据生成一个文本文件用于保存特征值与相应图片的映射关系，映射文件命名规则为`id_name_list`, 内容为`[id] [iamge_path]`。
特征文件的命名规则为"[编号].fmem"，文件中的内容按如下规则存储：

- 前20个字节保存文件基本信息：0-3字节保存特征向量维数dim，4-7直接保存该文件中特征向量的数量num，8-19字节为0
- 后面保存的是num个dim维的特征向量

## 文件组织结构

```bash
.
├── buildStatic.sh #建库脚本
├── config.json # 配置文件
├── data # 数据文件夹
│   ├── baseFeature
│   ├── basePhoto
│   ├── fixdata
│   ├── queryFeature
│   └── queryPhoto
├── getList.py # 生成图片列表程序
├── image_list_0.base # 分割后的图片列表
├── image_list_1.base # 分割后的图片列表
├── image_list.base # 基础图片库列表
├── image_list.query # 查询图片库列表
├── model # 模型
├── query # 检索程序
├── result.csv # 结果
├── staticSearch.sh # 检索脚本
└── wuhanTest # 建库程序
```

## 现场操作流程

- 准备好程序、脚本及配置文件
- 拷贝数据集
- 根据图片组织形式修改`writer.cpp`中`writeRedis()`函数的字符串分割
- 执行"./buildStatic.sh base 1 10000"开始建基础库
- 执行"./buildStatic.sh query"开始建查询库
- 执行"./staticSearch.sh"开始检索

## 性能测试

在1060机器处理 10555000 张图耗时 78456 秒，单卡平均 100 张/秒
测试图片数量：10000张
图片大小：10KB左右

|GPU型号|SMs(128cores/MP)|启用caffe线程数|耗时ms(三次测试)|平均耗时ms|
|:--|:--|:--|:--|:--|
|1080 TI|28|4|(65218,65562,65832)|65537.34|
|1080 TI|28|5|(65441,65508,65013)|**65320.67**|
|1080 TI|28|6|(66010,67890,68336)|67412|
|1080 TI|28|7|(67521,68071,67882)|67824.67|
|1080 TI|28|8|(69897,74337,68327)|70853.67|
|1060|10|2|(81199,81357,81157)|81237.67|
|1060|10|3|(76426,77433,76871)|76910|
|1060|10|4|(75921,76164,75828)|75971|
|1080 TIAN X|24|4|(96695,97775,98254)|97574.67|
|1080 TIAN X|24|6|(99568,99375,98277)|99073.34|
|1080 TIAN X|24|8|(99488,100095,100560)|100047.67|
