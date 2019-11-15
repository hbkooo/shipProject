#!/usr/bin/python
# -*- coding: utf-8 -*-
# use-help: python zmq-detect-markface.py a.jpg
import cv2 as cv
import sys
import base64
import json
import zmq

if __name__ == '__main__':
    context = zmq.Context()
    socket = context.socket(zmq.DEALER)
    url = "tcp://127.0.0.1:5559"
    socket.connect(url)
    print 'connect ok'
    data = {
        'interface': '5',
        'api_key': '',
        'image_base64': ""
    }
    imgName = sys.argv[1]
    fp = open(imgName, 'rb')
    data['image_base64'] = base64.b64encode(fp.read())
    fp.close()
    data_send = json.dumps(data)
    socket.send(data_send)
    message = socket.recv()
    print message
    r = message[:-2]
    result = json.loads(r)
    path=cv.imread(imgName)       
    faceNumber = result['face_nums']  

    for j in range(faceNumber):    
        x=int(result['result'][j]['left'])
        y=int(result['result'][j]['top'])
        width=int(result['result'][j]['width'])
        height=int(result['result'][j]['height'])
        # rectangle(Mat格式的图像, Point(左上角点的列坐标, 左上角的点的行坐标), Point(右下角点的列坐标, 右下角点的行坐标), Scalar(255, 0, 0), 1, 8, 0);    
        crop = path[y:y+height, x:x+width]  #裁剪
        # cv.rectangle(path,(x,y),(x+width,y+height),(0, 0, 255), 3, 8, 0) #画矩形框
        for i in range(5):
            center_x = int(result['result'][j]['landmark']['x'][i] + x)
            center_y = int(result['result'][j]['landmark']['y'][i] + y)
            cv.rectangle(crop,(center_x,center_y),(center_x+2,center_y-2),(0, 0, 255), 2, 8, 0) #画五个特征点
    cv.imwrite("./crop-" + imgName, crop)
    # cv.imwrite("./zmq-result-" + imgName,path)    #存储结果图像
    print "mark all faces done!!!"
