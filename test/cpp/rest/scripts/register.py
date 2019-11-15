#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys
import aiohttp
import base64
import json
import time
import asyncio

IP = '127.0.0.1'
PORT_1 = '33388'
PORT_2 = '33389'
IMAGE_SIZE = 1
IMAGE_TEST = 'a.jpg'
tasks = []


def prepare():
    for i in range(IMAGE_SIZE):
        if i % 2:
            tasks.append(doRegister(IP, PORT_1, IMAGE_TEST))
        else:
            tasks.append(doRegister(IP, PORT_2, IMAGE_TEST))


@asyncio.coroutine
def doRegister(ip, port, image_path):
    address = 'http://' + ip + ':' + port
    url = address + '/reg'
    headers = {'Content-Type': 'application/json'}
    data = {
        'api_key': 'value',
        'image_base64': '',
        'face_id': '000001',
        'group_id': '000001'
    }
    data['face_id'] = image_path
    fp = open(image_path, 'rb')
    data['image_base64'] = str(base64.b64encode(fp.read()))
    fp.close()
    session = yield from aiohttp.ClientSession()
    resp = yield from session.post(url, data=json.dumps(data))
    result = yield from resp.text()
    print(result)

if __name__ == '__main__':
    prepare()
    start = time.clock()
    loop = asyncio.get_event_loop()
    loop.run_until_complete(asyncio.wait(tasks))
    loop.close()
    print("Cost time : " + str(time.clock - start))
