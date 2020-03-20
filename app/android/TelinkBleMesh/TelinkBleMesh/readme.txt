目前状态： remote provision app端重发
动作切换时停止其他的动作

1. 在keyBind过程中有小概率连接断开（timeout）
2. remote provision 三个节点测试，

包含的文档：
1. app操作手册

2. app开发手册
    准备: lib导入
        准备事件回调处理 1, 可以参考demo中的application类
        创建Application类， 继承自MeshApplication， 并实现其中的抽象(abstract)方法;
        如果不方便继承， 则需要参考MeshApplication类添加广播监听和操作EventBus的事件分发;


    组网流程: provisioning & binding
    消息发送: send mesh message
    消息接收: StatusMessage
    Gatt-OTA: StartGattOTA
    Mesh-OTA: StartMeshOTA
    Remote-Provisioning (RP)
    Event说明:
    Mesh分享: 目前mesh分享功能是通过导出和导入Json文件来实现，
        其中导出是将本地的网络信息输出为Json格式
        导入功能是将解析选择的Json文件， 并导入至本地
        该功能的实现代码在Demo source code中， 可以参考 MeshStorageService 类的 #importExternal 和 #exportMeshToJson
    调试:
        LOG
        异常检查
            1. provision 失败:
    其它: file selector
3. readme.txt
    更新日志



// todo
fast provision

sync 1227
添加 remote provision, 在直连节点测试时可以正常provision且bind， 在非直连节点测试时会失败；


import
1. TelinkMeshApplication and MeshService
2. start service
3. MeshService#setupMeshNetwork


version info:
config model dst address is primary

common model dst address is target element address

V3.0.J.2
todo
1. remote provision
2. fast provision
3. remote mesh service

V3.0.J.1
1. Add iv update flow:
    1.1 Send mesh beacon when sequence number >= THRESHOLD_SEQUENCE_NUMBER(0xC00000) if Gatt connected
    1.2 Update local iv index when received mesh secure beacon (ivUpdating==false), and the received value is larger than local value;
        If updated, clear sequence storage map, and set local sequence to 0



V3.0.J
init version


