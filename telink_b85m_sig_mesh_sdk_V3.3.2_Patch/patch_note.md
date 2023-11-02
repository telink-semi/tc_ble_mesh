## V3.3.2_Patch_0001

### Bug Fixes

* N/A

### Features

* Add crc16 for iv sector to make it more robust under abnormal voltage.
* Support ZBit flash.
* Improve the efficiency of ota when using ZBit flash.
* Calibrate the flash vref according to the reading value from flash.
* Add low voltage detection function: if low voltage is detected, the chip will enter sleep state. for more details, please refer to the codes of 'BATT_CHECK_ENABLE'.

### Performance Improvements

* N/A  

### BREAKING CHANGES

* Flash:Modify some Flash API usage for compitible.
* void flash_read_mid(unsigned char* mid) change to unsigned int flash_read_mid(void),the mid from 3byte change to 4byte.
* The API of flash_read_status、flash_write_status not provide to external use,you need use the API in the directory of flash depend on mid(eg:flash_write_status_midxxxxxx).
* The first argument of API int flash_read_mid_uid_with_check( unsigned int *flash_mid ,unsigned char *flash_uid),flash_mid need 4byte space.The second argument need 16byte,has two case,8byte or 16byte,if the flash only has 8byte uid,flash_uid[8:15] will be clear to zero.
* The API of flash_lock,flash_unlock will be instead of flash_lock_midxxxxxx and flash_unlock_midxxxxxx.

### Notes

* to avoid compilation errors or loss of functionality, please update all files when upgrading the SDK.

### Bug Fixes

* N/A

### Features

* IV扇区增加crc16校验，使其在异常电压下更加健壮。
* 支持ZBit flash。
* 根据校准值校准Flash电压。
* 当使用ZBit Flash时提升OTA效率。
* 增加低电压检测功能：如果检测到低电压，芯片进入休眠状态。具体请参考BATT_CHECK_ENABLE对应的代码。

### Performance Improvements

* N/A

### BREAKING CHANGES

* Flash:为兼容不同的Flash型号，Flash驱动结构做了调整，修改了部分Flash接口调用方式。
* void flash_read_mid(unsigned char* mid) 改为 unsigned int flash_read_mid(void),mid由3byte改为4byte,最高byte用于区分mid相同但是功能存在差异的flash。 *为兼容不同型号的Flash,flash_read_status、flash_write_status不提供给外部使用，需要使用对应接口时，需要根据mid去选择flash目录下的接口(例如：flash_write_status_midxxxxxx)。
* 接口int flash_read_mid_uid_with_check( unsigned int *flash_mid ,unsigned char *flash_uid)的第一个参数flash_mid需要4个字节空间，第二个参数需要16byte空间， 现有flash的uid有两种情况，一种16byte，一种8byte，如果是8byte，flash_uid[8:15]会被清零。
* 接口flash_lock、flash_unlock由flash_lock_midxxxxxx和flash_unlock_midxxxxxx替代。

### Notes

* 为避免编译错误以及功能丢失，升级SDK时，请确认更新全部SDK文件。


