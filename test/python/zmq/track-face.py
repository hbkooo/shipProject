# -*- coding: utf-8 -*-
import cv2
import os
import sys
import time
import zmq
import json
import time
import base64
import requests
import datetime
import numpy as np
from PIL import Image
from cStringIO import StringIO


def send_message(data):
    data_send = json.dumps(data)
    socket.send(data_send)
    message = socket.recv()
    r = message[:-2]
    results = json.loads(r)
    return results


def detect(image_base64, image_id):
    data = {'interface': '5', 'api_key': '', 'image_base64': "", 'field':"track",'image_id': "0",'camera_id':"1"}
    # data = {'interface': '5', 'api_key': '', 'image_base64': ""}
    data['image_base64'] = image_base64
    data['image_id'] = image_id
    return send_message(data)


if __name__ == '__main__':
    headers = {'Content-Type': 'application/json'}
    font = cv2.FONT_HERSHEY_SIMPLEX
    num = 0
    context = zmq.Context()
    socket = context.socket(zmq.DEALER)
    ip = "127.0.0.1"
    url = "tcp://" + ip + ":5559"
    socket.connect(url)
    print 'connect ok'
 
    buffer = []
    cap = cv2.VideoCapture(0)
    k = 0
    # 打开摄像头，若打开本地视频，同 opencv 一样，只需将０换成("×××.avi")
    while (1):  # get a frame
        ret, frame = cap.read()  # show a frameq
        image = Image.fromarray(cv2.cvtColor(frame, cv2.COLOR_BGR2RGB))
        output_buffer = StringIO()
        image.save(output_buffer, format='JPEG')
        binary_data = output_buffer.getvalue()
        base64_data = base64.b64encode(binary_data)
        buffer.append(frame)
        result = detect(base64_data, str(k))

        print result
        if result['error_message'] == '10008':
            continue
        if result['track_nums'] > 0:
            print "result['track_nums']", result['track_nums']
            print k
            image_id = int(result['track'][0]['imageId'])
            bestframe = buffer[image_id]
            cv2.imshow("bestframe", bestframe)

        k = k + 1
        faceNumber = result['detect_nums']
        for j in range(faceNumber):
            x = int(result['detect'][j]['left'])
            y = int(result['detect'][j]['top'])
            width = int(result['detect'][j]['width'])
            height = int(result['detect'][j]['height'])
            # rectangle(Mat格式的图像, Point(左上角点的列坐标, 左上角的点的行坐标), Point(右下角点的列坐标, 右下角点的行坐标), Scalar(255, 0, 0), 1, 8, 0);
            # crop = path[y:y+height, x:x+width]  #裁剪
            cv2.rectangle(frame, (x, y), (x + width, y + height), (0, 0, 255), 3, 8,
                          0)  # 画矩形框
            for i in range(5):
                center_x = int(result['detect'][j]['landmark']['x'][i] + x)
                center_y = int(result['detect'][j]['landmark']['y'][i] + y)
                cv2.rectangle(frame, (center_x, center_y),
                              (center_x + 2, center_y + 2), (0, 0, 255), 2, 8,
                              0)  # 画五个特征点
            # cv.imwrite("./crop-" + imgName, path)
        cv2.imshow("capture", frame)
        time.sleep(0.01)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    cap.release()
    cv2.destroyAllWindows()
# 释放并销毁窗口
