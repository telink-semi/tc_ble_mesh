## V4.1.0.1

### Version

* SDK Version: tc_ble_mesh_V4.1.0.1
* Chip Version: TLSR825X / TLSR827X
* telink_b85m_ble_single_connection_sdk_v3.4.1
* mesh library commit log: df928dc56708e0e930684130aae789d12b85d4e3
* ble  library commit log: the same as mesh library.

### Bug Fixes

* (Firmware)delete __clzsi2() in mesh library which already exists in library of soft-fp.a to avoid reporting redefine error in some cases when compile sdk.
* (Firmware)fix the issue that receiver node may miss some segment messages when multiple nodes simultaneously send segment messages and destination address are all group address.

### Known issues

* N/A

### Features

* (Firmware) add flash protection function. set APP_FLASH_PROTECTION_ENABLE to 1 to enable. enabled by default.
  - need to click "Unlock" button in the Telink BDT tool first to unprotect flash, then to burn firmware or erase flash during the development and debugging phase.
* (Firmware)add extend ADV filter callback function mesh_blc_aux_adv_filter().
* (Firmware/Android/iOS)add examples of gpio and ZSIR sensor types for NLC(Network Lighting Control) occupancy sensor. set NLC_SENSOR_Type_SEL to NLCP_TYPE_OCS, and set NLC_SENSOR_SEL to SENSOR_ZSIR1000 or SENSOR_OCS_GPIO.
* (Firmware/Android/iOS)add ZSIR sensor example for NLC(Network Lighting Control) Ambient Light Sensor. set NLC_SENSOR_Type_SEL to NLCP_TYPE_ALS and set NLC_SENSOR_SEL to SENSOR_ZSIR1000.
* (Android/iOS)support configuring a node with multiple scene models inside.
* (Android/iOS)support naming nodes, and sorting nodes by name or node address.
* (Android/iOS)support batch selection and configuration of direct forwarding parameters for multiple nodes.
* (Android/iOS)support functions of On-Demand Private GATT Proxy and Solicitation PDU RPL Configuration.
* (Android/iOS)updated the cloud server IP address for network sharing functionality. And support setting the IP address.

### Performance Improvements

* (Firmware)optimize the gateway project to reduce firmware size by 6.5 kBytes and RAM by 2.0 kBytes.
* (Firmware/Android/iOS)optimize NLC (Network Lighting Control) related code.

### BREAKING CHANGES

* (Firmware)if you are using V4.1.0.0 SDK, and using a gateway project or setting MESH_USER_DEFINE_MODE to MESH_SPIRIT_ENABLE, please make sure to upgrade to V4.1.0.1 SDK, otherwise the node's provisioned state will be abnormally changed from provision state to unprovision state when the firmware before V4.1.0.0 upgrade to V4.1.0.0 or V4.1.0.0 upgrade to a newer version.

* (Firmware)for 512k flash, due to enable flash protection and the fact that the minimum protection size for several flash is 256k(can be found by searching lock_block_e on the SDK), it is necessary to move sectors with frequent write operations to after 0x70000, including FLASH_ADR_RESET_CNT，FLASH_ADR_MISC and FLASH_ADR_SW_LEVEL . After modifying the flash sector address, when the new firmware OTA to nodes which sdk version is V4.1.0.0 or earlier, the MCU will automatically migrate the three sectors. The migration code please refer to FLASH_MAP_AUTO_EXCHANGE_SOME_SECTORS_EN. For 512k flash, it is enabled by default. If the customer has previously modified a 512k flash map, these macro need to be defined: FLASH_ADR_MISC_LEGACY_VERSION，FLASH_ADR_SW_LEVEL_LEGACY_VERSION，FLASH_ADR_RESET_CNT_LEGACY_VERSION and FLASH_ADR_MD_VD_LIGHT_LEGACY_VERSION. If there are assert errors when compile sdk, please contact us. If the flash map has not been modified, there will be no assert error.If it is a new product development, because it is no need to consider compatibity with previous versions, then FLASH_MAP_AUTO_EXCHANGE_SOME_SECTORS_EN can be disabled to save firmware size.
   * Conclusion:
   * If upgrading an legacy project to the current version
    - currently using 512K byte flash and have not changed the SDK flash map. Please ignore the above information.
    - Currently using 1M byte flash, Please ignore the above information.
   * If it is a new product development, ignore the above information and you can disabled FLASH_MAP_AUTO_EXCHANGE_SOME_SECTORS_EN and  FLASH_MAP_AUTO_MOVE_SW_LEVEL_SECTOR_TO_NEW_ADDR_EN to save firmware size.

### Notes

* to avoid compilation errors or loss of functionality, please update all files when upgrading the SDK.

### Flash

* TLSR825X: GD25LD40C, GD25LD40E, GD25LD80C, GD25LD80E, ZB25WD40B, ZB25WD80B
* TLSR827X: GD25LD40C, GD25LD40E, GD25LD80C, GD25LD80E, ZB25WD40B, ZB25WD80B, P25Q80U

### CodeSize

* Flash and RAM (default target):
  - 8258_mesh:_________Flash 125.4 KB, RAM (27.7 KB + 3K stack),
  - 8258_mesh_LPN:____Flash 118.9 KB, RAM (21.7 KB + 3K stack),
  - 8258_mesh_gw:_____Flash 125.0 KB, RAM (30.5 KB + 3K stack),
  - 8258_mesh_switch:__Flash 114.1 KB, RAM (24.9 KB + 3K stack),
  
  - 8278_mesh:_________Flash 123.4 KB, RAM (27.5 KB + 3K stack),
  - 8278_mesh_LPN:____Flash 117.0 KB, RAM (23.3 KB + 3K stack),
  - 8278_mesh_gw:_____Flash 123.0 KB, RAM (31.0 KB + 3K stack),
  - 8278_mesh_switch:__Flash 110.7 KB, RAM (25.0 KB + 3K stack),


### Version

* SDK Version: tc_ble_mesh_V4.1.0.1
* Chip Version: TLSR825X / TLSR827X
* telink_b85m_ble_single_connection_sdk_v3.4.1
* mesh library commit log: df928dc56708e0e930684130aae789d12b85d4e3
* ble  library commit log: the same as mesh library.

### Bug Fixes

* (Firmware)删除mesh库文件中的__clzsi2()函数，避免有些情况下会报重定义，因为在浮点库soft-fp.a里面也有定义。
* (Firmware)修复多个节点同时发送，且目的地址是组播的segment长包消息时，接收端有概率不能接收到全部的分包消息的问题。

### Known issues

* N/A

### Features

* (Firmware)增加flash 保护功能，设置 APP_FLASH_PROTECTION_ENABLE 等于 1来使能该功能，默认使能。
  - 在开发和调试阶段，用户应先点击Telink BDT工具中的“Unlock”命令来解锁flash，然后再执行烧录固件或者擦除Flash。
* (Firmware)添加扩展广播包过滤回调函数mesh_blc_aux_adv_filter()。
* (Firmware/Android/iOS)为 NLC occupancy sensor添加 gpio 类型和 ZSIR sensor 类型的sample。修改 NLC_SENSOR_TYPE_SEL 为 NLCP_TYPE_OCS，以及 NLC_SENSOR_SEL 修改为 SENSOR_ZSIR1000 或者 SENSOR_OCS_GPIO 即可。
* (Firmware/Android/iOS)添加 ZSIR sensor 示例给NLC的光线感应sensor产品。修改 NLC_SENSOR_TYPE_SEL 为 NLCP_TYPE_ALS，以及 NLC_SENSOR_SEL 修改 为 SENSOR_ZSIR1000即可。
* (Android/iOS)支持配置节点内有多个场景model的设备。
* (Android/iOS)支持给节点命名以及通过名称或者地址排序来显示设备。
* (Android/iOS)支持批量配置多个节点的direct forwarding的参数。
* (Android/iOS)支持 On-Demand Private GATT Proxy 和 Solicitation PDU RPL Configuration 功能。
* (Android/iOS)更新了网络分享功能的云服务器IP地址。并支持修改IP地址。

### Performance Improvements

* (Firmware)优化gateway工程，减少 固件大小 6.5 kBytes，RAM 2.0 kBytes。
* (Firmware/Android/iOS)优化 NLC(Network Lighting Control) 相关的代码。

### BREAKING CHANGES

* (Firmware)如果正在使用 V4.1.0.0，并且使用了 gateway 工程 或者 MESH_USER_DEFINE_MODE 设置为 MESH_SPIRIT_ENABLE , 请务必更新到 V4.1.0.1，否则会出现旧版本升级到 V4.1.0.0(或者 V4.1.0.0 升级到更新版本) 后，节点的的已组网状态会被异常地变为未组网状态。

* (Firmware)对于512k flash，由于使能了flash 保护，并且有几个flash的最小保护区间是256k(可在sdk中搜索 lock_block_e 查询得到)，所以需要把执行写操作相对频繁的扇区修改到0x70000之后，包括FLASH_ADR_RESET_CNT，FLASH_ADR_MISC和FLASH_ADR_SW_LEVEL。修改扇区地址之后，当新的固件OTA到V4.1.0.0及之前的节点时，MCU会识别并自动迁移刚才提到的3个扇区，对应的宏开关是 FLASH_MAP_AUTO_EXCHANGE_SOME_SECTORS_EN，默认使能。如果客户之前有修改过 512k 的flash map默认定义的地址，则需要新增这几个宏定义：FLASH_ADR_MISC_LEGACY_VERSION，FLASH_ADR_SW_LEVEL_LEGACY_VERSION，FLASH_ADR_RESET_CNT_LEGACY_VERSION和FLASH_ADR_MD_VD_LIGHT_LEGACY_VERSION，定义后，编译时，如果有断言错误，请联系我们。如果没修改过 flash map，则编译时不会出现断言错误。如果是新产品开发，不需要兼容以前的版本，则可以关闭FLASH_MAP_AUTO_EXCHANGE_SOME_SECTORS_EN，以节省固件大小。
   * 结论：
   * 如果是旧项目升级到当前版本
    - 当前用的是512K flash且没有改过sdk自定义的flash map，忽略上述信息。
    - 当前用的是1M flash，忽略上述信息。
   * 如果是新产品开发，忽略上述信息，并可以关闭FLASH_MAP_AUTO_EXCHANGE_SOME_SECTORS_EN 和 FLASH_MAP_AUTO_MOVE_SW_LEVEL_SECTOR_TO_NEW_ADDR_EN，以节省固件大小。

### Notes

* 为避免编译错误以及功能丢失，升级SDK时，请确认更新全部SDK文件。



## V4.1.0.0

### Dependency Updates

* telink_b85m_ble_single_connection_sdk_v3.4.1
* mesh library commit log: 0a68e761eb91c310c1e79e6b4ad7976037e8946a
* ble  library commit log: the same as mesh library.

### Bug Fixes

* (Firmware)fix the issue of soft PA in extend adv mode.

### Features

* (Firmware/Android/iOS)support all features of mesh V1.1, mainly include:
  - enhance provision auth: set PROV_EPA_EN to 1 to enable. enable as default.
  - remote provision: provision the unprovision devices within one-hop or multi-hops. set MD_REMOTE_PROV to 1 to enable. disable as default.
  - device firmware update: Upgrade the firmware of multiple nodes simultaneously through mesh. set MD_MESH_OTA_EN to 1 to enable. disable as default.
  - directed forwarding: set MD_DF_CFG_SERVER_EN to 1 to enable. disable as default.
  - certificate-base provisioning: set CERTIFY_BASE_ENABLE to 1 to enable. disable as default.
  - private beacons: set MD_PRIVACY_BEA and PRIVATE_PROXY_FUN_EN to 1 to enable. disable as default.
  - subnet bridge: set MD_SBR_CFG_SERVER_EN to 1 to enable. disable as default.
  - Opcodes Aggregator: aggregate multiple messages into a single message to send. set MD_OP_AGG_EN to 1 to enable. disable as default.
  - NLC profiles: Network Lighting Control profiles. set one of NLCP_BLC_EN, NLCP_DIC_EN, NLCP_BSS_EN, NLCP_TYPE_ALS, NLCP_TYPE_OCS and NLCP_TYPE_ENM to 1 to enable the corresponding NLC profile. disable as default.
* (Firmware)add support the SMP function. set BLE_REMOTE_SECURITY_ENABLE to 1 to enable. disable as default.
* (Firmware)add support GATT 2M PHY. set BLE_GATT_2M_PHY_ENABLE to 1 to enable. disable as default.
* (Android/iOS)Add support multi-network. users can create, delete, switch networks.
* (Android/iOS)Add the network subpage to display network information and related control functions.

### Performance Improvements

* (Firmware)change the MCU default clock of mesh project from 16MHz to 32MHz.
* (Android)change the network data store format from serialize to objectBox.
* (Android/iOS)optimize the setting page to display different configuration items by category.

### BREAKING CHANGES

* (Firmware/Android/iOS)the opcode and model ID of mesh 1.1 new features, such as mesh OTA's, are changed compared with the version of the draft feature. Please pay attention to compatibility issues if you develop base on draft feature version before.

### Notes

* to avoid compilation errors or loss of functionality, please update all files when upgrading the SDK.

### CodeSize

* Flash and RAM (default target):
  - 8258_mesh:_________Flash 123.6 KB, RAM (27.7 KB + 3K stack),
  - 8258_mesh_LPN:____Flash 117.1 KB, RAM (21.6 KB + 3K stack),
  - 8258_mesh_gw:_____Flash 130.6 KB, RAM (32.5 KB + 3K stack),
  - 8258_mesh_switch:__Flash 110.8 KB, RAM (23.5 KB + 3K stack),


### Dependency Updates

* telink_b85m_ble_single_connection_sdk_v3.4.1
* mesh library commit log: 0a68e761eb91c310c1e79e6b4ad7976037e8946a
* ble  library commit log: the same as mesh library.

### Bug Fixes

* (Firmware)修复扩展广播包模式下软件PA问题。

### Features

* (Firmware/Android/iOS)支持 mesh V1.1 的所有功能，主要包含：
  - enhance provision auth：组网认证增强，设置 PROV_EPA_EN 等于 1来使能该功能，默认打开。
  - remote provision：把距离provisioner一跳及多跳的未配网设备都能添加到网络中。设置 MD_REMOTE_PROV 等于 1来使能该功能，默认关闭。
  - device firmware update：通过mesh方式对多个节点同时进行固件升级，设置 MD_MESH_OTA_EN 等于 1来使能该功能，默认关闭。
  - directed forwarding：路由功能，设置 MD_DF_CFG_SERVER_EN 等于 1来使能该功能，默认关闭。
  - certificate-base provisioning：基于证书认证的组网模式，设置 CERTIFY_BASE_ENABLE 等于 1来使能该功能，默认关闭。
  - private beacons：私有信标，设置 MD_PRIVACY_BEA 和 PRIVATE_PROXY_FUN_EN 等于 1来使能该功能，默认关闭。
  - subnet bridge：子网桥接，设置 MD_SBR_CFG_SERVER_EN 等于 1来使能该功能，默认关闭。
  - Opcodes Aggregator：多个消息组合成一条消息的功能，设置 MD_OP_AGG_EN 等于 1来使能该功能，默认关闭。
  - NLC profiles: Network Lighting Control profiles, 分别设置 NLCP_BLC_EN, NLCP_DIC_EN, NLCP_BSS_EN, NLCP_TYPE_ALS, NLCP_TYPE_OCS, NLCP_TYPE_ENM 等于 1来使能对应功能，默认关闭。
* (Firmware)增加 SMP 功能，设置 BLE_REMOTE_SECURITY_ENABLE等于 1来使能该功能，默认关闭。
* (Firmware)增加 GATT 2M PHY功能，设置 BLE_GATT_2M_PHY_ENABLE等于 1来使能该功能，默认关闭。
* (Android/iOS)添加多网络支持， 用户可以创建、删除、切换网络。
* (Android/iOS)添加network子页面， 用于显示网络信息及相关控制功能。

### Performance Improvements

* (Firmware)更改mesh 工程的 MCU 默认时钟，由 16MHz 改为 32MHz。
* (Android)更改网络数据存储格式，由serialize改为objectBox。
* (Android/iOS)优化setting页面显示，分类显示不同的配置项。

### BREAKING CHANGES

* (Firmware/Android/iOS)mesh OTA 等功能 的 opcode 和 model ID 的值，相对于 draft feature的版本有更新，如果之前有基于draft feature的版本进行开发，请注意兼容性问题。

### Notes

* 为避免编译错误以及功能丢失，升级SDK时，请确认更新全部SDK文件。



## V3.3.3.6

### Dependency Updates

* telink_b85m_ble_single_connection_sdk_v3.4.1
* mesh library commit log: 5d1b6666d7a518b3c93681cf1b896c63e2d931e6
* ble  library commit log: the same as mesh library.

### Bug Fixes

* N/A

### Features

* N/A

### Performance Improvements

* delete some unused APIs which name has typo.

### BREAKING CHANGES

* N/A

### Notes

* to avoid compilation errors or loss of functionality, please update all files when upgrading the SDK.

### CodeSize

* Flash and RAM (default target):
  - 8258_mesh:_________Flash 118 KB, RAM (28 KB + 3K stack),
  - 8258_mesh_LPN:____Flash 111 KB, RAM (23 KB + 3K stack),
  - 8258_mesh_gw:_____Flash 128 KB, RAM (36 KB + 3K stack),
  - 8258_mesh_switch:__Flash 106 KB, RAM (25 KB + 3K stack),


### Dependency Updates

* telink_b85m_ble_single_connection_sdk_v3.4.1
* mesh library commit log: 5d1b6666d7a518b3c93681cf1b896c63e2d931e6
* ble  library commit log: the same as mesh library.

### Bug Fixes

* N/A

### Features

* N/A

### Performance Improvements

* 删除一些未正确拼写名称且未使用的接口。

### BREAKING CHANGES

* N/A

### Notes

* 为避免编译错误以及功能丢失，升级SDK时，请确认更新全部SDK文件。



## V3.3.3.5.1

### Bug Fixes

* N/A

### Features

* (Firmware) update the "firmware" and "release_bin" folders only. The others remain the same as V3.3.3.5.
* (Firmware) add support Tencent connectivity. set MESH_ USER_ DEFINE_ MODE to MESH_ LLSYNC_ ENABLE. for more details, refer to "doc/AN_2211xxxx_C1_Tencent Lianliant_elink Development Document_20221125. docx".

### Performance Improvements

* N/A

### BREAKING CHANGES

* N/A

### Notes

* to avoid compilation errors or loss of functionality, please update all files when upgrading the SDK.

* Flash and RAM (default target):

  - 8258_mesh.bin:        Flash 118 KB, RAM (27.8 KB + 3K stack),
  - 8258_mesh_llsync.bin: Flash 124 KB, RAM (28.6 KB + 3K stack),


### Bug Fixes

* N/A

### Features

* (Firmware) 仅更新 “firmware” 和 "release_bin" 文件夹，其它保持和 V3.3.3.5 一样。
* (Firmware) 增加支持接入腾讯连连功能。MESH_USER_DEFINE_MODE 设置为 MESH_LLSYNC_ENABLE 即可。详情请参阅 “doc/AN_2211xxxx_C1_腾讯连连_telink开发文档_20221125.docx”。

### Performance Improvements

* N/A

### BREAKING CHANGES

* N/A

### Notes

* 为避免编译错误以及功能丢失，升级SDK时，请确认更新全部SDK文件。



## V3.3.3.5

### Bug Fixes

* (Firmware) fix bug of V3.3.4: if TIMER 0-TIMER 2 was set to be enabled (disable by default), TIMER 0-TIMER 2 will be disabled by mistake after the node is provisioned.

### Features

* (Firmware) add accepting OTA rules: upgrade is allowed only when the PID is the same, see OTA_ADOPT_RULE_CHECK_PID_EN, enabled by default; upgrade is allowed when the VID (Version ID) is increased, see OTA_ADOPT_RULE_CHECK_VID_EN, disabled by default. (This OTA rule has been added in V3.3.4)
* (Firmware) replace Telink BSD license with Apache 2.0 License.
* (Firmware) add customized low-power mesh products that can send and receive messages, see LPN_CONTROL_EN for details.
* (Firmware) add customized protocol of Provision.
* (Firmware) add support extend ADV mode for gateway.

### Performance Improvements

* (Firmware/Android/iOS) modify Version ID format, from two-digit ASCII code to four-digit BCD code, such as 3 3 3 5, which still be two bytes, see FW_VERSION_TELINK_RELEASE for details.

### BREAKING CHANGES

* N/A

### Notes

* to avoid compilation errors or loss of functionality, please update all files when upgrading the SDK.

* Flash and RAM (default target):

  - 8258_mesh:_________Flash 118 KB, RAM (28 KB + 3K stack),
  - 8258_mesh_LPN:____Flash 111 KB, RAM (23 KB + 3K stack),
  - 8258_mesh_gw:_____Flash 128 KB, RAM (36 KB + 3K stack),
  - 8258_mesh_switch:__Flash 106 KB, RAM (25 KB + 3K stack),


### Bug Fixes

* (Firmware) 修复V3.3.4版本问题：如果客户使能了TIMER0-TIMER2(默认不使能)，节点被组网后，TIMER0-TIMER2会被错误关闭的问题。

### Features

* (Firmware) 增加是否接受升级的OTA规则：PID相同时才允许升级，详见OTA_ADOPT_RULE_CHECK_PID_EN，默认使能；VID(Version ID)增加时才允许升级，详见OTA_ADOPT_RULE_CHECK_VID_EN，默认不使能。(该OTA规则在V3.3.4中已添加)
* (Firmware) 替换 Telink BSD license为 Apache 2.0 license。
* (Firmware) 增加客户定制的能收发命令的低功耗mesh产品，详见 LPN_CONTROL_EN。
* (Firmware) 增加客户定制的配网协议。
* (Firmware) 增加gateway支持收发extend ADV的模式。

### Performance Improvements

* (Firmware/Android/iOS) 修改两个byte的版本号格式，由原来的两个数字的ASCII码值改为4个数字的BCD码，如 3 3 3 5，仍然是占用两个byte，详见FW_VERSION_TELINK_RELEASE的定义。

### BREAKING CHANGES

* N/A

### Notes

* 为避免编译错误以及功能丢失，升级SDK时，请确认更新全部SDK文件。



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




