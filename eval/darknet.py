#! /usr/bin/python
# encoding: utf-8
from ctypes import *
import os
import os.path as osp
import shutil

from eval.voc_eval import voc_eval

#############################################################################
#############################################################################
#############################################################################
darknetSo = '../deps/yolo/libdarknet.so'
cfg = '../model/yolo.cfg'
weights = '../model/yolo.weights'
dataPath = '../model/yolo.data'

# /home/hbk/data/damage/JPEGImages/2138.jpg
# ...
image_list_file = 'testData/voc/test.txt'

xmlRoot = 'testData/voc/Annotations'

save_result_root = 'results'
#############################################################################
#############################################################################
#############################################################################

class BOX(Structure):
    _fields_ = [("x", c_float),
                ("y", c_float),
                ("w", c_float),
                ("h", c_float)]


class DETECTION(Structure):
    _fields_ = [("bbox", BOX),
                ("classes", c_int),
                ("prob", POINTER(c_float)),
                ("mask", POINTER(c_float)),
                ("objectness", c_float),
                ("sort_class", c_int)]


class IMAGE(Structure):
    _fields_ = [("w", c_int),
                ("h", c_int),
                ("c", c_int),
                ("data", POINTER(c_float))]


class METADATA(Structure):
    _fields_ = [("classes", c_int),
                ("names", POINTER(c_char_p))]


class YOLO(object):

    def __init__(self, libPath='libdarknet.so', encodingStr='utf-8'):
        self.encodingStr = encodingStr
        self.lib = CDLL(libPath, RTLD_GLOBAL)
        self.lib.network_width.argtypes = [c_void_p]
        self.lib.network_width.restype = c_int
        self.lib.network_height.argtypes = [c_void_p]
        self.lib.network_height.restype = c_int

        self.predict = self.lib.network_predict
        self.predict.argtypes = [c_void_p, POINTER(c_float)]
        self.predict.restype = POINTER(c_float)

        self.set_gpu = self.lib.cuda_set_device
        self.set_gpu.argtypes = [c_int]

        self.make_image = self.lib.make_image
        self.make_image.argtypes = [c_int, c_int, c_int]
        self.make_image.restype = IMAGE

        self.get_network_boxes = self.lib.get_network_boxes
        self.get_network_boxes.argtypes = [c_void_p, c_int, c_int, c_float, c_float, POINTER(c_int), c_int,
                                           POINTER(c_int)]
        self.get_network_boxes.restype = POINTER(DETECTION)

        self.make_network_boxes = self.lib.make_network_boxes
        self.make_network_boxes.argtypes = [c_void_p]
        self.make_network_boxes.restype = POINTER(DETECTION)

        self.free_detections = self.lib.free_detections
        self.free_detections.argtypes = [POINTER(DETECTION), c_int]

        self.free_ptrs = self.lib.free_ptrs
        self.free_ptrs.argtypes = [POINTER(c_void_p), c_int]

        self.network_predict = self.lib.network_predict
        self.network_predict.argtypes = [c_void_p, POINTER(c_float)]

        self.reset_rnn = self.lib.reset_rnn
        self.reset_rnn.argtypes = [c_void_p]

        self.load_net = self.lib.load_network
        self.load_net.argtypes = [c_char_p, c_char_p, c_int]
        self.load_net.restype = c_void_p

        self.do_nms_obj = self.lib.do_nms_obj
        self.do_nms_obj.argtypes = [POINTER(DETECTION), c_int, c_int, c_float]

        self.do_nms_sort = self.lib.do_nms_sort
        self.do_nms_sort.argtypes = [POINTER(DETECTION), c_int, c_int, c_float]

        self.free_image = self.lib.free_image
        self.free_image.argtypes = [IMAGE]

        self.letterbox_image = self.lib.letterbox_image
        self.letterbox_image.argtypes = [IMAGE, c_int, c_int]
        self.letterbox_image.restype = IMAGE

        self.load_meta = self.lib.get_metadata
        self.lib.get_metadata.argtypes = [c_char_p]
        self.lib.get_metadata.restype = METADATA

        self.load_image = self.lib.load_image_color
        self.load_image.argtypes = [c_char_p, c_int, c_int]
        self.load_image.restype = IMAGE

        self.rgbgr_image = self.lib.rgbgr_image
        self.rgbgr_image.argtypes = [IMAGE]

        self.predict_image = self.lib.network_predict_image
        self.predict_image.argtypes = [c_void_p, IMAGE]
        self.predict_image.restype = POINTER(c_float)

    def initNet(self, cfgPath, weightsPath, dataPath):
        if not isinstance(cfgPath, bytes):
            cfgPath = bytes(cfgPath, encoding=self.encodingStr)
        if not isinstance(weightsPath, bytes):
            weightsPath = bytes(weightsPath, encoding=self.encodingStr)
        if not isinstance(dataPath, bytes):
            dataPath = bytes(dataPath, encoding=self.encodingStr)

        self.net = self.load_net(cfgPath, weightsPath, 0)
        self.meta = self.load_meta(dataPath)

    # return : [ label score xc yc w h ]
    def detect(self, image, thresh=.5, hier_thresh=.5, nms=.45):
        """
        detect one image
        :param image: image path
        :param thresh: detect thresh
        :param hier_thresh:
        :param nms:
        :return: detect object results
        """

        if not hasattr(self, 'net'):
            print("\nerror : please init the network first by using "
                  "\"YOLO().initNet(cfgPath, weightsPath, dataPath)\"\n")
            return []

        if not isinstance(image, bytes):
            print('decoding image path')
            image = bytes(image, encoding=self.encodingStr)

        im = self.load_image(image, 0, 0)
        if im is None:
            print('[info]: {} is none ...'.format(image))
            return []
        num = c_int(0)
        pnum = pointer(num)
        self.predict_image(self.net, im)
        dets = self.get_network_boxes(self.net, im.w, im.h, thresh, hier_thresh, None, 0, pnum)
        num = pnum[0]
        if nms:
            self.do_nms_obj(dets, num, self.meta.classes, nms);

        res = []
        for j in range(num):
            for i in range(self.meta.classes):
                if dets[j].prob[i] > 0:
                    b = dets[j].bbox
                    label = self.meta.names[i]
                    if isinstance(label, bytes):
                        label = label.decode(self.encodingStr)
                    res.append((label, dets[j].prob[i], (b.x, b.y, b.w, b.h)))

        res = sorted(res, key=lambda x: -x[1])
        self.free_image(im)
        self.free_detections(dets, num)
        return res

    def classify(self, image):

        if not isinstance(image, bytes):
            print('decoding image path')
            image = bytes(image, encoding=self.encodingStr)

        img = self.load_image(image, 0, 0)
        out = self.predict_image(self.net, img)
        res = []
        for i in range(self.meta.classes):
            res.append((self.meta.names[i], out[i]))
        res = sorted(res, key=lambda x: -x[1])
        return res


def saveImageName(image_list_file):
    """
    image_list_file : '/home/hbk/data/damage/val.txt'
        /home/hbk/data/damage/JPEGImages/2137.jpg
        /home/hbk/data/damage/JPEGImages/2138.jpg
        /home/hbk/data/damage/JPEGImages/2139.jpg
        ...
    
    return:
        savePath
    """
    # 2137
    # 2138
    # 2139
    # ...
    savePath = osp.join(osp.dirname(image_list_file), 'name_'+osp.basename(image_list_file))
    with open(savePath, 'w') as fp:
        lines = open(image_list_file).readlines()
        for line in lines:
            line = line.strip()
            fp.write('{}\n'.format(osp.basename(line)[:-4]))
    
    return savePath
            

def evalMap(save_result_root, xmlRoot, savePath):
    """
        save_result_root: root dir should only contain labels.txt. Such as: person.txt, car.txt, mobilephone.txt ...
        xmlRoot: should contain ground truth xml files.
        savePath: image list file, and the file contains only image name per line.
    """

    print("***************************")
    print("***************************")
    
    mAP = []
    for class_name_file in os.listdir(save_result_root):
    
        class_name = class_name_file[:-4]
        rec, prec, ap = voc_eval(save_result_root + '/{}.txt',
                                 xmlRoot + '/{}.xml',
                                 savePath,
                                 class_name,
                                 '.')
        print("{} :\t {} ".format(class_name, ap))
        mAP.append(ap)

    mAP = tuple(mAP)

    print("***************************")
    print("mAP :\t {}".format( float( sum(mAP)/len(mAP)) ))
    
    print("***************************")
    print("***************************")

    os.remove('annots.pkl')

if __name__ == '__main__':
    
    if not osp.exists(save_result_root):
        os.mkdir(save_result_root)
    
    for cache in os.listdir(save_result_root):
        os.remove(osp.join(save_result_root, cache))

    yolo = YOLO(darknetSo)
    yolo.initNet(cfg, weights, dataPath)
    
    savePath = saveImageName(image_list_file)
    
    images = open(image_list_file).readlines()
    
    fps = {}
    results = []
    
    for image in images:
        image = image.strip()
        print('[info]: start detect {}'.format(image))
        # [ label score x y w h ]
        result = yolo.detect(image)
        
        for line in result:
        
            if line[0] not in fps:
                fp = open(osp.join(save_result_root, line[0]+'.txt'), 'w')
                fps[line[0]] = fp
            else:
                fp = fps[line[0]]
        
            x,y,w,h = line[2]
            
            # [ image_name score xmin ymin xmax ymax ]
            fp.write('{} {} {} {} {} {}\n'.format(osp.basename(image)[0:-4], str(line[1]), str(x-w/2), str(y-h/2), str(x+w/2),str(y+h/2)))
    
    for name in fps:
        fps[name].close()
    
    evalMap(save_result_root, xmlRoot, savePath)
    
    
    
    
    
    
    
    
    
    
