# ssplayer demo运行说明

1. cd demo
2. make clean;make
3. 将ffmpeg/lib下的库文件拷贝到板子/customer/lib, 导出路径export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/customer/lib
4. 将第二步编译生成的ssplayer文件拷到板子中, 运行./ssplayer hd_tcl.mp4

# ffmpeg library说明

1. 下载地址: https://github.com/aaron201912/ffmpeg.git
2. 编译方法:
   export PATH=/tools/toolchain/gcc-sigmastar-9.1.0-2020.07-x86_64_arm-linux-gnueabihf/bin/:$PATH //根据自己的编译环境指定9.1.0编译器即可
   cd ffmpeg/ffmpeg-4.1.3
   sh config_for_ssd222.sh
   make clean; make; make install
   生成的动态库文件在ffmpeg/ffmpeg-4.1.3/host/dynamic下 

注: 串口输入如下字符可控制播放状态.
's' : 退出播放
't' : 重新开始播放
'p' : 暂停播放
'c' : 取消暂停
'f' : 向前进10秒
'b' : 向后退10秒
'd' : 获取视频总时长
'g' : 获取当前播放位置
'u' : 静音/取消静音
'+' : 音量加
'-' : 音量减
'q' : 退出播放器进程

