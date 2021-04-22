使用说明

1、在goaheadNew目录下执行make编译goAhead，编译结果是goaheadNew\build\linux-arm-default\bin\libgoahead.a；

2、目前是在arm运行，如果需要改为pc下运行，需要修改goaheadNew\Makefile中的toolChain，在18、19行；

3、把libgoahead.a复制到work目录下，编译最终运行的bin，并且要实现work\include\setConfig.h里定义的回调函数，在work\httpserver.cpp里
有实现读取登录账号密码的get_Password回调函数指针的示例；

4、httpserver.cpp中init()函数是用来注册goAhead相关功能函数用的，除非有功能函数要修改，否则固定不变；

5、参考httpserver.cpp中的void* HttpServerMain(void *argv)函数的流程，

首先要设置相关功能的回调函数，如“get_Password = gett_Password;”；
其次是设置主页路径：setWebServerConfig("www", "html/login.asp");
最后是把goAhead启动起来：work( (void*) init );


6、使用的时候，把libgoahead.a库引进后就不用管goAhead的源码了；

7、运行的时候，需要把goaheadNew\work\data目录下的route.txt、 self.crt 、 self.key文件跟bin文件放在同一个目录下，否则goAhead会运行不了！

8、需要修改goAhead的http监听端口的话，可以尝试修改http.c文件的619行socketParseAddress(endpoint, &ip, &port, &secure, 80)的监听端口号，修改后再重写编译libgoahead.a并替换到工程中；

9、网页文件存放在goaheadNew\work\data\www下。

10、目前是编译成so库，也可以自行修改编译成bin