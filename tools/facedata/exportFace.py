#! /usr/bin/python
import base64
import os
import sqlite3
import thread


def export_face_data(database, table, store_path):
    conn = sqlite3.connect(database)
    cur = conn.cursor()
    sqlStatement = "SELECT user_id, group_id, image_1, image_2,image_3,image_4,image_5 from " + \
        str(table)
    cur.execute(sqlStatement)
    for image_info in cur:
        storeFaceImage(store_path,[image_info])
    conn.close()


def storeFaceImage(store_path, images_info):
    for image_info in images_info:
        user_id = image_info[0]
        group_id = image_info[1]
        base64_images = image_info[2:6]
        store_dir = store_path + "/images/" + group_id + "/" + user_id
        if not os.path.exists(store_dir):
            os.makedirs(store_dir)
        i = 0
        for base64_image in base64_images:
            if len(base64_image) > 0:
                filename = store_dir + "/" + str(i) + ".jpg"
                fo = open(filename, "wb")
                fo.write(base64.b64decode(base64_image))
                fo.close()
                i = i + 1


def main(database, table, store_dir):
    export_face_data(database, table, store_dir)
    print "export image done!"


def help():
    print "./exportFace.py [database path] [table name] [path to store images]"


if __name__ == '__main__':
    from sys import argv
    if len(argv) == 4:
        main(argv[1], argv[2], argv[3])
    else:
        help()
