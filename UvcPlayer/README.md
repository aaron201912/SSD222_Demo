# UvcPlayer demo 说明：

## 1. 主要功能：
   支持MJPEG摄像头的主要功能：
   a. 通过uvc协议从usb camera抓取mjpeg流；
   b. 出来一路720p分辨率的流给到libjpeg和libyuv之后转为为yuy2格式, yuy2格式给到divp做格式转换(nv12)和scaling up/down(缩放到屏的长宽), 然后divp给到disp显示到屏上.
   c. 如果需要使能旋转功能则增加vpe模块, 由于divp不支持同时缩放与旋转, 所以vpe模块实现scaling up/down, divp模块实现rotate.

## 2. 编译方法：
   a. 进入UvcPlayer/app文件夹
   b. 运行 make clean;make
   c. 生成 UvcPlayer 可执行文件

## 3. 运行方法；
   a. 插入usb摄像头, 确认板子生成/dev/video0后再运行UvcPlayer
   b. 将libjpeg/lib下的动态库,以及libyuv/lib下的动态库拷贝到板子/customer/lib下, 导出库 export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/customer/lib
   c. 将UvcPlayer拷贝到板子/customer目录下, 运行./UvcPlayer 1024 600 0

## 4. 注意事项
   a. 运行demo前必须先插上uvc摄像头, 确认板子生成/dev/video0后再运行demo. 如果没有生成video0节点, 需要在kernel中开启相关驱动.
   > Device Drivers
     > Multimedia support
       > Cameras/video grabbers support
       > Media USB Adapters
         > USB Video Class (UVC)
     > USB support
       > Support for Host-side USB

   b. 是否旋转通过运行命令控制
   ./UvcPlayer 1024 600 0   //不旋转图像
   ./UvcPlayer 1024 600 1   //旋转图像

   本demo也可播放本地jpg格式图片, 要求图片是yuv422编码.
   1. 是否使用UsbCam通过宏控制
      #define ENABLE_V4L2 //0: 本地图片播放, 1: 使能usb cam功能
   2. 将ENABLE_V4L2赋为0, 重新编译UvcPlayer
   3. 运行./UvcPlayer sigmastar.jpg 1024 600 [0/1]