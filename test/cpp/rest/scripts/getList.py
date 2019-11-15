# -*- coding: utf-8 -*-

import os
import sys
import time


def get_file_list(file_dir, image_list_name):
    file_object = open(image_list_name, "w+")
    count = 0
    for root, _, files in os.walk(file_dir, topdown=True, followlinks=True):
        for file in files:
            if count % 10000 is 0:
                print(count)
            count = count + 1
            file_object.write(os.path.relpath(
                os.path.join(root, file), file_dir) + "\n")
    file_object.close()
    print("Total images:" + str(count))


def main():
    if len(sys.argv) is not 3:
        print("Usage: python getList.py [image path] [list name]")
        return
    start = time.clock()
    data_path = str(sys.argv[1])
    image_list_name = str(sys.argv[2])
    get_file_list(data_path, image_list_name)
    elapsed = (time.clock() - start)
    print("Time used:", elapsed)


if __name__ == '__main__':
    main()
