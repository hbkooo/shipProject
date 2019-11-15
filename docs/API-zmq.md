<!-- TOC -->

- [0 常用错误代码](#0-常用错误代码)
- [1 注册/修改人脸](#1-注册修改人脸)
    - [1.1 描述](#11-描述)
    - [1.2 请求](#12-请求)
    - [1.3 返回](#13-返回)
    - [1.4 返回示例](#14-返回示例)
- [2 1:N搜索](#2-1n搜索)
    - [2.1 描述](#21-描述)
    - [2.2 请求](#22-请求)
    - [2.3 返回](#23-返回)
    - [2.4 返回示例](#24-返回示例)
- [3 删除人脸](#3-删除人脸)
    - [3.1 描述](#31-描述)
    - [3.2 请求](#32-请求)
    - [3.3 返回](#33-返回)
    - [3.4 返回示例](#34-返回示例)
- [4 查询人脸](#4-查询人脸)
    - [4.1 描述](#41-描述)
    - [4.2 请求](#42-请求)
    - [4.3 返回](#43-返回)
    - [4.4 返回示例](#44-返回示例)
- [5 1:1搜索](#5-11搜索)
    - [5.1 描述](#51-描述)
    - [5.2 请求](#52-请求)
    - [5.3 返回](#53-返回)
    - [5.4 返回示例](#54-返回示例)
- [6 人脸检测](#6-人脸检测)
    - [6.1 描述](#61-描述)
    - [6.2 请求](#62-请求)
    - [6.3 返回](#63-返回)
    - [6.4 返回示例](#64-返回示例)
- [7 特征提取](#7-特征提取)
    - [7.1 描述](#71-描述)
    - [7.2 请求](#72-请求)
    - [7.3 请求示例](#73-请求示例)
    - [7.4 返回](#74-返回)
    - [7.5 返回示例](#75-返回示例)
- [8 性别预测](#8-性别预测)
    - [8.1 描述](#81-描述)
    - [8.2 请求](#82-请求)
    - [8.3 返回](#83-返回)
    - [8.4 返回示例](#84-返回示例)
- [9 年龄预测](#9-年龄预测)
    - [9.1 描述](#91-描述)
    - [9.2 请求](#92-请求)
    - [9.3 返回](#93-返回)
    - [9.4 返回示例](#94-返回示例)
- [10 活体检测](#10-活体检测)
    - [10.1 描述](#101-描述)
    - [10.2 请求](#102-请求)
    - [10.3 返回](#103-返回)
    - [10.4 返回示例](#104-返回示例)
- [11 行人检测](#11-行人检测)
    - [11.1 描述](#111-描述)
    - [11.2 请求](#112-请求)
    - [11.3 返回](#113-返回)
    - [11.4 返回示例](#114-返回示例)
- [12 发型预测](#12-发型预测)
    - [12.1 描述](#121-描述)
    - [12.2 请求](#122-请求)
    - [12.3 返回](#123-返回)
    - [12.4 返回示例](#124-返回示例)

<!-- /TOC -->

# 0 常用错误代码

| 编码 | 说明 |
| :----: | :------: |
| 10001 | API Key错误 |
| 10002 | 图片中检测不到人脸 |
| 10003 | 单张图片存在多张人脸 |
| 10004 | 未知错误 |
| 10005 | Json参数错误，缺少参数或类型不对 |
| 10006 | Json body语法错误 |
| 10007 | 图片base64编码错误(删除头`data:image/jpg;base64`) |
| 10008 | 请求频率超出限制(每秒) |
| 10009 | 调用特征接口时人脸数据数组为空 |
| 10010 | 服务器授权验证未通过 |

# 1 注册/修改人脸
## 1.1 描述
传入每个人不超过5张图片和图片对应的用户分组与唯一编码，将数据存入数据库中完成注册。支持多人的图片批量注册。人数无限制。

## 1.2 请求
调用地址：tcp://127.0.0.1:5559  
接口ID：0   
请求方式：zmq  
请求类型：json message  
频率控制：有，每秒最多接收`poolSize*系数`个请求，超出限制不处理，返回`error_message: 10008`

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | interface | String | 调用此API的API ID |
| 必选 | api_key | String | 调用此API的API Key |
| 必选 | face_id | Array | 图片对应的唯一编码(不存在时创建,存在时覆盖) |
| 必选 | group_id | Array | 人脸用户分组 |
| 必选 | image_base64 | Array(2dim) | 多个人的多张人脸图片人脸图片base64编码(1-5张) |
| 可选 | landmarks | Array(2dim) | 人脸特征点，依次左眼、右眼，鼻尖，左嘴角、右嘴角（不使用landmarks系统会先进行人脸检测） |
***注意**： face_id的值必须是数字字符串数组,例如:["000001"]，下同*

## 1.3 返回
返回类型：JSON  

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| request_id | String | 用于区分每一次请求的唯一的字符串 |
| time_used | Int | 整个请求所花费的时间，单位为毫秒 |
| error_message | String | 101注册成功;102注册失败 |

## 1.4 请求示例

```json
{ 
    interface: '0',
    api_key: '',
    image_base64:
    [ [ 'base64...', 'base64...', 'base64...' ],
     [ 'base64...', 'base64...', 'base64...' ],
    ],
    face_id: [ '1', '2' ],
    group_id: [ '000001', '000001' ],
    landmarks: 
    [
        [
            { x: [ 122, 195, 162, 104, 173 ], y: [ 174, 177, 237, 256, 261 ]},
            { x: [ 70, 133, 112, 89, 137 ], y: [ 139, 131, 176, 212, 208 ]},
            { x: [ 59, 114, 88, 61, 109 ], y: [ 120, 122, 161, 179, 182 ] }
        ],
        [
            { x: [ 39, 74, 53, 35, 63 ], y: [ 72, 78, 91, 113, 119 ] },
            { x: [ 44, 91, 66, 47, 84 ], y: [ 91, 92, 118, 137, 136 ] },
            { x: [ 41, 82, 60, 50, 78 ], y: [ 87, 83, 110, 139, 135 ] }
        ]
    ] 
}

```

## 1.5 返回示例
```json
// 全部注册成功
{ 
	error_message: '101',
  	request_id: 'f9ed822f-9553-4bc2-9041-2cb609fbff15',
  	time_used: 13 
}
// 注册失败
{   error_message: '102', // 注册失败
    failed_face_id: [ '1', '2w' ], // 注册失败的face_id
    failed_group_id: [ '000001', '000001' ], // 注册失败的group_id
    failed_message: [ '10002', '10002' ], //face_id, group_id对应的注册失败的原因（如10002代表没有检测到人脸）
    request_id: 'e12dacd6-3a77-403f-9d8c-8e195f7aaf31' 
}

```

# 2 1:N搜索
## 2.1 描述
传入不多于3张图片,在人脸用户分组中搜索出最相似的一张人脸,并返回置信度。

## 2.2 请求
调用地址：tcp://127.0.0.1:5559  
接口ID：1  
请求方式：zmq   
请求类型：json message  
频率控制：有，每秒最多接收`maxRequests`个请求，超出限制返回`error_message: 10008`

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | interface | String | 调用此API的API ID |
| 必选 | api_key | String | 调用此API的API Key |
| 必选 | group_id | String | 人脸用户分组 |
| 必选 | score | Float | 相似度阈值 |
| 必选 | topk | Int | 查询最近邻的个数(1-5) |
| 可选 | field | String | 填写'feature'表示使用feature搜索模式，反之使用图片搜索，二选一 |
| 可选 | feature | Array | 要搜索的feature（256维特征向量） |
| 可选 | image_base64 | Array | 图片base64编码 |
| 可选 | landmarks | Array | 人脸特征点，依次左眼、右眼，鼻尖，左嘴角、右嘴角（不使用landmarks系统会先进行人脸检测） |

## 2.3 返回
返回类型：JSON  

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| request_id | String | 用于区分每一次请求的唯一的字符串 |
| face_id | Array | 搜索出最相似的id (最近邻的k个)|
| score | Array | 分数0~100 (最近邻的k个)|
| time_used | Int | 整个请求所花费的时间，单位为毫秒 |
| error_message | String | 201搜索成功;202未找到一个相似的人脸数 |

## 2.4 返回示例
```json
{ 
	error_message: '201',
  	face_id: ['1','2','3'],
  	request_id: '5f7ad249-fbc3-45f7-b0d3-c22d3e092d69',
  	score: [98.99,97.55,86.87],
  	time_used: 31 
 }
```

# 3 删除人脸
## 3.1 描述
传入需要删除人脸的用户分组和唯一标识,从数据库中删除该用户。

## 3.2 请求
调用地址：tcp://127.0.0.1:5559   
接口ID：2  
请求方式：zmq    
请求类型：json message  
频率控制：无

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | interface | String | 调用此API的API ID |
| 必选 | api_key | String | 调用此API的API Key |
| 必选 | face_id | String | 需要删除的人脸唯一id |
| 必选 | group_id | String | 人脸用户分组 |
| 可选 | field | String | 填写'range'表示删除范围内的,删除0-face_id 之间所有的face_id

## 3.3 返回
返回类型：JSON  

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| request_id | String | 用于区分每一次请求的唯一的字符串 |
| time_used | Int | 整个请求所花费的时间，单位为毫秒 |
| error_message | String | 301删除成功;302删除失败 |

## 3.4 返回示例
```json
{ 
	error_message: '301',
  	request_id: '7c9a3043-f0ec-4336-84a1-52a0b61e6c41',
  	time_used: 0 
}
```

# 4 查询人脸
## 4.1 描述
传入需要查询人脸的用户分组和唯一标识,在数据库中进行查询,查到则判断该用户已注册,否则判断用户未注册。

## 4.2 请求
调用地址：tcp://127.0.0.1:5559   
接口ID：3  
请求方式：zmq    
请求类型：json message  
频率控制：无  

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | interface | String | 调用此API的API ID |
| 必选 | api_key | String | 调用此API的API Key |
| 必选 | face_id | String | 需要查询的人脸唯一id |
| 必选 | group_id | String | 人脸用户分组 |

## 4.3 返回
返回类型：JSON  

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| request_id | String | 用于区分每一次请求的唯一的字符串 |
| time_used | Int | 整个请求所花费的时间，单位为毫秒 |
| error_message | String | 401此face_id已注册;402此face_id未注册 |

## 4.4 返回示例
```json
{ 
	error_message: '401',
  	request_id: 'cb5079a7-e047-4764-a9c5-1d2f3b97f7bf',
  	time_used: 0 
}
```

# 5 1:1比对
## 5.1 描述
传入两张图片判断是否为同一个人。  
传入两张人脸图片后分别进行人脸检测与特征提取，然后进行特征比对并返回两张人脸的相似度，当相似度大于阈值时判断为同一个人，否则判断为不是同一个人。

## 5.2 请求
调用地址：tcp://127.0.0.1:5559    
接口ID：4  
请求方式：zmq    
请求类型：json message  
频率控制：有，每秒最多接收`maxRequests`个请求，超出限制返回`error_message: 10008`

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | interface | String | 调用此API的API ID |
| 必选 | api_key | String | 调用此API的API Key |
| 必选 | image_base64 | Array | 图片base64编码 |
| 可选 | landmarks | Array | 人脸特征点，依次左眼、右眼，鼻尖，左嘴角、右嘴角（不使用landmarks系统会先进行人脸检测） |

## 5.3 返回
返回类型：JSON  

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| request_id | String | 用于区分每一次请求的唯一的字符串 |
| score | float | 两张人脸相似度(0~100) |
| time_used | Int | 整个请求所花费的时间，单位为毫秒 |
| error_message | String | 501同一个人;502不是同一个人 |

## 5.4 返回示例
```json
{ 
	error_message: '501',
  	request_id: '2f01df20-95fc-4a0e-a48c-225c7480d510',
  	score: 99.99,
  	time_used: 20 
}
```

# 6 人脸检测
## 6.1 描述
传入一张图片进行人脸检测和人脸分析。  
可以检测图片内所有人脸，并对检测到的所有人脸进行分析，获得人脸的5个关键点和各类属性信息。

## 6.2 请求
调用地址：tcp://127.0.0.1:5559    
接口ID：5  
请求方式：zmq    
请求类型：json message  
频率控制：有，每秒最多接收`maxRequests`个请求，超出限制返回`error_message: 10008`

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | interface | String | 调用此API的API ID |
| 必选 | api_key | String | 调用此API的API Key |
| 必选 | image_base64 | String | 图片 base64数据 |
| 可选 | minface_size | int | 最小搜索范围(默认最小值80) |
| 可选 | maxface_size | int | 最大搜索范围(默认最大值300,最大值最小值相差大于200) |
| 可选 | classify | String | 人脸属性(性别,年龄和发型) 例:"gender,age,hair",注:tarck模式下此选项无效|
| 可选 | field | String | 填写'portrait'表示使用肖像模式，在登记照场景下使用，在该模式下无需设置minface_size和maxface_size; 填写'track'表示使用人脸追踪模式，人脸追踪模式下当人脸追踪丢失后会将人脸数据返回，相关结果包含返回值'track'域中。 |
| 可选 | image_id | String | 图片编号（唯一，track模式下可选，detect模式下无用） |
| 可选 | camera_id | String | 摄像机编号（唯一，track模式下必选，detect模式下无用） |

## 6.3 返回
返回类型：JSON  

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| request_id | String | 用于区分每一次请求的唯一的字符串 |
| time_used | Int | 整个请求所花费的时间，单位为毫秒 |
| error_message | String | 601检测到人脸; |
| detect_nums | int | 图片中人脸个数 |
| detect | Array | 当前图片中人脸的信息 |
| detect.left | Int | 人脸框左上角的横坐标 |
| detect.top | Int | 人脸框左上角的纵坐标 |
| detect.width | Int | 人脸框宽度 |
| detect.height | Int | 人脸框高度 |
| detect.landmark | Object | 人脸特征点，依次左眼、右眼，鼻尖，左嘴角、右嘴角 |
| detect.sideFace | float | 是否侧脸（1表示侧脸，0表示正脸）|
| detect.quality | float | 图片质量（1表示人脸质量可靠，0表示人脸质量不可靠） |
| detect.pitch | double | 三维旋转之俯仰角度[-90(上), 90(下)], 参考值：小于20 |
| detect.yaw | double | 平面内旋转角[-180(逆时针), 180(顺时针)], 参考值：小于20 |
| detect.roll | double | 三维旋转之左右旋转角[-90(左), 90(右)], 参考值：小于20 |
| detect.blur | double | 模糊度,取值范围[0~1],0是最清晰,1是最模糊, 参考值：小于0.7 |
| detect.illu | double | 脸部光照的灰度值,取值范围[0~255], 0表示光照不好, 参考值：大于40 |
| detect.score | float | 人脸可信度，过低可能不是人脸 |
| detect.glasses | float | 是否带眼镜，0不带，1带 |
| detect.female | float | Female百分比 |
| detect.male | float | Male百分比|
| detect.age | Int | 每个图片的年龄|
| detect.hair | String | 每个图片的发型( 四种：shortHair`|`longHair`|`hat`|`braids/short ) |

---
| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| track_nums | int | 追踪完毕的人脸个数 |  
| track | Array | 追踪完毕的人脸信息 |  
| track.image_id | String | 该目标最清晰人脸所处在图片的ID | 
| track.inTime | String | 该目标进入图像的时间（国际标准时间公元1970年1月1日） |
| track.bestTime | String | 该目标被抓拍图像的时间（国际标准时间公元1970年1月1日） |
| track.outTime | String | 该目标退出图像的时间（国际标准时间公元1970年1月1日） |
| track.left | Int | 人脸框左上角的横坐标 |
| track.top | Int | 人脸框左上角的纵坐标 |
| track.width | Int | 人脸框宽度 |
| track.height | Int | 人脸框高度 |
| track.landmark | Object | 人脸特征点，依次左眼、右眼，鼻尖，左嘴角、右嘴角 |
| track.sideFace | float | 是否侧脸（1表示侧脸，0表示正脸）|
| track.quality | float | 图片质量（1表示人脸质量可靠，0表示人脸质量不可靠） |
| track.score | float | 人脸可信度，过低可能不是人脸 |
| track.glasses | float | 是否带眼镜，0不带，1带 |
| track.image_base64 | String | 图片 base64数据 |

## 6.4 返回示例
```json
{ 
            error_message: '601',
            face_nums: 4,
            request_id: '3eb6a9db-ae3d-4de7-94ee-577a748628d4',
            detect: 
            [   
                { glasses: 0,height: 77,landmark: [Object],left: 711,quality: 1,score: 0.9988855719566345,sideFace: 0,top: 108,width: 50 },
                { glasses: 0,height: 66,landmark: [Object],left: 115, quality: 1,score: 0.9965169429779053,sideFace: 0, top: 217,width: 42 },
                { glasses: 0,height: 50,landmark: [Object],left: 764,quality: 1,score: 0.9956468939781189,sideFace: 0,top: 129,width: 40 },
                { glasses: 0,height: 70,landmark: [Object],left: 538,quality: 1,score: 0.9951795339584351,sideFace: 0,top: 197,width: 47 } 
            ],
            track: 
            [   
                { glasses: 0,height: 77,landmark: [Object],left: 711,quality: 1,score: 0.9988855719566345,sideFace: 0,top: 108,width: 50,image_id: '12343243', inTime: "12312321",outTime: "345345345"，bestTime: "345345345" },
                { glasses: 0,height: 66,landmark: [Object],left: 115, quality: 1,score: 0.9965169429779053,sideFace: 0, top: 217,width: 42,image_id: '12343243', inTime: "12312321",outTime: "345345345"，bestTime: "345345345" },
                { glasses: 0,height: 50,landmark: [Object],left: 764,quality: 1,score: 0.9956468939781189,sideFace: 0,top: 129,width: 40,image_id: '12343243', inTime: "12312321",outTime: "345345345"，bestTime: "345345345" },
                { glasses: 0,height: 70,landmark: [Object],left: 538,quality: 1,score: 0.9951795339584351,sideFace: 0,top: 197,width: 47,image_id: '12343243', inTime: "12312321",outTime: "345345345"，bestTime: "345345345" } 
            ],
            time_used: 104 
}

```

# 7 特征提取
## 7.1 描述
传入图片进行人脸特征提取。  
可以传入多张图片分别提取人脸特征,每张图片中,只对人脸框面积最大的 1 个人脸进行特征提取,最终返回每张图片的人脸特征。

## 7.2 请求
调用地址：tcp://127.0.0.1:5559    
接口ID：6  
请求方式：zmq    
请求类型：json message  
频率控制：无  

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | interface | String | 调用此API的API ID |
| 必选 | api_key | String | 调用此API的API Key |
| 必选 | image_base64 | Array | 图片base64编码 |
| 必选 | landmarks | Array | 人脸特征点，依次左眼、右眼，鼻尖，左嘴角、右嘴角（不使用landmarks系统会先进行人脸检测） |

## 7.3 请求示例
```json
{ 
  interface：'6',
  api_key: '', 
  image_base64: ['base64...', 'base64...', 'base64...'],
  landmark: [
  { x: [ 2398, 2439, 2420, 2400, 2436 ], y: [ 459, 454, 483, 498, 493 ] }, 
  { x: [ 2398, 2439, 2420, 2400, 2436 ], y: [ 459, 454, 483, 498, 493 ] }, 
  { x: [ 2398, 2439, 2420, 2400, 2436 ], y: [ 459, 454, 483, 498, 493 ] }
  ]
}
```
## 7.4 返回
返回类型：JSON  

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| request_id | String | 用于区分每一次请求的唯一的字符串 |
| time_used | Int | 整个请求所花费的时间，单位为毫秒 |
| error_message | String | 701提取成功 |
| feature | Array | 每个图片的特征|
| dimension | Int | 特征维数|

## 7.5 返回示例
```json
{ 
        dimension: 256,
        error_message: '701',
        feature:
        [ 
            [Object],
            [Object]
        ],
        request_id:
        '3daa9e2e-1a49-4614-a19e-58fc74878b56',
        time_used: 64 
}
```

# 8 性别预测
## 8.1 描述
传入图片进行性别预测。  
可以传入多张图片分别预测性别,每张图片中,要求人脸完整包括发型等。

## 8.2 请求
调用地址：tcp://127.0.0.1:5559    
接口ID：7  
请求方式：zmq    
请求类型：json message    
频率控制：无  

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | interface | String | 调用此API的API ID |
| 必选 | api_key | String | 调用此API的API Key |
| 必选 | image_base64 | Array | 图片base64编码数组 |

## 8.3 返回
返回类型：JSON  

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| request_id | String | 用于区分每一次请求的唯一的字符串 |
| time_used | Int | 整个请求所花费的时间，单位为毫秒 |
| error_message | String | 801提取成功 |
| gender | Array | 每个图片的性别百分比,包括Female百分比和Male百分比|
| gender.Female | float | Female百分比 |
| gender.Male | float | Male百分比|

## 8.4 返回示例
```json
{
        error_message: '801',
        gender:
        [ { Female: 0.9985412359237671, Male: 0.0014587575569748878 },
          { Female: 0.5616888999938965, Male: 0.4383111000061035 },
          { Female: 0.00000861127318785293, Male: 0.9999914169311523 },
          { Female: 0.9985412359237671, Male: 0.0014587575569748878 },
          { Female: 0.9985412359237671, Male: 0.0014587575569748878 } ],
        request_id:
        '3daa9e2e-1a49-4614-a19e-58fc74878b56',
        time_used: 64 
}
```
# 9 年龄预测
## 9.1 描述
传入图片进行年龄预测。  
可以传入多张图片分别预测年龄。

## 9.2 请求
调用地址：tcp://127.0.0.1:5559    
接口ID：8  
请求方式：zmq    
请求类型：json message    
频率控制：无  

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | interface | String | 调用此API的API ID |
| 必选 | api_key | String | 调用此API的API Key |
| 必选 | image_base64 | Array | 图片base64编码数组 |

## 9.3 返回
返回类型：JSON  

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| request_id | String | 用于区分每一次请求的唯一的字符串 |
| time_used | Int | 整个请求所花费的时间，单位为毫秒 |
| error_message | String | 901预测成功 |
| age | Array | 每个图片的年龄|

## 9.4 返回示例
```json
{
        error_message: '901',
        age:
        [ 
            "adult",
            "kid"
        ],
        request_id: '3daa9e2e-1a49-4614-a19e-58fc74878b56',
        time_used: 64 
}
```
# 10 活体检测
## 10.1 描述
传入图片进行活体检测。  
传入两张同一个人的图片，根据图像判断人是活体还是只是照片。要求传入的照片只有一张人脸，而且传入的照片尺度大小要一样。

## 10.2 请求
调用地址：tcp://127.0.0.1:5559    
接口ID：9  
请求方式：zmq    
请求类型：json message    
频率控制：无  

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | interface | String | 调用此API的API ID |
| 必选 | live | Array| 带人脸的图片数组,两张base64图片为一组检测类型,检测类型的顺序可以任意 |
| 必选 | live[0].type | String| 检测类型, 眨眼 blink, 点头 nod, 摇头 shake ,张嘴 mouth|
| 必选 | live[0].image1_base64 | String| 图片 base64 编码 |
| 必选 | live[0].image2_base64 | String| 图片 base64 编码 |
| 必选 | live[1].type | String| 检测类型, 眨眼 blink, 点头 nod, 摇头 shake ,张嘴 mouth|
| 必选 | live[1].image1_base64 | String| 图片 base64 编码 |
| 必选 | live[1].image2_base64 | String| 图片 base64 编码 |
| 必选 | live[2].type | String| 检测类型, 眨眼 blink, 点头 nod, 摇头 shake ,张嘴 mouth|
| 必选 | live[2].image1_base64 | String| 图片 base64 编码 |
| 必选 | live[2].image2_base64 | String| 图片 base64 编码 |
| 必选 | live[3].type | String| 检测类型, 眨眼 blink, 点头 nod, 摇头 shake ,张嘴 mouth|
| 必选 | live[3].image1_base64 | String| 图片 base64 编码 |
| 必选 | live[3].image2_base64 | String| 图片 base64 编码 |


## 10.3 请求示例

```json
{
    interface : "9",
    api_key : "",
    live : [{type : "nod", image1_base64 : "base64...", image2_base64 : "base64..."},
            {type : "shake", image1_base64 : "base64...", image2_base64 : "base64..."},
            {type : "blink", image1_base64 : "base64...", image2_base64 : "base64..."}
           ]
}
```

## 10.4 返回
返回类型：JSON  

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| request_id | String | 用于区分每一次请求的唯一的字符串 |
| time_used | Int | 整个请求所花费的时间，单位为毫秒 |
| error_message | String | 1001预测成功 |
| lives | String | 每个图片是否为活体|

## 10.5 返回示例
```json
{
        error_message: '1001',
        lives: 'not live',
        request_id: '5d346983-9f28-47c6-a569-b25c0425358c',
        time_used: 1767 
}
```
# 11 行人检测
## 11.1 描述
传入图片进行行人检测。  
传入一张图片，对图片中行人进行检测，返回行人的轮廓坐标和宽与高。

## 11.2 请求
调用地址：tcp://127.0.0.1:5561
接口ID：10 
请求方式：zmq    
请求类型：json message    
频率控制：无  

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | interface | String | 调用此API的API ID |
| 必选 | api_key | String | 调用此 API 的 API Key | 
| 必选 | image1_base64 | String| 图片 base64 编码 |  

## 11.3 返回
返回类型：JSON  

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| request_id | String | 用于区分每一次请求的唯一的字符串 |
| time_used | Int | 整个请求所花费的时间，单位为毫秒 |
| error_message | String | 1101检测到行人 |
| person_nums | Int | 检测到的行人数量|
| time_used | Int | 整个请求所花费的时间，单位为毫秒 |
| result | Array | 每个行人的轮廓|
| result.left | Int | 左上点水平方向坐标|
| result.top | Int | 左上点竖直方向坐标|
| result.width | Int | 行人轮廓的宽度|
| result.height | Int | 行人轮廓的高度 |


## 11.4 返回示例
```json
{ 
            error_message: '1101',
            person_nums: 6,
            request_id: '32c6fd2d-9827-482d-9d43-5f313b7e31d0',
            result: 
            [ 
                { height: 1505, left: 1918, top: 147, width: 619 },
                { height: 1285, left: 1437, top: 524, width: 598 },
                { height: 1299, left: 224, top: 600, width: 446 },
                { height: 587, left: 0, top: 309, width: 251 },
                { height: 1169, left: 2770, top: 297, width: 301 },
                { height: 495, left: 2077, top: 0, width: 440 } 
            ],
            time_used: 495 
}
```

# 12 发型预测
## 12.1 描述
传入图片进行发型预测。  
可以传入多张图片分别预测发型。

## 12.2 请求
调用地址：tcp://127.0.0.1:5559    
接口ID：11  
请求方式：zmq    
请求类型：json message    
频率控制：无 

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | api_key | String | 调用此API的API Key |
| 必选 | image_base64 | Array | 图片base64编码数组 |

## 12.3 返回
返回类型：JSON  

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| request_id | String | 用于区分每一次请求的唯一的字符串 |
| time_used | Int | 整个请求所花费的时间，单位为毫秒 |
| error_message | String | 1201预测成功 |
| hair | Array | 每个图片的发型( 四种：shortHair`|`longHair`|`hat`|`braids/short ) |

## 12.4 返回示例
```json
{
        error_message: '1201',
        hair:
        [ 
            "shortHair",
            "longHair",
            "hat"
        ],
        request_id: '63ee91c4-6ffb-4129-a806-3668ac88bcbc',
        time_used: 300
}
```

# 13 查询注册库信息
## 13.1 描述
传入需要查询人脸的用户分组,在数据库中进行查询,返回该用户分组的总注册数，若不传入用户分组，则返回所有分组注册总数。

## 13.2 请求
调用地址：tcp://127.0.0.1:5559
接口ID：12
请求方式：zmq
请求类型：json message
频率控制：无

| 是否必选 | 参数名 | 类型 | 参数说明 |
| :------: | :----: | :--: | :------: |
| 必选 | interface | String | 调用此API的API ID |
| 必选 | api_key | String | 调用此API的API Key |
| 可选 | group_id | String | 人脸用户分组 |

***注意**： group_id为可选参数,传入group_id时,返回该group_id的总注册数，若不传入group_id，则返回所有分组的注册总数。

## 13.3 返回
返回类型：JSON

| 参数名 | 类型 | 参数说明 |
| :----: | :--: | :------: |
| request_id | String | 用于区分每一次请求的唯一的字符串 |
| time_used | Int | 整个请求所花费的时间，单位为毫秒 |
| reg_nums | Int | 注册数量 |
| error_message | String | 1301 查询成功|

## 13.4 返回示例
```json
{
	error_message: '1301',
  	request_id: 'cb5079a7-e047-4764-a9c5-1d2f3b97f7bf',
  	group_id：'00001',
  	reg_nums：2,
  	time_used: 0
}
```