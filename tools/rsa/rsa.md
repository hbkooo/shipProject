<!-- rsa加密使用说明 -->

一.公钥和私钥的生成:
1.-- 生成 RSA 私钥
openssl genrsa -out rsa_private_key.pem 1024
2.-- 生成 RSA 公钥
openssl rsa -in rsa_private_key.pem -pubout -out rsa_public_key.pem

二.文件加密
1.需要加密的文件:auth.json
    格式:
    {
        "AuthParams" : {
        "mac_address" : "2C:FD:A1:5C:B0:B1",
        "auth_date" : "20191212"
        }
    }

2.加密需要:(1)RSA 私钥 (2)auth.json 放在可执行文件目录下
3.执行./rsa,加密成功后会生成 license-key.lic 文件
4.把license-key.lic提供给客户