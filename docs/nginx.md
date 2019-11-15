<!-- TOC -->

- [1. nginx安装](#1-nginx安装)
- [2. nginx使用](#2-nginx使用)
- [3. nginx配置](#3-nginx配置)
    - [3.1. 配置文件介绍](#31-配置文件介绍)
    - [3.2. 负载均衡配置](#32-负载均衡配置)
        - [3.2.1. 配置参数说明](#321-配置参数说明)
    - [3.3. 日志配置](#33-日志配置)
- [4. 参考](#4-参考)

<!-- /TOC -->

# 1. nginx安装

如果是在`ubuntu`系统下，可以直接使用命令行一键安装，安装完后也会自动启动`nginx`服务。

```bash
$ sudo apt-get install nginx
```

如果是在`mac`下，可以使用`brew`安装。

```bash
$ brew install nginx
```

# 2. nginx使用

```bash
# 启动nginx
$ sudo nginx
# 重启服务
$ sudo service nginx restart
# 快速关闭
$ sudo nginx -s stop
# 优雅关闭（等待worker线程完成处理）
$ sudo nginx -s quit
# 重载配置文件
$ sudo nginx -s reload
# 重新打开配置文件
$ sudo nginx -s reopen
```

# 3. nginx配置

## 3.1. 配置文件介绍

`nginx`配置文件，默认位置包括：

- /etc/nginx/nginx.conf
- /etc/nginx/conf.d/*
- /etc/nginx/sites-available/*

`/etc/nginx/nginx.conf`是主配置文件。有时候仅仅一个配置文件是不够的，由其是当配置文件很大时，不能全部逻辑塞一个文件里，所以配置文件也是需要来管理的。其`http`块最后两行`include`，也就是说会自动包含目录`/etc/nginx/conf.d/`的以`conf`结尾的文件，还有目录`/etc/nginx/sites-enabled/`下的所有文件。

`nginx`配置文件主要分为六个区域：`main(全局配置)`、`events(nginx工作模式)`、`http(http配置)`、`server(主机设置)`、`location(URL匹配)`、`upstream(负载均衡服务器设置)`。

## 3.2. 负载均衡配置

以代理服务器`ip`为`192.168.1.7`，两台处理请求的服务器`ip`分别为`192.168.1.7`和`192.168.1.8`为例：

在`/etc/nginx/conf.d/`下新建文件`verifier.conf`，内容如下：

```conf
upstream verifier_group {
#   ip_hash;
    server 192.168.1.7:33388 weight=1 max_fails=3 fail_timeout=30s;
    server 192.168.1.8:33388 weight=1 max_fails=3 fail_timeout=30s;
}
server {
    listen 80;
    server_name 192.168.1.7;
    location / {
        proxy_pass http://verifier_group;
    }
}
```

配置完成后，请求的服务器`ip`和端口号为`server_name`和`listen`对应的值，此处即为`192.168.1.7`和`80`。

> PS: 不同的机器需要修改相应的`ip`和端口号

### 3.2.1. 配置参数说明

`upstream`模块负责负载均衡，通过一个简单的调度算法来实现客户端`IP`到后端服务器的负载均衡。其参数详解：

|参数|说明|
|:---:|:---:|
|verifier_group|负载均衡组，名称可以任意指定，在后面需要的地方直接调用即可|
|least_conn|它是优先发送给那些接受请求少的，目的是为了让请求分发得更平衡|
|ip_hash|通知`nginx`使用`ip hash`负载均衡算法，默认情况下，如果不指定方式，就是随机轮循(round-robin)，所有`server`被随机访问|
|server|服务器，有新服务器就添加一条|
|weight|调度的权重|
|max_fails|允许请求失败的次数|
|fail_timeout|在经历了`max_fails`次失败后，暂停服务的时间|
|down|表示当前的`server`暂时不参与负载均衡|
|backup|预留的备份机器。当其他所有的非`backup`机器出现故障或者忙的时候，才会请求`backup`机器，因此这台机器的压力最轻|

> PS：当负载调度算法为`ip_hash`时，后端服务器在负载均衡调度中的状态不能是`weight`和`backup`。

`server`模块是`http`的子模块，用来定一个虚拟主机。配置参数如下：

|参数|说明|
|:---:|:---:|
|server|标志定义虚拟主机开始|
|listen|用于指定虚拟主机的服务端口|
|server_name|用来指定IP地址或者域名，多个域名之间用空格分开|
|location|`location`模块|

`location`模块是用来定位的，定位`URL`，解析`URL`，它提供了强大的正则匹配功能，也支持条件判断匹配。

|参数|说明|
|:---:|:---:|
|proxy_pass|用来指定代理的后端服务器地址和端口，地址可以是主机名或者`IP`地址，也可以是通过`upstream`指令设定的负载均衡组名称|

`location`的正则匹配规则如下：

```conf
location  = / {
  # 精确匹配 / ，主机名后面不能带任何字符串
  [ configuration A ]
}
location  / {
  # 因为所有的地址都以 / 开头，所以这条规则将匹配到所有请求
  # 但是正则和最长字符串会优先匹配
  [ configuration B ]
}
location /documents/ {
  # 匹配任何以 /documents/ 开头的地址，匹配符合以后，还要继续往下搜索
  # 只有后面的正则表达式没有匹配到时，这一条才会采用这一条
  [ configuration C ]
}
location ~ /documents/Abc {
  # 匹配任何以 /documents/Abc 开头的地址，匹配符合以后，还要继续往下搜索
  # 只有后面的正则表达式没有匹配到时，这一条才会采用这一条
  [ configuration CC ]
}
location ^~ /images/ {
  # 匹配任何以 /images/ 开头的地址，匹配符合以后，停止往下搜索正则，采用这一条。
  [ configuration D ]
}
location ~* \.(gif|jpg|jpeg)$ {
  # 匹配所有以 gif,jpg或jpeg 结尾的请求
  # 然而，所有请求 /images/ 下的图片会被 config D 处理，因为 ^~ 到达不了这一条正则
  [ configuration E ]
}
location /images/ {
  # 字符匹配到 /images/，继续往下，会发现 ^~ 存在
  [ configuration F ]
}
location /images/abc {
  # 最长字符匹配到 /images/abc，继续往下，会发现 ^~ 存在
  # F与G的放置顺序是没有关系的
  [ configuration G ]
}
location ~ /images/abc/ {
  # 只有去掉 config D 才有效：先最长匹配 config G 开头的地址，继续往下搜索，匹配到这一条正则，采用
    [ configuration H ]
}
location ~* /js/.*/\.js
```

## 3.3. 日志配置

默认的日志输出在`/var/log/nginx/access.log`和`/var/log/nginx/error.log`。默认日志格式不包括分发到的服务器`ip`，可在`http`块中重新配置（新的日志格式命令为`verifierlog`）：

```conf
##
# Logging Settings
##
log_format verifierlog '$remote_addr - $remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent" "$http_x_forwarded_for" $upstream_addr $upstream_response_time $request_time';
access_log /var/log/nginx/access.log verifierlog;
error_log /var/log/nginx/error.log;
```

# 4. 参考

1. [Welcome to NGINX Wiki!](https://www.nginx.com/resources/wiki/)
2. [nginx的配置、虚拟主机、负载均衡和反向代理](https://www.zybuluo.com/phper/note/89391)
3. [nginx教程](https://nginx.rails365.net/chapters/1.html)