# lvgl-watch  
### 暂停开发
Open-Smartwatch手表第三方固件、源代码，基于LVGL8.0  
![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/1452206376/lvgl-watch)  
需要在GPIO2上接一个振动电机  
如果需要红外遥控功能，则需要在GPIO4上接一个红外发射LED，接法：3V3->LED->GPIO4  
视频链接：[Bilibili](https://www.bilibili.com/video/BV1fq4y1K7t7)  

本项目为PlatformIO项目，不能用Arduino编译，但是用的是Arduino/C++语言。编译时可以使用VSCode PlatformIO扩展或命令行下的PlatformIO编译器编译  
## 直接烧录编译好的bin文件
不推荐这种方法，如果想获取最新内容，请直接编译master分支  
如果没有编译器，也可以使用release里的文件烧录。（我没有测试这种方法，不确定可用）
把它们放在同一个文件夹下，然后输入指令如下（需要安装esptool: `sudo apt install esptool`）：  
`esptool --baud 921600 write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 ./bootloader_dio_40m.bin 0x8000 ./partitions.bin 0xe000 ./boot_app0.bin 0x10000 ./firmware.bin`
