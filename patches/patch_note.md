## V3.3.1_Patch_0002

### Dependency Updates

* ble library log:SHA-1: 5c84f6ef2bd29a69cae45b94b8e1347e2e503e8b 

### Bug Fixes

* fix segment rx when multi group segment message appear.

### Features

* add extend adv filter callback function mesh_blc_aux_adv_filter().

### Performance Improvements

* N/A

### BREAKING CHANGES

* N/A

### Notes

* N/A


### Dependency Updates

* ble library log:SHA-1: 5c84f6ef2bd29a69cae45b94b8e1347e2e503e8b    

### Bug Fixes

* 修复多个节点同时发送目的地址为非单播的分包消息时，接收端大概率接收不到任何分包消息的问题.

### Features

* 添加扩展广播包过滤回调函数mesh_blc_aux_adv_filter().

### Performance Improvements

* N/A

### BREAKING CHANGES

* N/A

### Notes

* N/A


## V3.3.1_Patch_0001

### Dependency Updates

* ble library log:e90b226981817271f5d18b1e616334da31d048d6.  

### Bug Fixes

* Raw data maybe abnormal when switch scan channel in receiving adv, add software crc double check adv before report.

### Features

* N/A

### Performance Improvements

* N/A

### BREAKING CHANGES

* N/A

### Notes

* N/A


### Dependency Updates

* ble library log:e90b226981817271f5d18b1e616334da31d048d6.   

### Bug Fixes

* 正在收包时切换扫描通道可能导致raw data异常，在上报前用软件crc再次检验。

### Features

* N/A

### Performance Improvements

* N/A

### BREAKING CHANGES

* N/A

### Notes

* N/A

