# 一、下载编译脚本

 先去下载Openssl编译脚本： 

git clone https://github.com/x2on/OpenSSL-for-iPhone

# 二、执行编译

打开终端，cd到OpenSSL-for-iPhone目录，输入./build-libssl.sh，默认编译的是openssl 1.1.1c 过一会即可完成编译

假如编译指定版本

./build-libssl.sh --version=1.1.1a
还可以使用--help查看编译选项


# 三、使用

执行完后会生成两个文件夹：

include文件夹是头文件，lib是静态库，把这两个文件夹拖进项目
————————————————
版权声明：本文为CSDN博主「thehunters」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/thehunters/article/details/125661230