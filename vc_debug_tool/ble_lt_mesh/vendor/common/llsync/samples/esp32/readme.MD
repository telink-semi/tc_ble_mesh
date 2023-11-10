## 简介

本工程是基于esp-idf环境的LLSync MESH SDK移植示例。

## 快速开始

运行该示例前，请确保你已经阅读了一下文档，并对BLE MESH有了初步了解。

- [LLSync mesh SDK移植说明](../../docs/LLSync_Mesh_SDK移植说明.md)
- [LLSync Mesh接入规范](../../docs/LLSync_Mesh接入规范.md)

### 编译环境安装

> 注意：所有的操作均在Linux环境下进行

本工程基于esp-idf v4.4版本开发，`commit 8153bfe4125e6a608abccf1561fd10285016c90a`，如果使用其他版本，可能会编译报错，请自行解决。

```shell
git clone -b v4.4 --recursive https://github.com/espressif/esp-idf.git esp-idf-v4.4
cd esp-idf-v4.4/
```

### patch补丁

由于本SDK需要使用static OOB来和平台校验设备合法性，故对nimble mesh的源码做了一定修改。

```shell
# 进入patch目录 执行 ./patch.sh
# ./patch.sh 
idf path:/home/esp/esp-idf
patching...
patch success.
```

如上，说明patch执行成功。可以正常编译本SDK。

```shell
# 如果需要恢复 则执行 ./patch.sh clean 
# ./patch.sh clean
idf path:/home/esp/esp-idf
clean ....
clean done.
```

### 新建设备

在[控制台](https://console.cloud.tencent.com/iotexplorer)新建项目-->新建产品-->新建设备，取得设备的三元组信息。

### 添加设备三元组

使用设备的三元组替换`./samples/esp32/main/hal/ble_qiot_import.c`中宏定义的的三元组信息。

### 编译

在`./samples/esp32`目录下，执行 `idf.py build`编译该项目

```shell
#idf.py build
...
Creating esp32 image...
Merged 25 ELF sections
Successfully created esp32 image.
Generated /mnt/hgfs/workspace/ble_mesh/tencentcloud-iot-explorer-BLE-MESH-sdk-embedded/samples/esp32/build/blemesh.bin
[7/7] cd /mnt/hgfs/workspace/ble_mesh/tencentcloud-iot-explorer-BLE-MESH-sdk-embedded/samples/esp32/build/esp-idf/esptool_py && /root/...._table/partition-table.bin /mnt/hgfs/workspace/ble_mesh/tencentcloud-iot-explorer-BLE-MESH-sdk-embedded/samples/esp32/build/blemesh.bin
blemesh.bin binary size 0x95960 bytes. Smallest app partition is 0x100000 bytes. 0x6a6a0 bytes (42%) free.

Project build complete. To flash, run this command:
/root/.espressif/python_env/idf4.4_py3.8_env/bin/python ../../../../../../../home/esp/esp-idf/components/esptool_py/esptool/esptool.py -p (PORT) -b 460800 --before default_reset --after hard_reset --chip esp32  write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x1000 build/bootloader/bootloader.bin 0x10000 build/partition_table/partition-table.bin 0x20000 build/blemesh.bin
or run 'idf.py -p (PORT) flash'
```

### 运行

使用`esp tool`或者`idf.py flash`烧录固件到`esp32`开发板，复位运行该示例。

### 运行log分析

#### 设备绑定

结合PB-GATT入网流程分析

![ed4f736037451e1d.png](https://i.bmp.ovh/imgs/2022/07/13/ed4f736037451e1d.png)

```shell
# provisioner(连连小程序)连接成功
I (75669) NimBLE: proxy_connected: conn_handle 0

I (76529) NimBLE: proxy_complete_pdu: Mesh Provisioning PDU
# 小程序发送入网申请 attention time : 5秒
W (76529) NimBLE: bt_mesh_pb_gatt_recv: recv <-- 2 bytes: 0005
# 设备端回复同意入网。使用Static OOB方式入网
W (76529) NimBLE: proxy_segment_and_send: send --> conn_handle 0 type 0x03 len 12: 010100010001000000000000
# Provisioning Start OOB Publish Key is used
W (76619) NimBLE: bt_mesh_pb_gatt_recv: recv <-- 6 bytes: 020000010000
# 设备端计算本地authvalue
ble qiot dump: random, length: 16
 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
===============================================
25 BE 9C F6 AB 54 6C DF 28 58 8C 98 62 19 38 C6 | %....Tl.(X..b.8.


ble qiot dump: hmac before, length: 54
 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
===============================================
32 35 62 65 39 63 66 36 61 62 35 34 36 63 64 66 | 25be9cf6ab546cdf
32 38 35 38 38 63 39 38 36 32 31 39 33 38 63 36 | 28588c98621938c6
31 31 48 38 50 51 44 5A 48 58 37 38 65 33 36 64 | 11H8PQDZHX78e36d
30 62 34 39 31 61                               | 0b491a


ble qiot dump: authvalue, length: 16
 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
===============================================
6C 54 B4 8F 87 1A AA 39 91 2E 8C FB B8 13 3E FC | lT.....9......>.

E (76689) NimBLE: prov_auth: sizeof(link.auth) : 16

E (76699) NimBLE: prov_auth: link.auth: 6c54b48f871aaa39912e8cfbb8133efc

I (76979) NimBLE: proxy_complete_pdu: Mesh Provisioning PDU
# 交换公钥
W (76979) NimBLE: bt_mesh_pb_gatt_recv: recv <-- 65 bytes: 03236ee9430e4c3312d73196734541b23ecd1f982887136c9413004160484e79127ce316706be5751cb6474c4cf8ec7c570e42249d0e3e788625b5bc3f40e0fd5a
# 交换公钥
W (77249) NimBLE: proxy_segment_and_send: send --> conn_handle 0 type 0x03 len 65: 03ff8c28f96945120c2849e60ba3ba9a1ecdf32215eac501dcc928b1c72da2b06db51387a05740d75d344464bb0d1aebc98228a97c4832a123bc90996b02976429

I (78179) NimBLE: proxy_complete_pdu: Mesh Provisioning PDU
# 确认
W (78179) NimBLE: bt_mesh_pb_gatt_recv: recv <-- 17 bytes: 05b7673b110dad6f95dad773cb2e4a32de

LocalRandom: 25be9cf6ab546cdf28588c98621938c6
W (78189) NimBLE: proxy_segment_and_send: send --> conn_handle 0 type 0x03 len 17: 0581e1fb98072e42fc2347653d57effd5d

I (78239) NimBLE: proxy_complete_pdu: Mesh Provisioning PDU
# 收到连连报文校验哈希值
W (78239) NimBLE: bt_mesh_pb_gatt_recv: recv <-- 17 bytes: 06502d96528ecd292a588619788a01f6eb

# 计算连连authvalue
ble qiot dump: random, length: 16
 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
===============================================
50 2D 96 52 8E CD 29 2A 58 86 19 78 8A 01 F6 EB | P-.R..)*X..x....


ble qiot dump: hmac before, length: 54
 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
===============================================
35 30 32 64 39 36 35 32 38 65 63 64 32 39 32 61 | 502d96528ecd292a
35 38 38 36 31 39 37 38 38 61 30 31 66 36 65 62 | 588619788a01f6eb
31 31 48 38 50 51 44 5A 48 58 37 38 65 33 36 64 | 11H8PQDZHX78e36d
30 62 34 39 31 61                               | 0b491a


ble qiot dump: authvalue, length: 16
 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
===============================================
E4 1E 19 0B CE E1 3C AC 07 12 CA 1B 51 B7 94 D6 | ......<.....Q...

E (78319) NimBLE: prov_random: link.auth: e41e190bcee13cac0712ca1b51b794d6
# 发送设备端校验哈希值
W (78319) NimBLE: proxy_segment_and_send: send --> conn_handle 0 type 0x03 len 17: 0625be9cf6ab546cdf28588c98621938c6

I (78779) NimBLE: proxy_complete_pdu: Mesh Provisioning PDU
# 校验通过 收到连连MESH通行证
W (78779) NimBLE: bt_mesh_pb_gatt_recv: recv <-- 34 bytes: 074ca912640cb5d97e67a0d72e8709495ae037eb40ad58434530ed91afe883b54b7a
# 配网完成
W (78789) NimBLE: proxy_segment_and_send: send --> conn_handle 0 type 0x03 len 1: 08

```



#### 设备控制

```shell
qiot info: sg_mesh_handle.recv_tid_num:0, tid:1
qiot info: Att type is:0xf000
qiot info: find id is:0
qiot info: Mesh set data is OnOff:0

qiot info: sg_mesh_handle.recv_tid_num:1, tid:2
qiot info: Att type is:0xf000
qiot info: find id is:0
qiot info: Mesh set data is OnOff:1

```





