import os
import os.path as osp
import sys
import numpy as np
import cv2

#############################################################################
#############################################################################
# Make sure that caffe is on the python path:
caffe_root = '../deps/caffe_ship/caffe_fast_rcnn/'
rotation_lib = '../deps/caffe_ship/lib/'
DOTA_devkit = 'DOTA_devkit/'
#############################################################################
#############################################################################

sys.path.insert(0, DOTA_devkit)
sys.path.insert(0, rotation_lib)
sys.path.insert(0, os.path.join(caffe_root, 'python'))
import caffe
from rotation.rt_test import r_im_detect
from rotation.rotate_polygon_nms import rotate_gpu_nms
from eval.ship_eval import voc_eval

#############################################################################
#############################################################################
#############################################################################
# load model
prototxt = '../model/ship.prototxt'
caffemodel = '../model/ship.caffemodel'

# /home/hbk/data/damage/JPEGImages/2138.jpg
# ...
image_list_file = 'testData/ship/test.txt'
annoRoot = 'testData/ship/Annotations'

save_result_root = 'results'
#############################################################################
#############################################################################
#############################################################################


class ShipModel(object):
    def __init__(self, prototxt, caffemodel):
        
        self.net = caffe.Net(prototxt, caffemodel, caffe.TEST)
        print('[info]: load model done ...')
        
        self.classnames = ('__background__',
                               'warship',
                               'civil-ship',
                               'submarine',
                               'aircraft-carrier')
    
    def detect(self, image_file, thresh=0.5):
        """
        return format: [ label score x1 y1 x2 y2 x3 y3 x4 y4 ]
        """
        
        # Load the demo image
        im = cv2.imread(image_file)

        im_height = im.shape[0]
        im_width = im.shape[1]

        # Detect all object classes and regress object bounds
        scores, boxes = r_im_detect(self.net, im)

        NMS_THRESH = 0.3
        
        ####################################
        ### formate: [x_ctr  y_ctr  w  h  an  score  class]
        predections = np.empty((0,7))         # the last all classes result
        #######################################
        
        for cls_ind, cls in enumerate(self.classnames[1:]):
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
        
        order = predections[:,5] > thresh
        predections = predections[order]    # get score > 0.5
        
        dets[:, 2] = dets[:, 2] / 1.4
        dets[:, 3] = dets[:, 3] / 1.4
        predections[:, 2] = predections[:, 2] / 1.4
        predections[:, 3] = predections[:, 3] / 1.4
        pre = predections[:,0:6]
        
        results = self.formatResult(predections, thresh, im_height, im_width)
        
        # [ label score x1 y1 x2 y2 x3 y3 x4 y4 ]
        return results
    
    def formatResult(self, predections, threshold, height, width):  
        """
        return format: [ label score x1 y1 x2 y2 x3 y3 x4 y4 ]
        """
        return_bboxes = []
        for idx in range(len(predections)):
            cx,cy,h,w,angle = predections[idx][0:5]
            lt = [cx - w/2, cy - h/2,1]
            rt = [cx + w/2, cy - h/2,1]
            lb = [cx - w/2, cy + h/2,1]
            rb = [cx + w/2, cy + h/2,1]

            pts = []
            pts.append(lt)
            pts.append(rt)
            pts.append(rb)
            pts.append(lb)

            angle = -angle

            cos_cita = np.cos(np.pi / 180 * angle)
            sin_cita = np.sin(np.pi / 180 * angle)

            M0 = np.array([[1,0,0],[0,1,0],[-cx,-cy,1]])
            M1 = np.array([[cos_cita, sin_cita,0], [-sin_cita, cos_cita,0],[0,0,1]])
            M2 = np.array([[1,0,0],[0,1,0],[cx,cy,1]])
            rotation_matrix = M0.dot(M1).dot(M2)

            rotated_pts = np.dot(np.array(pts), rotation_matrix)

            det_str = str(int(rotated_pts[0][0])) + "," + str(int(rotated_pts[0][1])) + "," + \
                str(int(rotated_pts[1][0])) + "," + str(int(rotated_pts[1][1])) + "," + \
                str(int(rotated_pts[2][0])) + "," + str(int(rotated_pts[2][1])) + "," + \
                str(int(rotated_pts[3][0])) + "," + str(int(rotated_pts[3][1])) + "\r\n"

            if (predections[idx][5] > threshold):
                rotated_pts = self.over_bound_handle(rotated_pts, height, width)
                # [ label score x1 y1 x2 y2 x3 y3 x4 y4 ]
                return_bboxes.append([ self.classnames[int(predections[idx][6])], predections[idx][5], int(rotated_pts[0][0]), int(rotated_pts[0][1]),
                                        int(rotated_pts[1][0]), int(rotated_pts[1][1]),
                                        int(rotated_pts[2][0]), int(rotated_pts[2][1]),
                                        int(rotated_pts[3][0]), int(rotated_pts[3][1]) ])
        return return_bboxes
    
    def over_bound_handle(self, pts, img_height, img_width):

        pts[np.where(pts < 0)] = 1

        pts[np.where(pts[:,0] > img_width), 0] = img_width-1
        pts[np.where(pts[:,1] > img_height), 1] = img_height-1

        return pts
    

def saveImageName(image_list_file):
    """
    image_list_file : '/home/hbk/data/damage/val.txt'
        /home/hbk/data/damage/JPEGImages/2137.jpg
        /home/hbk/data/damage/JPEGImages/2138.jpg
        /home/hbk/data/damage/JPEGImages/2139.jpg
        ...
    
    return:
        imagesetfile
    """
    # 2137
    # 2138
    # 2139
    # ...
    imagesetfile = osp.join(osp.dirname(image_list_file), 'name_'+osp.basename(image_list_file))
    with open(imagesetfile, 'w') as fp:
        lines = open(image_list_file).readlines()
        for line in lines:
            line = line.strip()
            fp.write('{}\n'.format(osp.basename(line)[:-4]))
    
    return imagesetfile
    

def evalMap(save_result_root, annoRoot, imagesetfile):
    """
        save_result_root: root dir should only contain labels.txt. Such as: person.txt, car.txt, mobilephone.txt ...
        annoRoot: should contain ground truth txt files.
        imagesetfile: image list file, and the file contains only image name per line.
    """

    print("***************************")
    print("***************************")
    
    mAP = []
    for class_name_file in os.listdir(save_result_root):
    
        class_name = class_name_file[:-4]
        rec, prec, ap = voc_eval(save_result_root + '/{:s}.txt',
                                 annoRoot + '/{:s}.txt',
                                 imagesetfile,
                                 class_name,
                                 '.')
        print("{} :\t {} ".format(class_name, ap))
        mAP.append(ap)

    mAP = tuple(mAP)

    print("***************************")
    print("mAP :\t {}".format( float( sum(mAP)/len(mAP)) ))
    
    print("***************************")
    print("***************************")
    
    if osp.exists('ship_annots.pkl'):
        os.remove('ship_annots.pkl')


if __name__ == '__main__':
    
    caffe.set_device(0)
    caffe.set_mode_gpu()
    
    print('save_result_root: ', save_result_root)
    if not osp.exists(save_result_root):
        os.mkdir(save_result_root)
    
    for cache in os.listdir(save_result_root):
        os.remove(osp.join(save_result_root, cache))
    
    ship = ShipModel(prototxt, caffemodel)
    
    imagesetfile = saveImageName(image_list_file)
    images = open(image_list_file).readlines()
    
    fps = {}
    results = []
    
    for image in images:
        image = image.strip()
        print('[info]: start detect {}'.format(image))
        # [ label score x1 y1 x2 y2 x3 y3 x4 y4 ]
        result = ship.detect(image)
        
        for line in result:
            label = line[0]
            if label not in fps:
                fp = open(osp.join(save_result_root, label+'.txt'), 'w')
                fps[label] = fp
            else:
                fp = fps[label]
            #print(line)
            # [ image_name score x1 y1 x2 y2 x3 y3 x4 y4 ]
            fp.write('{} {} {}\n'.format(osp.basename(image)[0:-4], str(line[1]), ' '.join([str(b) for b in line[2:]])))
    
    for name in fps:
        fps[name].close()
    
    evalMap(save_result_root, annoRoot, imagesetfile)
    
        
       
        