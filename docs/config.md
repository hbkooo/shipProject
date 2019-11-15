# 配置说明

|选项|参数类型|说明|
|:--|:--|:--|
|apiKey|string|必需的唯一标识|
|listenip|ipv4||
|port|int||
|httpThreads|int|处理http请求|
|caffeThreads|int|处理需要使用caffe的请求|
|maxRequests|int|每秒最大请求数，超出返回908|
|modelPath|string|模型所在路径|
|GPUDevices|int|表示GPU设备数，配置`1`则在指定`GPUId`运行，`>1`会将caffe线程分配到不同GPU|
|switch|int|数据采集功能开关。0时关闭;1时仅保存数据库;2时保存数据库并上传|
|serverIP|string|数据接收服务器的地址|
|serverPort|string|数据接收服务器的端口|
|uploadTimepoint|string|数据上传时间点，每天定时上传。时间点为'0:0:0'时持续上传|
|logConfigParams|object|日志级别开关配置|
|isDecryptModel|bool|模型加解密开关|

```json
{
	"caffeParams": {
		"GPUDevices": 1,
		"GPUId": 0,
		"caffeThreads": 4,
		"isDecryptModel": false,
		"modelPath": "model/"
	},
	"faceDataParams": {
		"switch": 0,
		"serverURL": "127.0.0.1:8989",
		"uploadTimepoint": "3:0:0"
	},
	"httpParams": {
		"apiKey": "",
		"httpThreads": 2,
		"listenIP": "0.0.0.0",
		"maxRequests": 20,
		"port": 33388
	},
	"logConfigParams": {
		"debugEnabled": false,
		"errorEnabled": true,
		"fatalEnabled": false,
		"infoEnabled": true,
		"traceEnabled": false,
		"verboseEnabled": false,
		"warningEnabled": false
	},
	"searchToolParams": {
		"distance": 250,
		"redis_db_id": 5,
		"redis_passwd": "",
		"searchThreshold": 0.75,
		"simThreshold": 0.66,
		"topK": 80
	}
}
```
