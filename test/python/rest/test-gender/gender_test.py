#!/usr/bin/python
# -*- coding: utf-8 -*-
# python gender_test.py *.jpg
import sys
import requests
import base64
import json
import os
import time
import shutil
if __name__ == '__main__':
    length = len(sys.argv)
    # ip = '192.168.1.7'
    ip = '127.0.0.1'
    if length == 2:
        ip = sys.argv[1]
    address = 'http://' + ip + ':33388'
    url = address + '/gender'
    headers = {'Content-Type': 'application/json'}
    data = {
        'api_key': '',
        'image_base64': []
    }
    # fd = open(gender-fail.txt, 'w')
    # imageDir = "/home/wdk/workspace_git_push/detector/test/python-test/test-gender/gender-test"
    os.system('tar zxf gender-test.tar.gz')
    imageDir = "gender-test"
    gender_types = os.listdir(imageDir)
    print gender_types
    suc = 0
    suc_Male = 0
    suc_Female = 0
    total_Male = 0
    total_Female = 0
    total = 0
    for gender_type in gender_types:
        print gender_type
        f = open(imageDir + '/' + '../' + gender_type + '.txt', 'r')
        imgList = list(f)
        f.close()
        persons = len(imgList)
        if gender_type == 'Male':
            total_Male = persons
        if gender_type == 'Female':
            total_Female = persons
        # persons = 1
        # print persons
        for i in range(persons):
            # print '***********************'
            data['image_base64'] = []
            photo = imageDir + '/' + imgList[i].split('\n')[0]
            # print photo
            fp = open(photo, 'rb')
            data['image_base64'].append((base64.b64encode(fp.read())))
            fp.close()
            print json.dumps(data)
            r = requests.post(url, data=json.dumps(data))
            result = r.json()
            # print result['gender'][0]
           
            if result['gender'][0] == gender_type and result['gender'][0] == 'Male':
                suc_Male = suc_Male + 1
            if result['gender'][0] == gender_type and result['gender'][0] == 'Female':
                suc_Female = suc_Female + 1

            if result['gender'][0] == gender_type:
                suc = suc + 1
            # else:
            # #     # print photo
            #     oldname = photo
            #     newname = imageDir + '/../tmp/' + imgList[i].split('\n')[0]
            #     shutil.copyfile(oldname,newname)
            total = total + 1 
            # print r.json()
            time.sleep(0.010)
    os.system('rm -rf gender-test')
    print "总人数：" + str(total)
    print "男性总人数：" + str(total_Male)
    print "女性总人数：" + str(total_Female)
    correct_rate = suc / float(total) * 100
    correct_male_rate = suc_Male / float(total_Male) * 100
    correct_female_rate = suc_Female / float(total_Female) * 100    
    print "男女性别识别正确的概率为：" + str(correct_rate)
    print "男性性别识别正确的概率为：" + str(correct_male_rate)
    print "女性性别识别正确的概率为：" + str(correct_female_rate)



# if __name__ == '__main__':
#     # ip = '192.168.1.170'
#     ip = '127.0.0.1'
#     address = 'http://' + ip + ':33379'
#     url = address + '/gender'
#     headers = {'Content-Type': 'application/json'}
#     data = {
#         'api_key': '',
#         'image_base64': []
#     }
#     imageDir = "/home/wdk/workspace/verifier/test/python-test/gender-test"
 
#     total = 0
#     f = open(imageDir + '/' + '../' + 'test_persons.txt', 'r')
#     imgList = list(f)
#     f.close()
#     persons = len(imgList)
#     for i in range(persons):
#         # print '***********************'
#         data['image_base64'] = []
#         photo = imageDir + '/' + imgList[i].split('\n')[0]
#         # print photo
#         fp = open(photo, 'rb')
#         data['image_base64'].append((base64.b64encode(fp.read())))
#         fp.close()
#         # print json.dumps(data)
#         r = requests.post(url, data=json.dumps(data))
#         result = r.json()
        
#         if result['gender'][0] == 'Male':
#             oldname = photo
#             newname = imageDir + '/../tmp/Male/' + imgList[i].split('\n')[0]
#             shutil.copyfile(oldname,newname) 
#         elif result['gender'][0] == 'Female':
#             oldname = photo
#             newname = imageDir + '/../tmp/Female/' + imgList[i].split('\n')[0]
#             shutil.copyfile(oldname,newname)
#         total = total + 1 
#         print total
#         # print r.json()
#         time.sleep(0.010)
#     print "done!!!"


# if __name__ == '__main__':
#     ip = '192.168.1.170'
#     # ip = '127.0.0.1'
#     photo = sys.argv[1]
#     address = 'http://' + ip + ':33388'
#     url = address + '/gender'
#     headers = {'Content-Type': 'application/json'}
#     data = {
#         'api_key': '',
#         'image_base64': []
#     }
#     data['image_base64'] = []
#     fp = open(photo, 'rb')
#     data['image_base64'].append((base64.b64encode(fp.read())))
#     fp.close()
#     # print data['image_base64']
#     # print json.dumps(data)
#     r = requests.post(url, data=json.dumps(data))
#     # result = r.json()
#     print r.json()
























