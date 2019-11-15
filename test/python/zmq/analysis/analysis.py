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
import commands


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
    true_cnt_sideFace = 0
    true_cnt_score = 0
    true_cnt_glasses = 0
    true_cnt_quality = 0
    true_cnt_hair = 0
    true_cnt_gender = 0
    total = 0
    context = zmq.Context()
    socket = context.socket(zmq.DEALER)
    ip = "192.168.1.11"
    url = "tcp://" + ip + ":5559"
    socket.connect(url)
    print 'connect ok'
    data = {'interface': '5', 'api_key': '', 'image_base64': ""}
    os.system('tar -zxf analysis-dataset.tar.gz')
    pics_dir = commands.getoutput('pwd') + '/analysis-dataset/'
    df = pd.read_csv(pics_dir + '../data.csv', index_col='name')
    os.chdir(pics_dir)
    for pic in glob.glob("*.jpg"):
        total = total + 1
        print total
        imgName = pics_dir + pic
        fp = open(imgName, 'rb')
        image_base64 = base64.b64encode(fp.read())
        fp.close()

        ## detect api
        results = detect(image_base64)
        if results['error_message'] == '602':
            print pic
        elif results['error_message'] == '601':
            if (df['score'][pic] == '0.95+'
                    and round(results['result'][0]['score'], 5) >= 0.95):
                true_cnt_score = true_cnt_score + 1
            elif (df['score'][pic] == '0.95-'
                  and round(results['result'][0]['score'], 5) < 0.95):
                true_cnt_score = true_cnt_score + 1

            if (df['sideface'][pic] == 1.0
                    and results['result'][0]['sideFace'] == 1.0):
                true_cnt_sideFace = true_cnt_sideFace + 1
            elif (df['sideface'][pic] == 0.0
                  and results['result'][0]['sideFace'] == 0.0):
                true_cnt_sideFace = true_cnt_sideFace + 1
            if (df['glasses'][pic] == 1.0
                    and results['result'][0]['glasses'] == 1.0):
                true_cnt_glasses = true_cnt_glasses + 1
            elif (df['glasses'][pic] == 0.0
                  and results['result'][0]['glasses'] == 0.0):
                true_cnt_glasses = true_cnt_glasses + 1

            if (df['quality'][pic] == 1.0
                    and results['result'][0]['quality'] == 1.0):
                true_cnt_quality = true_cnt_quality + 1
            elif (df['quality'][pic] == 0.0
                  and results['result'][0]['quality'] == 0.0):
                true_cnt_quality = true_cnt_quality + 1

        ## gender api
        results = gender(image_base64)
        if (df['gender'][pic] == 'male' and results['gender'][0]['Male'] >=
                results['gender'][0]['Female']):
            true_cnt_gender = true_cnt_gender + 1
        elif (df['gender'][pic] == 'female' and
              results['gender'][0]['Male'] < results['gender'][0]['Female']):
            true_cnt_gender = true_cnt_gender + 1

        # ## age api
        #         results = age(image_base64)
        #         df_tmp['age'][pic] = results['age'][0]
        #         shutil.copyfile(imgName, age_path + str(results['age'][0]) + '-' + pic)

        ## hair api
        results = hair(image_base64)
        if (df['hair'][pic] == 'shortHair'
                and results['hair'][0] == 'shortHair'):
            true_cnt_hair = true_cnt_hair + 1
        elif (df['hair'][pic] == 'longHair'
              and results['hair'][0] == 'longHair'):
            true_cnt_hair = true_cnt_hair + 1
        elif (df['hair'][pic] == 'bald' and results['hair'][0] == 'bald'):
            true_cnt_hair = true_cnt_hair + 1
    # accuracy
    accuarcy_sideface = true_cnt_sideFace / float(total) * 100
    accuarcy_score = true_cnt_score / float(total) * 100
    accuarcy_quality = true_cnt_quality / float(total) * 100
    accuarcy_glasses = true_cnt_glasses / float(total) * 100
    accuarcy_gender = true_cnt_gender / float(total) * 100
    accuarcy_hair = true_cnt_hair / float(total) * 100

    print 'accuarcy_sideface: ', accuarcy_sideface, '%'
    print 'accuarcy_score: ', accuarcy_score, '%'
    print 'accuarcy_quality: ', accuarcy_quality, '%'
    print 'accuarcy_glasses: ', accuarcy_glasses, '%'
    print 'accuarcy_gender: ', accuarcy_gender, '%'
    print 'accuarcy_hair: ', accuarcy_hair, '%'

    os.system('rm -rf ../analysis-dataset')