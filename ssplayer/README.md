# 目录说明

    1. demo: 应用main文件,编译出可执行档
    2. ffmpeg: 包含ffmpeg头文件以及动态链接库文件,源码参考: https://github.com/aaron201912/ffmpeg/tree/master/ffmpeg-4.1.3
    3. player: 含ffplayer头文件以及库文件,源码实现参考: https://github.com/aaron201912/ffmpeg/tree/master/ffplayer
    4. 3rdparty: 包含第三方依赖库文件, 如播放网络流时需要相关库支持
    5. hd_tcl.mp4: 视频流,SSD222平台使用软解码,限于CPU性能目前只能解到480P40

# 编译方法

    1. 将ssplayer文件夹拷贝到project同级目录, cd ssplayer/demo
    2. make clean;make //若更换了工程目录可使用: make ALKAID_PATH=/home/user/ssd222
    3. 将ffmpeg/lib, player/lib, 3rdparty/lib下的库文件拷贝到板子/customer/lib, 导出路径export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/customer/lib
    4. 将第二步编译生成的ssplayer文件拷到板子中, 运行./ssplayer hd_tcl.mp4

# 运行说明
    应用提供一些简单的操作命令, 串口输入如下字符可控制播放状态.
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
