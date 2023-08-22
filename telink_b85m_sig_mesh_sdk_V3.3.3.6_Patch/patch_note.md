## V3.3.3.6_Patch_0002

### Dependency Updates

* telink_b85m_ble_single_connection_sdk_v3.4.1
* mesh library commit log: 5d1b6666d7a518b3c93681cf1b896c63e2d931e6
* ble  library commit log: 0472400439802fec81f1dd0f0d11239b928e867f

### Bug Fixes

* fix the issue of soft PA in extend adv mode.

### Features

* N/A

### Performance Improvements

* N/A

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

* 修复扩展广播包模式下软件PA问题。

### Features

* N/A

### Performance Improvements

* N/A

### BREAKING CHANGES

* N/A

### Notes

* 为避免编译错误以及功能丢失，升级SDK时，请确认更新全部SDK文件。


## V3.3.3.6_Patch_0001

### Dependency Updates

* telink_b85m_ble_single_connection_sdk_v3.4.1
* mesh library commit log: 5d1b6666d7a518b3c93681cf1b896c63e2d931e6
* ble  library commit log: the same as mesh library.

### Bug Fixes

* fix the issue of LPN low probability iv update failure. when LPN is updated IV index for 50 times, there will be approximately 1 failure. when fail, LPN can be back to normal by powering off and restarting.

### Features

* N/A

### Performance Improvements

* N/A

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

* 修复 LPN 小概率 iv update 失败的问题。LPN更新 50次 IV，大约会出现 1次异常，出现异常时，LPN 断电重启能恢复正常。

### Features

* N/A

### Performance Improvements

* N/A

### BREAKING CHANGES

* N/A

### Notes

* 为避免编译错误以及功能丢失，升级SDK时，请确认更新全部SDK文件。



