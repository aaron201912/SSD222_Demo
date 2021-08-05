## 主要实现连接Usb Camera，在Panel显示Camera画面的功能。
* 支持输出格式为MJPEG，YUV422_YUYV，YUV420SP的Usb Camera。在Usb Camera同时支持这些格式的情况下，默认码流输出格式的优先级是：MJPEG > YUV422_YUYV > YUV420SP。可应用内修改此优先级。
* 支持Usb Camera设置推荐输出分辨率。若Usb Camera不支持推荐的分辨率，会自适应到当前设置码流格式下的最接近的分辨率。
* 支持输出显示的画面旋转
* 支持抓图功能，生成jpg图片。可以设置抓图张数，抓图间隔，抓图大小和图片保存路径。
***
## 编译方法
* 执行 make clean; make
***
## 运行
* 编译生成 usbCamera bin档，其依赖于jpeg库和libyuv。依赖lib路径为uvc/libjpeg/lib，uvc/libyuv/lib，将bin档和依赖库挂载或拷贝到板子上。
* 导入环境变量 export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./:/lib:/config/lib
* 执行 ./usbCamera
