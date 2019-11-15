##  scripts文件说明

|脚本|说明|
|:--:|:--:|
|git-lfs.deb.sh|git-lfs安装脚本|
|install_deps.sh|依赖安装脚本(OpenCV/Caffe/Redis)|
|monitor.sh|监控重启脚本|
|verifier_install.run|用户软件安装包|
|isntall_cuda.sh|安装cuda和cudnn脚本|
|install_runtime.sh|安装加密狗运行环境|
|pack.sh|打包发布脚本，无参数时未对程序加密，添加任意参数时对程序加密|

## 打包脚本

```bash
# Current path is 'verifier'
./scripts/pack.sh
# After this script is executed, a installer will be generate in 'release'
```

## 用户安装脚本

打包完成后，将release文件夹中的verifier_install.run软件包交给用户。  
用户直接执行./verifier_install.run即可安装。  
安装完成后插入加密锁，进入verifier目录，执行./verifier即可启动服务。  
