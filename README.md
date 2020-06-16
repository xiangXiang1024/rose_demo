# 前端部分
这是数值计算程序自动化优化工具的前端部分，使用到了ROSE Compiler工具。

## 使用环境
Ubuntu 18.04

## 编译方法
- 在CMakeLists.txt中修改rose和boost所在路径
- 进入build目录，打开bash，切换至root用户
- 执行以下命令
```shell script
cmake ..
make
```

## 使用方法
以root用户运行analysis.sh，输入待优化程序所在路径。