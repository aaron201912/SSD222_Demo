## Build:  
#### 1. build libgoahead.a
```
cd goahead_test
make clean;make
```
#### 在goahead_test/build/linux-arm-default/bin目录下生成libgoahead.a

#### 2. build httpserver bin  
```
cp build/linux-arm-default/bin/libgoahead.a work
cd work
make clean;make
```
#### 在work/bin目录下生成httpserver，同时在goahead_test/out目录下生成webdemo的运行环境  

## Run:  
#### 将goahead_test/out目录下的内容拷贝或挂载到目标板上。  
#### 执行下列指令开启webserver服务：  
```
./goahead -v --home 配置文件路径 网页文件路径 绑定IP:监听端口
```
#### 例如：  
```
./httpserver -v --home /etc /mnt/zkEnv/2021_verify/P3_webdemo/www 172.19.24.211:80 &
```
#### 配置文件路径为/etc,网页路径为/mnt/zkEnv/2021_verify/P3_webdemo/www 板子ip为172.19.24.211，使用80端口。  

#### 在pc端打开浏览器，输入http://172.19.24.211/tmp.asp 即可访问该网页。
#### 注意：浏览器需要关闭代理服务器。
