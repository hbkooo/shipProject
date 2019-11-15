#!/usr/bin/env python

# --------------------------------------------------------
# Faster R-CNN
# Copyright (c) 2015 Microsoft
# Licensed under The MIT License [see LICENSE for details]
# Written by Ross Girshick
# --------------------------------------------------------

"""
Demo script showing detections in sample images.

See README.md for installation instructions before running.
"""
import os.path as osp
import sys

def add_path(path):
    if path not in sys.path:
        sys.path.insert(0, path)

this_dir = osp.dirname(__file__)

# Add caffe to PYTHONPATH
caffe_path = osp.join(this_dir, 'caffe_fast_rcnn', 'python')
add_path(caffe_path)

# Add lib to PYTHONPATH
lib_path = osp.join(this_dir, 'lib')
add_path(lib_path)


from fast_rcnn.config import cfg
from rotation.rotate_polygon_nms import rotate_gpu_nms
from utils.timer import Timer
import matplotlib.pyplot as plt
import numpy as np
import scipy.io as sio
import caffe, os, cv2
import argparse
import math
from rotation.data_extractor import get_rroidb, test_rroidb, get_MSRA
#reload(cv2)
# from eval.MSRA_eval import eval as MSRA_eval
from rotation.rt_test import r_im_detect
from rotation.merge_box import merge
import rotation.rt_test_crop as rt_crop
from rotation.data_pick import vis_image




CLASSES = ('__background__',
           'warship',
           'civil-ship',
           'submarine',
           'aircraft-carrier')

def demo(net, image_name, conf = 0.1):
    """Detect object classes in an image using pre-computed object proposals."""

    # Load the demo image
    im_file = image_name
    im = cv2.imread(im_file)

    #print(type(im),im.shape)
    im.resize((400,300,3))
    #print(type(im),im.shape)

    im_height = im.shape[0]
    im_width = im.shape[1]

    # Detect all object classes and regress object bounds
    timer = Timer()
    timer.tic()
    scores, boxes = r_im_detect(net, im)

    timer.toc()
    # print ('Detection took {:.3f}s for '
    #        '{:d} object proposals').format(timer.total_time, boxes.shape[0])

    # Visualize detections for each class
    CONF_THRESH = conf
    NMS_THRESH = 0.3

    ####################################
    ### formate: [x_ctr  y_ctr  w  h  an  score  class]
    predections = np.empty((0,7))         # the last all classes result
    #######################################

    for cls_ind, cls in enumerate(CLASSES[1:]):
        cls_ind += 1 # because we skipped background
        cls_boxes = boxes[:, 5*cls_ind:5*(cls_ind + 1)] # D  (300,5)
        cls_scores = scores[:, cls_ind]                 # (300,)   cls_scores[:, np.newaxis]  ===>  (300,1)
        dets = np.hstack((cls_boxes,
                          cls_scores[:, np.newaxis])).astype(np.float32)   # (300,5) + (300,1) ===> (300,6)

        keep = rotate_gpu_nms(dets, NMS_THRESH) # D
        dets = dets[keep, :]                              # only one class

        ############################ add the other axis to contain the class label  ####################
        ###############   (n,6)   ===>   (n,7)      [ box(5) , score ]   ===>   [ box(5) , score , class ]

        label = np.ones_like(keep)
        dets = np.hstack((dets,label[:,np.newaxis]*cls_ind))

        ########################### contact with different class #######################################
        ##############  (n1,7) + (n2,7) + (n3,7) + (n4,7)   ===>   (n1+n2+n3+n4,7)
        predections = np.vstack((predections,dets))

    order = predections[:,5] > 0.5
    predections = predections[order]    # get score > 0.5
    dets[:, 2] = dets[:, 2] / cfg.TEST.GT_MARGIN
    dets[:, 3] = dets[:, 3] / cfg.TEST.GT_MARGIN
    predections[:, 2] = predections[:, 2] / cfg.TEST.GT_MARGIN
    predections[:, 3] = predections[:, 3] / cfg.TEST.GT_MARGIN
    pre = predections[:,0:6]

    return timer.total_time

def parse_args():
    """Parse input arguments."""
    parser = argparse.ArgumentParser(description='Faster R-CNN demo')
    parser.add_argument('--gpu', dest='gpu_id', help='GPU device id to use [0]',
                        default=0, type=int)
    parser.add_argument('--cpu', dest='cpu_mode',
                        help='Use CPU mode (overrides --gpu)',
                        action='store_true')

    args = parser.parse_args()

    return args

if __name__ == '__main__':

    cfg.TEST.HAS_RPN = True  # Use RPN for proposals

    args = parse_args()


    prototxt = '../../model/ship.prototxt'
    caffemodel = '../../model/ship.caffemodel'

    print "prototxt",prototxt
    print "caffemodel",caffemodel

    if not os.path.isfile(caffemodel):
        raise IOError(('{:s} not found.\n').format(caffemodel))

    if args.cpu_mode:
        caffe.set_mode_cpu()
    else:
        caffe.set_mode_gpu()
        caffe.set_device(args.gpu_id)
        cfg.GPU_ID = args.gpu_id

    net = caffe.Net(prototxt, caffemodel, caffe.TEST)

    print '\n\nLoaded network {:s}'.format(caffemodel)

    # Warmup on a dummy image
    im = 128 * np.ones((300, 500, 3), dtype=np.uint8)
    for i in xrange(2):
        _, _= r_im_detect(net, im)

    im_names = []
    gt_boxes = []

    demo_dir = './data'
    for img in os.listdir(demo_dir):
        im_names.append(os.path.join(demo_dir, img))
        gt_boxes.append([0, 0, 0, 0, 0])

    # The detection results will save in cood_dir in txt
    cood_dir = "./result"


    # for im_idx in range(len(im_names)):
    #     print '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~'
    #     print 'Demo for data/demo/{}'.format(im_names[im_idx])
    #     demo(net, im_names[im_idx], gt_boxes[im_idx], cood_dir)

    times = 0
    iter = 300

    img_path = "./data/ship1.jpg"
    for i in range(iter):
        time = demo(net,img_path)
        times = times + time
        print('{} image took {}s'.format(i, time))

    print('detect {} images use times {}s, average time {}s'.format(iter,times,times/iter))



