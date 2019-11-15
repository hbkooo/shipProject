# 说明

## lockerTool

用于测试加密锁的相关功能，整个项目编译完成后会在verifier/build/bin目录下生成lockerTool可执行文件。执行程序前请确保对应的加密锁已经插入主机。  

```bash
usage: ./lockerTool [options] ... 
options:
  -s, --show       Show basic information of the lockController.
  -r, --read       Read and print internal data of the lockController.
  -w, --write      File to write into the lockController. (string [=])
  -e, --encrypt    File to encrypt. (string [=])
  -d, --decrypt    File to decrypt. (string [=])
  -t, --test       Test the 'getConfig' funcation.
  -?, --help       print this message
```

```bash
# Example
# Current path is verifier/build
$ ./bin/lockerTool -w ../config.json 
     7B 0A 09 22 | 6C 69 73 74 | 65 6E 69 70 | 22 3A 20 22  [{.."listenip": "]
     30 2E 30 2E | 30 2E 30 22 | 2C 0A 09 22 | 70 6F 72 74  [0.0.0.0",.."port]
     22 3A 20 33 | 33 33 38 38 | 2C 0A 09 22 | 67 70 75 69  [": 33388,.."gpui]
     64 22 3A 20 | 30 2C 0A 09 | 22 68 74 74 | 70 54 68 72  [d": 0,.."httpThr]
     65 61 64 73 | 22 3A 20 32 | 2C 09 0A 09 | 22 63 61 66  [eads": 2,..."caf]
     66 65 54 68 | 72 65 61 64 | 73 22 3A 20 | 34 2C 0A 09  [feThreads": 4,..]
     22 6D 6F 64 | 65 6C 70 61 | 74 68 22 3A | 20 22 2E 2E  ["modelpath": "..]
     2F 6D 6F 64 | 65 6C 2F 22 | 2C 0A 09 22 | 6C 6F 67 63  [/model/",.."logc]
     6F 6E 66 22 | 3A 20 22 6C | 6F 67 2E 63 | 6F 6E 66 22  [onf": "log.conf"]
     2C 0A 09 22 | 73 65 61 72 | 63 68 54 6F | 6F 6C 50 61  [,.."searchToolPa]
     72 61 6D 73 | 22 3A 20 7B | 0A 09 09 22 | 72 65 64 69  [rams": {..."redi]
     73 5F 64 62 | 5F 69 64 22 | 3A 20 35 2C | 0A 09 09 22  [s_db_id": 5,..."]
     72 65 64 69 | 73 5F 70 61 | 73 73 77 64 | 22 3A 20 22  [redis_passwd": "]
     22 2C 0A 09 | 09 22 74 6F | 70 4B 22 3A | 20 38 30 2C  [",..."topK": 80,]
     0A 09 09 22 | 64 69 73 74 | 61 6E 63 65 | 22 3A 20 32  [..."distance": 2]
     35 30 2C 0A | 09 09 22 73 | 69 6D 54 68 | 72 65 73 68  [50,..."simThresh]
     6F 6C 64 22 | 3A 20 30 2E | 36 36 2C 0A | 09 09 22 73  [old": 0.66,..."s]
     65 61 72 63 | 68 54 68 72 | 65 73 68 6F | 6C 64 22 3A  [earchThreshold":]
     20 30 2E 37 | 35 0A 09 7D | 0A 7D 0A FF |              [ 0.75..}.}..    ]
```

## userManager

手动录入用户信息并写入数据库中。  

```bash
# current path is verifier/tools
cd userManager
python3 userManager.py
```

```bash
# Example
$ python3 userManager.py
=========================================================
Press numbers to select function: 
[1] Add information of a new user to database. 
[2] Update information of an exist user . 
[3] Delete information of an exist user of the database. 
[4] Search information of a exist user of the database. 
[5] Show information of a all user of the database. 
[0] Quit. 
=========================================================
5
('a225e468-5c4e-4354-b34b-c58c6db75f3d', 'sunhao', '华中科技大学', '123456', 'dddd', '软件园')
```

