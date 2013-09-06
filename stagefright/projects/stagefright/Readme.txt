测试：
1. 在eclipse中导入java工程，并把libH264Android.so拷贝到libs/armeabi目录下。
2. 把test目录下的测试文件predecode.h264使用Android命令拷贝到手机中 adb push predecode.h264 /mnt/sdcard
3. 运行测试程序，按手机的菜单键弹出选项菜单，选择播放。

添加其他解码方式：
1. 将其他解码方式的对外暴露函数的头文件添加到test-jin.cpp中，然后在函数映射列表中添加自定义的对应函数，将解码方法封装到jni函数中。

2. 在Java端导入声明对应的native方法，并调用。