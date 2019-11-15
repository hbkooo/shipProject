##  monitor打包文件说明

|脚本|说明|
|:--:|:--:|
|monitor.sh|监控重启脚本|  
|pack.sh|打包发布脚本|  
|install.sh|安装脚本|  
|verifierMonitor_vx.x.x|用户软件安装包|  

## 打包脚本

```bash
# Current path is 'detector'
./scripts/license_monitor/pack.sh
# After this script is executed, a installer will be generate in 'release'
```

## 用户安装脚本
注意：打包加密狗版本的需安装加密狗的运行环境包
即：`${PROJECT_DIR}/deps/aksusbd_7.54-1_i386.deb `  
打包完成后，将release文件夹中的verifierMonitor_vx.x.x软件包交给用户。  
用户直接执行如下命令即可安装自动启动运行(首次启动一般需要一分钟)。并且程序在挂掉  
会在两秒内自动重启。
```bash   
$ chmod +x verifierMonitor_vx.x.x 
$ ./verifierMonitor_vx.x.x
``` 
  

## 彻底杀掉守护进程
先使用 `ps -ef |grep verifierMonitor` 查到守护进程号即 `PID` ,在执行 `crontab -r` 之后，  
使用 `kill PID` 彻底杀掉守护进程。
