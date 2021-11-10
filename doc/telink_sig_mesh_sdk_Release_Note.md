## V3.3.4

### Bug Fixes

* N/A

### Features

* (Firmware) add customers's customization functions: low-power and non-low-power version of mesh, OTA function.
* (Firmware) add the functions of retention sleep, soft timer function and iv update process for mesh_switch project. Set destination address of "group control" by configuring the publish address of client model.
* (Firmware) add the function of private fast provision for gateway. set FAST_PROVISION_ENABLE to 1.
* (Firmware) add sdk codes of sig_mesh_tool.exe for debugging or secondary development.
* (Android/iOS) add the identification and configuration for mesh_switch device.
* (Android/iOS) add the function of private fast provision for LPN nodes.
* (Android/iOS) add configuration UI to configure device's TTL, etc.
* (iOS) add FIFO for commands, allow the application layer to push several commands at the same time.

### Performance Improvements

* (Firmware) modify the maximum number of device's elements from 8 to 16.

### BREAKING CHANGES

* N/A

### Notes

* to avoid compilation errors or loss of functionality, please update all files when upgrading the SDK.

### Bug Fixes

* N/A

### Features

* (Firmware) 增加客户定制的低功耗和非低功耗版本的mesh，以及定制的OTA功能。
* (Firmware) 增加支持mesh_switch 工程的 retention sleep模式，soft timer功能，iv update流程。可以通过配置client model的publish address 设置"组控按键"对应的目标地址。
* (Firmware) 增加gateway支持私有的fast provision功能，设置FAST_PROVISION_ENABLE为1即可。
* (Firmware) 增加sig_mesh_tool.exe工具对应的sdk代码，可用于debug或者二次开发的。
* (Android/iOS) 增加mesh_switch设备的识别和配置。
* (Android/iOS) 增加LPN节点支持私有的 fast provision入网。
* (Android/iOS) 增加节点配置界面，用于配置节点的TTL等。
* (iOS) 增加串行指令队列，允许应用层同时压入多条指令。

### Performance Improvements

* (Firmware) 更改一个节点最大支持的element 个数，由8个改为16个。

### BREAKING CHANGES

* N/A

### Notes

* 为避免编译错误以及功能丢失，升级SDK时，请确认更新全部SDK文件。



## V3.3.3
### Features
* Support ZBit flash.
* Improve the efficiency of ota when using ZBit flash. 
* Calibrate the flash vref according to the reading value from flash. 
* add low voltage detection function: if low voltage is detected, the chip will enter sleep state. for more details, please refer to the codes of 'BATT_CHECK_ENABLE'.

### BUG FIXS
  - N/A

### BREAKING CHANGES
* Flash: Modify some Flash API usage for compitible.
* void flash_read_mid(unsigned char* mid) change to unsigned int flash_read_mid(void),the mid from 3byte change to 4byte.
* The API of flash_read_status、flash_write_status not provide to external use,you need use the API in the directory of flash depend on mid(eg:flash_write_status_midxxxxxx).
* The first argument of API int flash_read_mid_uid_with_check( unsigned int *flash_mid ,unsigned char *flash_uid),flash_mid need 4byte space.The second argument need 16byte,has two case,8byte or 16byte,if the flash only has 8byte uid,flash_uid[8:15] will be clear to zero.
* The API of flash_lock,flash_unlock will be instead of flash_lock_midxxxxxx and flash_unlock_midxxxxxx.



### Features
* 支持ZBit flash。
* 根据校准值校准Flash电压。
* 当使用ZBit Flash时提升OTA效率。
* 增加低电压检测功能：如果检测到低电压，芯片进入休眠状态。具体请参考BATT_CHECK_ENABLE对应的代码。

### Bug fixs
* N/A

### BREAKING CHANGES
* Flash:为兼容不同的Flash型号，Flash驱动结构做了调整，修改了部分Flash接口调用方式。
* void flash_read_mid(unsigned char* mid) 改为 unsigned int flash_read_mid(void),mid由3byte改为4byte,最高byte用于区分mid相同但是功能存在差异的flash。
* 为兼容不同型号的Flash,flash_read_status、flash_write_status不提供给外部使用，需要使用对应接口时，需要根据mid去选择flash目录下的接口(例如：flash_write_status_midxxxxxx)。
* 接口int flash_read_mid_uid_with_check( unsigned int *flash_mid ,unsigned char *flash_uid)的第一个参数flash_mid需要4个字节空间，第二个参数需要16byte空间，
现有flash的uid有两种情况，一种16byte，一种8byte，如果是8byte，flash_uid[8:15]会被清零。
* 接口flash_lock、flash_unlock由flash_lock_midxxxxxx和flash_unlock_midxxxxxx替代。



## V3.3.2
### Bug Fixes

* (iOS APP) fix bug: When the IVI of the app is different from that of device, decryption will fail because IVI of the device was not be used. all customers need to update to the iOS version.
* (iOS APP) fix bug: fail to disable log_saving function.

### Features	

* (Firmware) update flash driver(synchronized with "825x_BLE_SDK_3_4_1 patch 001"). must be updated if the functions of getting flash UID or flah protection were used.
* (Firmware) add extend_ADV mode for 8258 LPN. current SDK support extend_ADV mode for 8258 mesh, 8258 gateway and 8258 LPN.
* (Android APP) support multiple network keys and application keys.
* (iOS APP) add API of 'defaultPublishPeriodModel' to set default publish parameters.

### Performance Improvements

* (Android APP) change provisioner's unicast address range from 0x0100 to 0x0400.

### BREAKING CHANGES

* Flash(B85)
  * Modify some Flash API usage for compitible:
  * void flash_read_mid(unsigned char\* mid) change to unsigned int flash_read_mid(void),the mid from 3byte change to 4byte.
  * The API of flash_read_status、flash_write_status not provide to external use,you need use the API in the directory of flash depend on mid(eg:flash_write_status_midxxxxxx).
  * The first argument of API int flash_read_mid_uid_with_check( unsigned int \*flash_mid ,unsigned char \*flash_uid),flash_mid need 4byte space.The second argument need 16byte,has two case,8byte or 16byte,if the flash only has 8byte uid,flash_uid\[8:15\] will be clear to zero.
  * The API of flash_lock,flash_unlock,flash_read_otp,flash_write_otp,flash_erase_otp,flash_lock_otp will be instead of flash_lock_midxxxxxx,flash_unlock_midxxxxxx,flash_read_otp_midxxxxxx,flash_write_otp_midxxxxxx,flash_erase_otp_midxxxxxx,flash_lock_otp_midxxxxxx.
  * delete the useless API.

### Notes

* to avoid compilation errors or loss of functionality, please update all files when upgrading the SDK.


### Bug Fixes

* (iOS APP) fix bug: 修复bug：当app的ivi和节点的不一致时，SDK未根据设备端返回的ivi进行解密导致解密失败。所有客户都需要升级iOS版本。
* (iOS APP) fix bug: SDK的日志保存功能无法关闭。

### Features

* (Firmware) 更新 flash driver 同步到825x_BLE_SDK_3_4_1 patch 001。如客户需要读取flash UID 或者 使用保护功能，请务必更新。
* (Firmware) 增加支持 8258 LPN 的扩展广播包功能。当前SDK支持 8258 mesh，8258 gateway和8258 LPN的扩展广播包功能。
* (Android APP) 增加支持多个network key 和 application key的应用。
* (iOS APP) 增加用于配置 默认publish参数的API: defaultPublishPeriodModel。

### Performance Improvements

* (Android APP) 修改provisioner的每一个组网地址段的范围，从 0x0100 改为 0x0400。

### BREAKING CHANGES

* Flash(B85)
  * 为兼容不同的Flash型号，Flash驱动结构做了调整，修改了部分Flash接口调用方式：
  * void flash_read_mid(unsigned char\* mid) 改为 unsigned int flash_read_mid(void),mid由3byte改为4byte,最高byte用于区分mid相同但是功能存在差异的flash。
  * 为兼容不同型号的Flash,flash_read_status、flash_write_status不提供给外部使用，需要使用对应接口时，需要根据mid去选择flash目录下的接口(例如：flash_write_status_midxxxxxx)。
  * 接口int flash_read_mid_uid_with_check( unsigned int \*flash_mid ,unsigned char \*flash_uid)的第一个参数flash_mid需要4个字节空间，第二个参数需要16byte空间，现有flash的uid有两种情况，一种16byte，一种8byte，如果是8byte，flash_uid\[8:15\]会被清零。
  * 接口flash_lock、flash_unlock、flash_read_otp、flash_write_otp、flash_erase_otp、flash_lock_otp由flash_lock_midxxxxxx、flash_unlock_midxxxxxx、flash_read_otp_midxxxxxx、flash_write_otp_midxxxxxx、flash_erase_otp_midxxxxxx、flash_lock_otp_midxxxxxx替代。
  * 删除不使用的接口。

### Notes

* 为避免编译错误以及功能丢失，升级SDK时，请确认更新全部SDK文件。

## V3.3.1
### Bug Fixes

* N/A

### Features	

* (Firmware) add the default device name in customized OTA mode. if it was used, the device name of new firmware will not be checked by the node when OTA.

### Performance Improvements

* (Firmware) improve the receiving success rate of extend adv mode. only "liblt_8258_mesh_extend_adv.a" need to be updated.

### BREAKING CHANGES

* N/A

### Notes

* to avoid compilation errors or loss of functionality, please update all files when upgrading the SDK.


### Bug Fixes

* N/A

### Features

* (Firmware) 增加客户定制OTA模式下的默认device name。当节点使用此name时，节点将不检查 new firmware 的device name。

### Performance Improvements

* (Firmware) 改善 extend ADV mode 的收包成功率。仅更新liblt_8258_mesh_extend_adv.a 即可。

### BREAKING CHANGES

* N/A

### Notes

* 为避免编译错误以及功能丢失，升级SDK时，请确认更新全部SDK文件。

## V3.3.0
### Bug Fixes

* (iOS APP) fix bug: encryption and decryption fail if AID of AppKey is 0.
* (iOS APP) fix bug: when many segment messages are received continuously and the ack messages are continuous response, the APP will crash.

### Features	

* (Firmware/Android/iOS) add 825x DLE(Data Length Extend) function.
* (Firmware) add 825x extend ADV mode for mesh OTA. time cost of OTA is about 4 minutes. disabled by default.
* (Firmware) support to modify the max number of subscription address by SUB_LIST_MAX.
* (Firmware) add long sleep mode (more than 1 hour) by the function of "cpu_long_sleep_wakeup".
* (Firmware) add 8258_mesh_monitor project.
* (Firmware) add customized protocol of Provision and OTA.
* (Firmware) add customized process of IV update procedure.
* (Android APP) add function: only the nodes which selected by user can be provisioned.

### Performance Improvements

* (Firmware) improve the sleep processing flow of mesh_LPN project by using soft timer.
* (Firmware) package customer's source codes into library.
* (Android APP) improve the color setting UI in HSL mode.

### BREAKING CHANGES

* N/A

### Notes

* to avoid compilation errors or loss of functionality, please update all files when upgrading the SDK.


### Bug Fixes

* (iOS APP) 修复AppKey的AID为0时mesh数据包加解密异常的bug。
* (iOS APP) 修复bug：当连续收到多个segment message，执行连续回复多个SigSegmentAcknowledgmentMessage时，会发生APP异常退出的问题。

### Features

* (Firmware/Android/iOS) 增加825x DLE(Data Length Extend)功能。
* (Firmware) 增加825x 扩展广播包模式的mesh OTA，升级时间为4分钟左右。默认关闭。
* (Firmware) 支持通过修改SUB_LIST_MAX来设置支持的最大分组数量。
* (Firmware) 增加长休眠接口函数(超过1小时)：cpu_long_sleep_wakeup
* (Firmware) 增加8258_mesh_monitor工程。
* (Firmware) 添加客户定制的入网以及OTA协议。
* (Firmware) 添加客户定制的IV update流程的特殊处理。
* (Android APP) 增加支持只对用户选择的节点进行配网的功能。

### Performance Improvements

* (Firmware) 改善mesh_LPN工程的睡眠处理流程，改为使用soft timer来实现。
* (Firmware) 封装客户相关的source code成library。
* (Android APP) 改善HSL模式下的颜色选择UI界面。

### BREAKING CHANGES

* N/A

### Notes

* 为避免编译错误以及功能丢失，升级SDK时，请确认更新全部SDK文件。




## V3.2.3
### Bug Fixes

* (Firmware) fix bug: node can't send mesh commands during GATT OTA.
* (IOS APP) fix bug: app can't judge that have read the att_service table completely, if there is no characteristic value in the last att_service.
* (IOS APP) fix bug: only report “responseMax” packets to application layer when the number of packets received is bigger than "responseMax".

### Features	

* (Firmware) gateway can send adv packets and can be connected by master. enable PROVISIONER_GATT_ADV_EN for this function, disabled by default.
* (Firmware) the number of ota nodes is expanded to 200 in gateway project, which is defined by MESH_OTA_UPDATE_NODE_MAX.
* (Firmware) add UART mode for 8269 master dongle project. Master dongle firmware need set HCI_ACCESS to HCI_USE_UART, and sig_mesh_tool.exe need select the "UART" button to enable this function. The default is USB mode.
* (android APP) add API of connecting specified node and reporting the connection status to app.
* (android APP) add the function of displaying and comparing the PID value before OTA.
* (IOS APP) add the function of adding new NetKey and AppKey, and app can use the new key to send commands.

### Performance Improvements

* (Firmware) Improve compatibility by removing the association between the transmit interval and the connection interval, when the node is in GATT connected state.
* (Firmware) Improve HSL transition algorithm.

### BREAKING CHANGES

* N/A

### Notes

* to avoid compilation errors or loss of functionality, please update all files when upgrading the SDK.


### Bug Fixes

* (Firmware) 修复节点在进行 GATT OTA 的时候，不能发送 mesh 命令的问题。
* (IOS APP) 修复如果设备最后一个服务内没有特征值时，无法判断出读取服务列表完毕的问题。
* (IOS APP) 修复当命令的实际回包数量大于"responseMax"时，只上报了responseMax个回包到应用层的问题。

### Features

* (Firmware) 增加gateway节点发送可连接广播包和被master连接的功能。把PROVISIONER_GATT_ADV_EN置为1可开启这个功能，默认为0.
* (Firmware) gateway 进行mesh OTA的节点数增加到200个节点, 具体数量定义是MESH_OTA_UPDATE_NODE_MAX。
* (Firmware) 增加8269 master dongle的串口通讯模式。 master dongle firmware配置HCI_ACCESS为HCI_USE_UART，上位机选择 "UART"按钮即可。默认为USB模式。
* (android APP) 增加API接口，用于连接指定节点，并且把连接状态上报给app;
* (android APP) 增加在OTA前显示和判断PID的功能;
* (IOS APP) 增加添加新的NetKey和AppKey的功能，并能使用新添加的key发送指令。

### Performance Improvements

* (Firmware) 取消直连节点的发包间隔和连接间隔的关联，提高兼容性。
* (Firmware) 改善HSL渐变算法。

### BREAKING CHANGES

* N/A

### Notes

* 为避免编译错误以及功能丢失，升级SDK时，请确认更新全部SDK文件。



## V3.2.2
### Bug Fixes

* (android APP) fix bug: provision failed(timeout) if static-oob data is not found in a static-oob device side. 
* (android APP) fix bug: app key binding failed if the target vendor model does not support app key encryption. 

### Features

* update 8278 driver for A2 chip.
* add GATT low power mode, need to enable macro of GATT_LPN_EN, disabled by default.
* add “user_app_config.h” for user to modify some default settings. It can be use to separate user settings and application codes from SDK.
* (iOS APP) support static oob provisioning and static oob database importing.

### Performance Improvements

* (VC tool, APP) update storage format of json file.

### BREAKING CHANGES

* N/A

### Note

* to avoid compilation errors or loss of functionality, please update all files when upgrading the SDK.


## V3.2.1
### Features
* (firmware) update 8278 drivers for A1.
* (android APP) add function of static_oob provisioning.

### BREAKING CHANGES

* N/A


## V3.2.0
### Bug Fixes
* fix some error characters in UART log in sometimes.
* (android APP) fix issue of multi hop command:change transition time from none to default when sending command.

### Features
* add beacon functions(iBeacon&Eddystone), disabled by default.
* enable SUBSCRIPTION_SHARE_EN as default to share group setting between the models which are in bind state.
* LPN：set LED state 'on' by default. Once power up, send level status message to display in UI. Long press key SW1 to trigger factory reset function.
* add a new macro FEATURE_FRIEND_EN,user can set FEATURE_FRIEND_EN to 0 to disable friend feature to reduce RAM and code size.
* improve user experience by letting nodes which are provisioned can be shown in GATT scan window of sig_mesh_tool.exe.
* (android APP) switch from c-lib version to java-source version.
* (android APP) add QR code to share network by cloud.
* (IOS APP) cut down the size of OpenSSL file.
* (IOS APP) add nodeIdentifySet in GATT OTA.
* (IOS APP) add sourceCode library TelinkSigMeshLib, update doc of sourceCode library TelinkSigMeshLib.

### Performance Improvements
* (android APP) optimize success rate of remote-provision
* (IOS APP) optimize success rate of remote-provision

### BREAKING CHANGES

* N/A


## V3.1.6
### Bug Fixes
* fix ttl bug:change command's ttl from 1 to 10 in android app.
### Features
* for MESH_IRONMAN_MENLO_ENABLE configuration.
    - update flash map to allow new bootloader and dual-mode image updates.
    - update mesh firmware's CLOCK from 16MHz to 32MHz.
    - add tx power calibration for constant output power.
* update bootloader firmware's CLOCK from 24MHz to 48MHz.
* add IO defines for 825x 32pin dongle.

### BREAKING CHANGES

* N/A


## V3.1.5
### Bug Fixes		
* 8269 mesh project Spirit mode(MESH_SPIRIT_ENABLE): fix compile error.	
					
### Features		
* spirit lpn project use soft timer.
* Add demo: light control model cooperate with sensor model. Code refer to SENSOR_LIGHTING_CTRL_EN.
* Set TTL default value from 5 to 10.
* Eclipse compile in multy thread mode.

### BREAKING CHANGES

* N/A


## V3.1.4
### Features
* add mesh+zb_ble dual mode
### Bug Fixes
* fw:Fixed getting vid error when enable mi/genie dual mode
* app:Fixed duplicate symbol '_crc16' issue

### BREAKING CHANGES

* N/A


## V3.1.0
### Features
* add project to support both Tmall genius and Xiaomi XiaoAI. Set MESH_USER_DEFINE_MODE to MESH_MI_SPIRIT_ENABLE. Note: must use Chip with 1M flash
* INI command：if TID value is not 0, it will be used when send command. if TID is 0, it will auto be increased by stack.
* 1M flash: add no pingpong OTA fuction.Please refer to PINGPONG_OTA_DISABLE.
* Add the model of the light mode, switch ,switch with battery for the mi mode .
* support 8278.
* Homekit and sig mesh dual mode project can self define flash map
* Lpn reserve more retention area for user.
* add dimmer ligtht/naught wire switch/live wire switch
* ios:
* 1.update lib.
* 2.Set publish of time model when app keyBind device success, period.steps are 20 seconds, time_auth is 1.
* 3.App will call statusNowTime when SDK callback setFilter success.
* 4.Optimize setFilter api.
* 5.Add SigMeshOC.framework automatic packaging script "Script.sh".
* 6.SigMeshOCDemo integrates SigMeshOC.framework by default, developer also can integrates sourceCode project "SigMeshOC.xcodeproj".
* 7.Sno will be stored locally on the mobile phone from this version, not in JSON. The local address of the provisioner is not necessarily the minimum address of the allocatedUnicastRange.
* android:
* 1. LPN node control: add on/off function;
* 3. add time publish if node contains time model when scanning and send time status when connect success;
* 4. update lib  

### BREAKING CHANGES

* N/A




