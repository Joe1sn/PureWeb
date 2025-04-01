# PureWeb
PureWeb 是一个类似hexo、hugo的，简单的、微型的静态博客网站，使用c++20编写

使用效果：[joe1sn.eu.org/TestRepo/](https://joe1sn.eu.org/TestRepo/)

目前处于 **it just work** 状态

## 使用

将`speciou.css`、`github.css`、`config.json`、`PureWeb.exe`放在同一目录下

在二进制文件的同级目录编写`config.json`如下

```json
{
    "Deploy": {
        "branch": "main",
        "repo": "https://github.com/Joe1sn/TestRepo"
    },
    "Directory": {
        "archive_dir": "archives",
        "category_dir": "categories",
        "public_dir": "public",
        "source_dir": "source",
        "tag_dir": "tags"
    },
    "Pagination": {
        "pagination_dir": "page",
        "per_page": 20
    },
    "Site": {
        "author": "Joe1sn",
        "path": "",
        "subtitle": "",
        "title": "PureWeb Blog"
    }
}
```

- `branch`：git仓库分支
- `repo`：仓库地址
- `pagination_dir`：分页使用的标签
- `per_page`：每页文章数量
- `path`：服务器根目录，如`www.example.com/test`的参数为`test`，就是根的话保持为空

初始化：

```
.\PureWeb.exe init
```

然后再`post`文件夹下上传markdown文件，使用如下指令渲染

```
.\PureWeb.exe fresh
```

1. 在`website`文件夹下开启http服务器，然后访问
2. 再次`.\PureWeb.exe init`上传到远程仓库

# TODO

- [x] 解析json配置文件
- [x] 解析md文件
- [x] 文件夹下的md全部转为html
- [x] css美化
- [x] 生成各个页面主页index
- [x] 利用git刷新远程仓库
- [x] 【bug】cmark的code替换有问题
- [x] 移除 TestRepo
- [x] index分页
- [ ] 非法字符url编码【Half】
- [ ] git相关操作： init初始化时删除原有的 .git