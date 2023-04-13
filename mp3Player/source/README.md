### 这是一个演示解码播放MP3格式音频文件的demo

   1. 编译libmad

   $ cd mp3_player/libmad-0.15.1b
   $ ./build.sh
   在mp3_player/libmad/lib下会生成动态链接库，可修改build.sh指定toolchain的版本和路径。

   2. 编译Mp3Player

   $ cd mp3_player
   $ ./build.sh
   在当前目录会生成Mp3Player，并将生成的bin和lib拷贝到../runEnv/bin和../runEnv/lib目录下。
   可修改build.sh指定toolchain的版本和路径。

   3. 运行demo
   将runEnv目录下对应toolchain版本的Mp3Player和动态链接库拷贝到板子/customer/lib下，在板子中运行如下命令：
   $ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/customer/lib
   $ ./Mp3Player layout_test.mp3
