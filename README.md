# SSD222_Demo
UI demo on ssd222.

# SSD222_demo

<p style="text-indent:2em">SSD222_demo针对SSD2x2平台测试的UI demo。包含bringup目录和tool目录。bringup目录为未加密版本的的zkgui demo；tool目录为设置分别率的配置bin档，目前支持1024x600和800x480两种分辨率。</p>

## bringup：

### app
<p style="text-indent:2em">编译zkgui bin的source code目录。</p>

### zk_bootup
<p style="text-indent:2em">点屏demo。</p>

### zk_mini
<p style="text-indent:2em">精简版本。包含基本控件使用，播放音频文件，语音识别，网络，双屏拼接等。</p>

### zk_full
<p style="text-indent:2em">全功能版本。除zk_mini的全功能外，还包含模块测试，人脸检测。</p>

### zk_mini_nosensor
<p style="text-indent:2em">不带sensor case的精简版本。</p>

### 编译方法：
1. 使用FlyThings IDE导入工程；
2. 选中项目，点击右键选择清空项目，构建项目；
3. 导出生成文件到指定根目录，生成文件包含libzkgui.so和ui等。

## tool:
<p style="text-indent:2em">用来改变触屏分辨率配置的文件：</p>

```
  echo 1024x600.bin > /sys/bus/i2c/devices/0-005d/gtcfg
  echo 800x480.bin > /sys/bus/i2c/devices/0-005d/gtcfg
```
