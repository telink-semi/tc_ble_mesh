#! /bin/bash
echo 启动脚本, 开始生成intermediate证书...

root_pem=./output-root/root.pem
root_private_pem=./output-root/root-private.pem

if [ ! -f $root_pem ] ; then
    echo error : root.pem not exist
    exit
fi


if [ ! -f $root_private_pem ] ; then
    echo error : root-private.pem not exist
    exit
fi


#输出目录
output_dir=./output-intermediate

#配置文件
config_file=./gen-intermediate.config

#私钥文件， DER格式
private_key_der_file=$output_dir/intermediate-private.der
#私钥文件， PEM格式
private_key_pem_file=$output_dir/intermediate-private.pem

#请求文件
csr_file=$output_dir/intermediate-csr.csr

#证书文件
der_file=$output_dir/intermediate.der
pem_file=$output_dir/intermediate.pem

# 清理output目录
rm -rf $output_dir
mkdir $output_dir


echo "生成intermediate证书"

# 创建私钥, 先创建DER格式， 再转换为pem格式， 方便在第三方工具上查看
openssl ecparam -name prime256v1 -genkey -out $private_key_der_file -outform DER
openssl ec -inform der -in $private_key_der_file -out $private_key_pem_file

# 生成证书请求文件
openssl req -new -sha256 -key $private_key_pem_file -out $csr_file -config $config_file

# 签署证书请求文件（root证书为自签名）
openssl x509 -req -CA $root_pem -CAkey $root_private_pem -in $csr_file  -days 3650 -extensions v3_req -extfile $config_file -out $der_file -outform DER 

openssl x509 -inform der -in $der_file -out $pem_file


echo 添加readme文件...

readme_file=$output_dir/readme.txt

if test -f $readme_file ; then
    rm $readme_file
else
    echo "intermediate-output" > $readme_file
fi

echo $private_key_der_file 和 $private_key_pem_file "=>" 私钥文件 >> $readme_file
echo $csr_file "=>" 请求文件 >> $readme_file
echo $der_file 和 $der_file "=>" 证书文件 >> $readme_file

echo 添加readme文件完成！

echo 脚本执行完成, 请查看$output_dir 目录