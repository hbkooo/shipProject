# 分析脚本使用说明

## 环境配置
```bash
# 需要安装python-pandas工具，安装命令：
$ sudo apt install -y python-pandas
```
## 脚本列表

|名称|功能|
|:--:|--|
|dataset.py|处理原始人脸图|
|correct.py|根据数据生成含有多标签表格|
|analysis.py|表格数据与接口返回数据对比,得出结果|

## 使用方法简述

- 首先，使用dataset.py脚本将收集到的人脸数据按照各种标签(如gender)进行预分类，并且存放到相应类别的文件夹中
- 其次，人工通过对预分类的数据进行纠错，保证所有数据划分正确
- 然后，使用correct.py脚本读取划分后的数据标签，生成一个含有多标签的csv格式数据表格文件
- 最后，使用analysis.py脚本调用verifier现有接口，对收集到的人脸数据进行处理，并同时读取csv格式文件对比表格中相对应的数据是否
一致，从而得出分析结果

## 使用命令
```bash
# 注册/修改人脸
$ python dataset.py
$ python correct.py
$ python analysis.py
```
