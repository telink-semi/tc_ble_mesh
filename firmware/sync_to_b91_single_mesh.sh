#record sdk version
SDK_VERSION=0.2
echo "Telink BLE SDK_VERSION = $SDK_VERSION" > ./sdk_version.txt

cp -rf proj_lib/mesh_crypto				../ble_lt_multimode_eagle/proj_lib/
cp -rf proj_lib/sig_mesh				../ble_lt_multimode_eagle/proj_lib/
cp -rf vendor/common					../ble_lt_multimode_eagle/vendor
cp -rf vendor/user_app					../ble_lt_multimode_eagle/vendor
cp -rf config.h							../ble_lt_multimode_eagle/

rm -rf ../ble_lt_multimode_eagle/vendor/common/ble_ll_ota.c

pause
::exit