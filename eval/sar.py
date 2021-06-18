import os
import os.path as osp
import sys
import numpy as np
from eval.voc_eval import voc_eval

current_root = os.getcwd()
#############################################################################
#############################################################################
# Make sure that caffe is on the python path:
caffe_root = '../deps/caffe_ship/caffe_fast_rcnn/'
#############################################################################
#############################################################################

sys.path.insert(0, os.path.join(caffe_root, 'python'))
import caffe

#############################################################################
#############################################################################
#############################################################################
# load model
prototxt = '../model/sar.prototxt'
caffemodel = '../model/sar.caffemodel'

# ship
# plane
labelFile = '../model/sar.names'

# /home/hbk/data/damage/JPEGImages/2138.jpg
# ...
image_list_file = 'testData/sar/test.txt'
xmlRoot = 'testData/sar/Annotations'

save_result_root = 'results'
#############################################################################
#############################################################################
#############################################################################


class SARModel(object):
    def __init__(self, prototxt, caffemodel, labelFile):
        
        self.net = caffe.Net(prototxt, caffemodel, caffe.TEST)
        print('[info]: load model done ...')
        
        classnames = open(labelFile).readlines()
        self.classnames = [c.strip() for c in classnames]
        
        img_resize = 512
        self.net.blobs['data'].reshape(1, 3, img_resize, img_resize)
        self.transformer = caffe.io.Transformer({'data': self.net.blobs['data'].data.shape})
        self.transformer.set_transpose('data', (2, 0, 1))
        self.transformer.set_mean('data', np.array([104, 117, 123]))  # mean pixel
        self.transformer.set_raw_scale('data', 255)  # the reference model operates on images in [0,255] range instead of [0,1]
        self.transformer.set_channel_swap('data', (2, 1, 0))  # the reference model has channels in BGR order instead of RGB
        pass
    
    def detect(self, image_file, thresh=0.5):
        """
        return format: [ label score xmin ymin xmax ymax ]
        """
        image = caffe.io.load_image(image_file)
        transformed_image = self.transformer.preprocess('data', image)
        self.net.blobs['data'].data[...] = transformed_image

        detections = self.net.forward()['detection_out']
        det_label = detections[0, 0, :, 1]
        det_conf = detections[0, 0, :, 2]
        det_xmin = detections[0, 0, :, 3] * image.shape[1]
        det_ymin = detections[0, 0, :, 4] * image.shape[0]
        det_xmax = detections[0, 0, :, 5] * image.shape[1]
        det_ymax = detections[0, 0, :, 6] * image.shape[0]
        result = np.column_stack([det_label, det_conf, det_xmin, det_ymin, det_xmax, det_ymax])
        
        # [ label score xmin ymin xmax ymax ]
        results = []
        for res in result:
            # print(res)
            if thresh and res[1] < thresh:
                continue
            
            results.append([ self.classnames[int(res[0])-1], res[1], res[2], res[3], res[4], res[5] ])
        
        return results


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
    
    caffe.set_device(0)
    caffe.set_mode_gpu()
    
    print('save_result_root: ', save_result_root)
    if not osp.exists(save_result_root):
        os.mkdir(save_result_root)
    
    for cache in os.listdir(save_result_root):
        os.remove(osp.join(save_result_root, cache))
    
    sar = SARModel(prototxt, caffemodel, labelFile)
    
    savePath = saveImageName(image_list_file)
    images = open(image_list_file).readlines()
    
    fps = {}
    results = []
    
    for image in images:
        image = image.strip()
        print('[info]: start detect {}'.format(image))
        # [ label score xmin ymin xmax ymax ]
        result = sar.detect(image)
        
        for line in result:
            label = line[0]
            if label not in fps:
                fp = open(osp.join(save_result_root, label+'.txt'), 'w')
                fps[label] = fp
            else:
                fp = fps[label]
            
            # [ image_name score xmin ymin xmax ymax ]
            fp.write('{} {} {} {} {} {}\n'.format(osp.basename(image)[0:-4], str(line[1]), str(line[2]), str(line[3]), str(line[4]),str(line[5])))
    
    for name in fps:
        fps[name].close()
    
    evalMap(save_result_root, xmlRoot, savePath)
    
        
       
        