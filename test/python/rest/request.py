#!/usr/bin/python
# -*- coding: utf-8 -*-

# python request.py a.jpg 192.168.1.2
import sys
import requests
import base64
import json
import time


if __name__ == '__main__':
    length = len(sys.argv)
    ip = '127.0.0.1'
    port = '33388'
    api = '/feature'
    if length == 2:
        photo = sys.argv[1]
    elif length == 3:
        photo = sys.argv[1]
        ip = sys.argv[2]
    else:
        print '[image] [IP]'
    address = 'http://' + ip + ':' + port
    url = address + api
    headers = {'Content-Type': 'application/json'}

    data = {
        'api_key': '',
        'image_base64': []
    }
    fp = open(photo, 'rb')
    data['image_base64'].append(base64.b64encode(fp.read()))
    fp.close()
    start = time.clock()
    r = requests.post(url, data=json.dumps(data))
    elapsed = (time.clock() - start)
    result = r.json()
    print result
    print 'Time used:', elapsed * 1000
