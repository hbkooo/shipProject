# 1. git lfs 的使用

Git LFS（Large File Storage，大文件存储）是可以用 `Git`管理大文件的扩展命令，下载的网址为： <https://github.com/git-lfs/git-lfs>。

## 1.1. 安装 git lfs

安装可参考 <https://github.com/git-lfs/git-lfs/blob/master/INSTALLING.md>，或者按照如下命令安装：

- `mac` 下使用 `homebrew` 安装：
```bash
$ brew install git-lfs
```

- `linux` 下安装：
```bash
$ curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.deb.sh | sudo bash
$ sudo apt-get install git-lfs
```

安装完之后，需要执行如下的命令来设置 `the global Git hooks for Git LFS`。每台机器只需要执行一次。
```bash
$ git lfs install
```

## 1.2. 使用 git lfs

常用的 `Git LFS` 命令:

### 1.2.1. 文件跟踪：

```bash
# 针对使用了 LFS 的仓库进行了特别优化的 clone 命令，显著提升获取 LFS 对象的速度，接受和 git clone 一样的参数，该命令同样兼容没有使用 LFS 的仓库。
$ git lfs clone https://github.com/git-lfs/git-lfs.git

# 使用 Git LFS 管理指定的文件
$ git lfs track "*.mp4"

# 查看当前的管理列表
$ git lfs track

# 不再使用 Git LFS 管理指定的文件
$ git lfs untrack "*.mp4"

# 查看当前 Git LFS 对象的状态
$ git lfs status

# 枚举目前被 Git LFS 管理的具体文件
$ git lfs ls-files

# 查看当前所用的 Git LFS 的版本
$ git lfs version

# 获取帮助
$ git lfs help <subcommand>
```

### 1.2.2. 文件拉取：

```bash
# 只获取仓库本身，而不获取任何 LFS 对象
$ GIT_LFS_SKIP_SMUDGE=1 https://github.com/git-lfs/git-lfs.git
# 或者
$ git -c filter.lfs.smudge= -c filter.lfs.required=false clone https://github.com/git-lfs/git-lfs.git

# 获取当前 commit 下包含的 LFS 对象的当前版本
$ git lfs pull
```


## 1.3. 参考链接

1. [github 链接](https://github.com/git-lfs/git-lfs)
2. [Git LFS 操作指南](https://zzz.buzz/zh/2016/04/19/the-guide-to-git-lfs/)
3. [使用 git LFS 管理 github 上的大型档案](http://cyruschiu.github.io/2016/09/26/using-git-lfs/)
