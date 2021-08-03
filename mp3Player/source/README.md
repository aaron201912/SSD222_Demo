### 这是一个演示解码播放MP3格式音频文件的demo

   1. 编译libmad

   $ cd mp3_player/libmad-0.15.1b
   $ ./build.sh
   在mp3_player/libmad/lib下会生成动态链接库

   2. 编译Mp3Player

   $ cd mp3_player
   $ make clean;make

   3. 运行demo
   将第一步生成的动态链接库拷贝到板子/customer/lib下，在板子中运行如下命令：
   $ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/customer/lib
   $ ./Mp3Player layout_test.mp3
