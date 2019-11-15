#!/usr/bin/python
# -*- coding: utf-8 -*-

# python reg_all.py 192.168.1.2
import sys
import requests
import base64
import json


if __name__ == '__main__':
    length = len(sys.argv)
    ip = '127.0.0.1'
    if length == 2:
        ip = sys.argv[1]
    address = 'http://' + ip + ':33388'
    url = address + '/reg'
    headers = {'Content-Type': 'application/json'}

    data = {
        'api_key': 'value',
        'image_base64': [],
        'face_id': '000001',
        'group_id': '000001'
    }
    imageDir = "/home/lchy/dataSet/casia-10000"
    f = open(imageDir + '/reg-imagelist.txt', 'r')
    imgList = list(f)
    f.close()
    persons = len(imgList) / 5
    fsuccess = open('reg-success.txt', 'w')
    ffail = open('reg-failed.txt', 'w')
    for i in range(persons):
        start = i * 5
        data['face_id'] = imgList[start].split("/")[0]
        print '***********************'
        data['image_base64'] = []
        for i in range(5):
            photo = imageDir + '/casia/' + imgList[start + i].split('\n')[0]
            fp = open(photo, 'rb')
            data['image_base64'].append((base64.b64encode(fp.read())))
            fp.close()
        # print json.dumps(data)
        r = requests.post(url, data=json.dumps(data))
        result = r.json()
        if result['error_message'] == '101':
            fout = fsuccess
        else:
            fout = ffail
        for i in range(5):
            fout.write(imgList[start + i])
        # print r.json()
    fsuccess.close()
    ffail.close()
