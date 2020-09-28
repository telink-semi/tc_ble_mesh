# TelinkSigMeshLibExtensions

version: v3.2.3

## 集成扩展库TelinkSigMeshLibExtensions步骤

1. 参考Demo做法，将SDK源码文件夹`TelinkSigMeshLib`(路径为app/ios/TelinkSigMeshLib/TelinkSigMeshLib)和扩展功能文件夹`Extensions`(需要从Telink的FAE手上申请获取该文件夹)拷贝到与APP文件夹`SigMeshOCDemo`同一级的目录。
2. 打开meshOTA和remote provision功能编译宏。步骤为：打开代码文件`TelinkSigMeshLib.h`(路径为TelinkSigMeshLib/TelinkSigMeshLib/TelinkSigMeshLib.h)，取消注释第47行的注释代码`//#define kExist`。
3. 拉取库源码工程`TelinkSigMeshLib.xcodeproj`到客户自己的APP中，在客户自己的APP中添加TelinkSigMeshLibExtensions库的二进制库链接。步骤为：TelinkSigMeshLibExtensions->Build Phases->Link Binary With Libraries->点击“+”->选中“TelinkSigMeshLibExtensions.framework”->点击“Add”即可完成集成。

SigMesh iOS SDK of Telink Semiconductor (Shanghai) Co., Ltd.
