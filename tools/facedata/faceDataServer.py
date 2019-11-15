#!/usr/bin/python

import base64
import os
import random
import SocketServer
import thread
import time
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer

import simplejson


class S(BaseHTTPRequestHandler):
    def _set_headers(self):
        self.send_response(200)
        self.send_header('Content-Type', 'application/json')
        self.end_headers()

    def do_HEAD(self):
        self._set_headers()

    def do_POST(self):
        self.data_string = self.rfile.read(int(self.headers['Content-Length']))

        data = simplejson.loads(self.data_string)
        # print data
        thread.start_new_thread(processRequestData, (data,))
        returnData = simplejson.dumps({"request_id": data["request_id"]})
        self._set_headers()
        self.wfile.write(returnData)
        return


def processRequestData(jsonData):
    log(jsonData["request_id"])
    base64_images = (jsonData["image_1"], jsonData["image_2"],
                     jsonData["image_3"], jsonData["image_4"], jsonData["image_5"])
    storeImage(jsonData["user_id"], jsonData["group_id"],
               jsonData["interface"], base64_images)


def log(data):
    log_path = "logs/"
    if not os.path.exists(log_path):
        os.makedirs(log_path)
    filename = log_path + time.strftime("%Y_%m_%d", time.localtime()) + ".txt"
    data = time.strftime("%Y_%m_%d_%H_%M_%S",
                         time.localtime()) + " : " + str(data)
    fo = open(filename, "a")
    fo.write(data + '\n')
    fo.close()


def storeImage(user_id, group_id, interface, base64_images):
    i = 0
    store_path = "images/" + \
        str(interface) + \
        "/" + \
        time.strftime("%Y%m%d", time.localtime()) + \
        "/" + \
        str(group_id) + \
        "/" + \
        str(user_id)
    if not os.path.exists(store_path):
        os.makedirs(store_path)
    for base64_image in base64_images:
        if len(base64_image) > 0:
            filename = store_path + "/" + \
                time.strftime("%H%M%S", time.localtime()) + \
                    str(i) + ".jpg"
            fo = open(filename, "wb")
            fo.write(base64.b64decode(base64_image))
            fo.close()
            i = i + 1


def run(server_class=HTTPServer, handler_class=S, port=8989):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print 'Starting http server...'
    httpd.serve_forever()


if __name__ == "__main__":
    from sys import argv

    if len(argv) == 2:
        run(port=int(argv[1]))
    else:
        run()
