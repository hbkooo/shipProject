#!/usr/bin/python
# -*- coding:utf-8 -*-
import os
import sys
import glob
import zmq
import json
import base64
import pandas as pd
import shutil


def send_message(data):
    data_send = json.dumps(data)
    socket.send(data_send)
    message = socket.recv()
    r = message[:-2]
    results = json.loads(r)
    return results

def pandas_df_to_markdown_table(df):
    fmt = [':---:' for i in range(len(df.columns))]
    df_fmt = pd.DataFrame([fmt], columns=df.columns)
    df_formatted = pd.concat([df_fmt, df])
    df_formatted.to_csv("data.md", sep="|", index=False)

def detect(image_base64):
    data = {'interface': '5', 'api_key': '', 'image_base64': ""}
    data['image_base64'] = image_base64
    return send_message(data)

def gender(image_base64):
    data = {'interface': '7', 'api_key': '', 'image_base64': []}
    data['image_base64'].append(image_base64)
    return send_message(data)

def age(image_base64):
    data = {'interface': '8', 'api_key': '', 'image_base64': []}
    data['image_base64'].append(image_base64)
    return send_message(data)

def hair(image_base64):
    data = {'interface': '11', 'api_key': '', 'image_base64': []}
    data['image_base64'].append(image_base64)
    return send_message(data)

if __name__ == '__main__':
    pics = []
    label = [
        'name', 'sideface', 'score', 'glasses', 'quality', 'gender', 'hair',
        'age'
    ]
    df = pd.DataFrame(columns=label, index=pics)
    context = zmq.Context()
    socket = context.socket(zmq.DEALER)
    ip = "127.0.0.1"
    url = "tcp://" + ip + ":5559"
    socket.connect(url)
    print 'connect ok'
    data = {'interface': '5', 'api_key': '', 'image_base64': ""}
    pics_dir = "/home/wdk/tmp/test-1000-pics/"
    for i in range(7):
        os.mkdir(pics_dir + label[i + 1])
    sideface_path = pics_dir + label[1] + '/'
    score_path = pics_dir + label[2] + '/'
    glasses_path = pics_dir + label[3] + '/'
    quality_path = pics_dir + label[4] + '/'
    gender_path = pics_dir + label[5] + '/'
    hair_path = pics_dir + label[6] + '/'
    age_path = pics_dir + label[7] + '/'
    os.chdir(pics_dir)
    for pic in glob.glob("*.jpg"):
        pics.append(pic)
        df_tmp = df.reindex(pics)
        imgName = pics_dir + pic
        fp = open(imgName, 'rb')
        image_base64 = base64.b64encode(fp.read())
        fp.close()
## detect api
        df_tmp['name'][pic] = pic
        results = detect(image_base64)
        if results['error_message'] == '602':
            print pic
            # os.remove(imgName)
        elif results['error_message'] == '601':
            df_tmp['score'][pic] = round(results['result'][0]['score'], 5)

            if results['result'][0]['score'] >= 0.95:
                if (os.path.exists(score_path + '0.95+/') == False):
                    os.mkdir(score_path + '0.95+/')
                shutil.copyfile(imgName, score_path + '0.95+/' + pic)
            elif results['result'][0]['score'] < 0.95:
                if (os.path.exists(score_path + '0.95-/') == False):
                    os.mkdir(score_path + '0.95-/')
                shutil.copyfile(imgName, score_path + '0.95-/' + pic)

            df_tmp['sideface'][pic] = results['result'][0]['sideFace']

            if results['result'][0]['sideFace'] == 1.0:
                if (os.path.exists(sideface_path + '1.0/') == False):
                    os.mkdir(sideface_path + '1.0/')
                shutil.copyfile(imgName, sideface_path + '1.0/' + pic)
            elif results['result'][0]['sideFace'] == 0.0:
                if (os.path.exists(sideface_path + '0.0/') == False):
                    os.mkdir(sideface_path + '0.0/')
                shutil.copyfile(imgName, sideface_path + '0.0/' + pic)

            df_tmp['glasses'][pic] = results['result'][0]['glasses']

            if results['result'][0]['glasses'] == 1.0:
                if (os.path.exists(glasses_path + '1.0/') == False):
                    os.mkdir(glasses_path + '1.0/')
                shutil.copyfile(imgName, glasses_path + '1.0/' + pic)
            elif results['result'][0]['glasses'] == 0.0:
                if (os.path.exists(glasses_path + '0.0/') == False):
                    os.mkdir(glasses_path + '0.0/')
                shutil.copyfile(imgName, glasses_path + '0.0/' + pic)

            df_tmp['quality'][pic] = results['result'][0]['quality']

            if results['result'][0]['quality'] == 1.0:
                if (os.path.exists(quality_path + '1.0/') == False):
                    os.mkdir(quality_path + '1.0/')
                shutil.copyfile(imgName, quality_path + '1.0/' + pic)
            elif results['result'][0]['quality'] == 0.0:
                if (os.path.exists(quality_path + '0.0/') == False):
                    os.mkdir(quality_path + '0.0/')
                shutil.copyfile(imgName, quality_path + '0.0/' + pic)

## gender api
        results = gender(image_base64)
        if (results['gender'][0]['Male'] >= results['gender'][0]['Female']):
            df_tmp['gender'][pic] = 'male'
        else:
            df_tmp['gender'][pic] = 'female'

        if df_tmp['gender'][pic] == 'male':
            if (os.path.exists(gender_path + 'male/') == False):
                os.mkdir(gender_path + 'male/')
            shutil.copyfile(imgName, gender_path + 'male/' + pic)
        elif df_tmp['gender'][pic] == 'female':
            if (os.path.exists(gender_path + 'female/') == False):
                os.mkdir(gender_path + 'female/')
            shutil.copyfile(imgName, gender_path + 'female/' + pic)

## age api
        results = age(image_base64)
        df_tmp['age'][pic] = results['age'][0]
        shutil.copyfile(imgName, age_path + str(results['age'][0]) + '-' + pic)

## hair api
        results = hair(image_base64)
        df_tmp['hair'][pic] = results['hair'][0]

        if results['hair'][0] == 'shortHair':
            if (os.path.exists(hair_path + 'shortHair/') == False):
                os.mkdir(hair_path + 'shortHair/')
            shutil.copyfile(imgName, hair_path + 'shortHair/' + pic)
        elif results['hair'][0] == 'longHair':
            if (os.path.exists(hair_path + 'longHair/') == False):
                os.mkdir(hair_path + 'longHair/')
            shutil.copyfile(imgName, hair_path + 'longHair/' + pic)
        elif results['hair'][0] == 'bald':
            if (os.path.exists(hair_path + 'bald/') == False):
                os.mkdir(hair_path + 'bald/')
            shutil.copyfile(imgName, hair_path + 'bald/' + pic)

        df = df_tmp
    print df
    #     print df.describe
    # pandas_df_to_markdown_table(df)
    df.to_csv('data-label.csv', sep='|')
