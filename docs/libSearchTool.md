# libSearchTool模块说明

<!-- TOC -->

- [libSearchTool模块说明](#libsearchtool模块说明)
    - [1 Redis模块](#1-redis模块)
        - [1.1 Redis](#11-redis)
        - [1.2 序列化](#12-序列化)
    - [2 检索模块](#2-检索模块)

<!-- /TOC -->
## 1 Redis模块

### 1.1 Redis
[redis操作][Redis-cmd]
- groupId(Hash)

|key|value|
|:--|:--|
|`[faceId]`|图片数|
|`[faceId]_hashkey_[count]`|`[faceId]`第`[count]`张图片特征的哈希码|

- verifier_hashcodes(set)
All hash codes.

- hashcode(Hash)

|key|value|
|:--|:--|
|`[groupId]-[faceId]_feat_[count]`|浮点数特征|

### 1.2 序列化

使用`Boost Serialization`库序列化与反序列化

## 2 检索模块

[Redis-cmd]:http://www.runoob.com/redis/redis-sets.html

