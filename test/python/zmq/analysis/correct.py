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

if __name__ == '__main__':
    pics = []
    # label = [
    #     'gender', 'sideface', 'score', 'glasses', 'quality', 'hair', 'age'
    # ]
    label = [
        'name', 'sideface', 'score', 'glasses', 'quality', 'gender', 'hair'
    ]

    df = pd.DataFrame(columns=label, index=pics)

    pics_dir = "/home/wdk/tmp/test-1000-pics/"
    sideface_path = pics_dir + label[1] + '/'
    score_path = pics_dir + label[2] + '/'
    glasses_path = pics_dir + label[3] + '/'
    quality_path = pics_dir + label[4] + '/'
    gender_path = pics_dir + label[5] + '/'
    hair_path = pics_dir + label[6] + '/'
    # age_path = pics_dir + label[7] + '/'
    paths = [
        sideface_path, score_path, glasses_path, quality_path, gender_path,
        hair_path
    ]
    flag = 0 
    for path in paths:
        for sub_dir in (os.listdir(path)):
            os.chdir(path + sub_dir)
            for pic in glob.glob("*.jpg"):
                if flag == 0:
                    pics.append(pic)
                df_tmp = df.reindex(pics)
                df_tmp['name'][pic] = pic
                if path == sideface_path:
                    df_tmp['sideface'][pic] = sub_dir
                elif path == score_path:
                    df_tmp['score'][pic] = sub_dir
                elif path == glasses_path:
                    df_tmp['glasses'][pic] = sub_dir
                elif path == quality_path:
                    df_tmp['quality'][pic] = sub_dir
                elif path == gender_path:
                    df_tmp['gender'][pic] = sub_dir
                elif path == hair_path:
                    df_tmp['hair'][pic] = sub_dir
                df = df_tmp
        flag = 1
    print df
    os.chdir(pics_dir)
    # pandas_df_to_markdown_table(df)
    df.to_csv('data.csv')
