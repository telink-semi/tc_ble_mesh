#! /bin/bash
echo 启动脚本, 开始生成device证书...
# bin文件类型
bin_type=0x00


# intermediate pem 证书文件
intermediate_pem=./output-intermediate/intermediate.pem
if [ ! -f $intermediate_pem ] ; then
    echo error : intermediate.pem not exist
    exit
fi

# intermediate der 证书文件
intermediate_der=./output-intermediate/intermediate.der
if [ ! -f $intermediate_der ] ; then
    echo error : intermediate.der not exist
    exit
fi

# intermediate 私钥
intermediate_private_pem=./output-intermediate/intermediate-private.pem
if [ ! -f $intermediate_private_pem ] ; then
    echo error : intermediate-private.pem not exist
    exit
fi

#输出目录
# dev_random=$((RANDOM))
# output_dir=./output-device_$dev_random # 方便测试生成多个设备证书
output_dir=./output-device

#配置文件
config_file=./gen-device.config

#输出的私钥文件， DER格式
private_key_der_file=$output_dir/device-private.der
#输出的私钥文件， PEM格式
private_key_pem_file=$output_dir/device-private.pem

#输出的请求文件
csr_file=$output_dir/device-csr.csr

#输出的证书文件
der_file=$output_dir/device.der
pem_file=$output_dir/device.pem

#输出bin文件， 4K大小
bin_file=$output_dir/device.bin

# 清理output目录
rm -rf $output_dir
mkdir $output_dir


# 计算CRC16校验值
function crc16() {
    data=$1
    local crc=0xFFFF
    len=${#data}
    len=$((len / 2))
    for (( i=0; i<len; i++ )); do
        char=0x${data:i*2:1}${data:i*2+1:1}
        crc=$((crc ^ (0xFF & $char)))
        for (( j=0; j<8; j++ )); do
            if ((crc & 0x0001)); then
                crc=$((crc >> 1))
                crc=$((crc ^ 0xA001))
            else
                crc=$((crc >> 1))
            fi
        done
    done
    printf "%04X" $crc
}

# echo crc=$(crc16 "abcd") #1d97 - 7575
# exit

echo 生成device证书

# 创建私钥, 先创建DER格式， 再转换为pem格式， 方便在第三方工具上查看
openssl ecparam -name prime256v1 -genkey -out $private_key_der_file -outform DER
openssl ec -inform der -in $private_key_der_file -out $private_key_pem_file

# 生成证书请求文件
openssl req -new -sha256 -key $private_key_pem_file -out $csr_file -config $config_file

# 签署证书请求文件（root证书为自签名）
# openssl x509 -req -CA $intermediate_pem -CAkey $intermediate_private_pem -in $csr_file -days 3650 -CAcreateserial -extensions v3_req -extfile $config_file -out $der_file -outform DER 
openssl x509 -req -CA $intermediate_pem -CAkey $intermediate_private_pem -in $csr_file -days 3650 -extensions v3_req -extfile $config_file -out $der_file -outform DER 
openssl x509 -inform der -in $der_file -out $pem_file


echo 生成bin文件 start "---------"

#生成4kbin文件
printf '\xff%.0s' {1..4096} > $bin_file

#输入type
# echo -ne "\x$bin_type" 
echo "\x$bin_type" | xxd -r -p | dd conv=notrunc of=$bin_file bs=1 seek=0 status=none


#将device der长度信息写入到0位置， windows gitbash上不支持 seek 参数值为 hex
der_file_len=$(wc -c < $der_file)
lenHex=$(printf "%08X" "$der_file_len")
echo $lenHex | xxd -r -p | xxd -e | awk '{print $2}' | xxd -r -p | dd of=$bin_file seek=16 bs=1 conv=notrunc status=none #小端
#拷贝 device der
dd if=$der_file of=$bin_file seek=20 bs=1 conv=notrunc status=none

#拷贝intermediate der长度
der_file_len=$(wc -c < $intermediate_der)
lenHex=$(printf "%08X" "$der_file_len")
echo $lenHex | xxd -r -p | xxd -e | awk '{print $2}' | xxd -r -p | dd of=$bin_file seek=1792 bs=1 conv=notrunc status=none #小端

#拷贝 intermediate der （0x704）
dd if=$intermediate_der of=$bin_file seek=1796 bs=1 conv=notrunc status=none

#拷贝public key (0xe00)
openssl ec -text -noout -in $private_key_der_file | sed -n '7,11p' | cut -c 3- | xxd -r -p | dd of=$bin_file seek=3584 skip=1 bs=1 conv=notrunc status=none

#拷贝private key (0xe40)
openssl ec -text -noout -in $private_key_der_file | sed -n '3,5p' | xxd -r -p | dd of=$bin_file seek=3648 bs=1 conv=notrunc status=none

# 判断是否有CN开头
# cn_line_text=$(grep '^CN' gen-device.config)
cn_line_text=$(awk '/^CN/{print;exit}'  $config_file)
echo CN信息 $cn_line_text

if [ -z "$cn_line_text" ]; then
    echo config文件格式错误 $config_file ， 请检查是否有CN项
    exit
fi

device_uuid=${cn_line_text:3:36} #读取device uuid 信息
echo device_uuid=$device_uuid 

#拷贝device UUID (0xe60)
echo $device_uuid | xxd -r -p | dd of=$bin_file seek=3680 bs=1 conv=notrunc status=none

#计算CRC， 写到0F00
echo 计算crc...

crc_data=$(xxd -p -l 3840 $bin_file | tr -d '[:space:]')
# echo crc_data=$crc_data
crc=$(crc16 "$crc_data")

echo crc计算完成: $crc

# echo $crcHex | xxd -r -p | dd of=$bin_file seek=0x0F00 bs=1 conv=notrunc #大端
echo $crc | xxd -r -p | xxd -e | awk '{print $2}' | xxd -r -p | dd of=$bin_file seek=3840 bs=1 conv=notrunc status=none #小端

# wc -c $bin_file #文件长度

echo 生成bin文件 - end "---------"

echo 添加readme文件

readme_file=$output_dir/readme.txt

if test -f $readme_file ; then
    rm $readme_file
else
    echo "device-output" > $readme_file
fi

echo $private_key_der_file 和 $private_key_pem_file "=>" 私钥文件 >> $readme_file
echo $csr_file "=>" 请求文件 >> $readme_file
echo $der_file 和 $der_file "=>" 证书文件 >> $readme_file

echo "" >> $readme_file

echo "设备信息:" >> $readme_file

echo "deviceUUID:"$device_uuid >> $readme_file

# echo "private key:"$private >> $readme_file

echo 添加readme文件完成！

echo 脚本执行完成, 请查看$output_dir 目录