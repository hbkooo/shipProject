import random
import shutil
import os
import os.path as osp

# VOC
#root = '/home/hkk/data/VOCdevkit/VOC2007/'
#save_root = '/home/hbk/shipProject/eval/voc/'

# sar
root = '/home/hbk/ImageManTrain/SARDATA/sar/'
save_root = '/home/hbk/shipProject/eval/sar/'

# ship
root_ship = '/home/hbk/ImageManTrain/SHIPDATA/train/'
save_root_ship = '/home/hbk/shipProject/eval/ship/'


def mkdir(path):
    if not osp.exists(path):
        os.mkdir(path)

def processShip():
    
    mkdir(save_root_ship)
    
    images = os.listdir(osp.join(root_ship, 'JPEGImages'))
    random.shuffle(images)
    test_images = images[0:500]
    
    save_img = osp.join(save_root_ship, 'JPEGImages')
    save_xml = osp.join(save_root_ship, 'Annotations')
    img_root = osp.join(root_ship, 'JPEGImages')
    xml_root = osp.join(root_ship, 'labelTxt')
    mkdir(save_img)
    mkdir(save_xml)
    
    f = open(osp.join(save_root_ship, 'test.txt'), 'w')
    count = 0
    
    for img in test_images:
        
        shutil.copy(osp.join(img_root, img), osp.join(save_img, img))
        shutil.copy(osp.join(xml_root, img[:-4]+'.txt'), osp.join(save_xml, img[:-4]+'.txt'))
        f.write('{}\n'.format(osp.join(save_img, img)))
        count += 1
        
    f.close()
    print('copy {} images done...'.format(str(count)))

def process():
    
    mkdir(save_root)
    
    images = os.listdir(osp.join(root, 'JPEGImages'))
    random.shuffle(images)
    test_images = images[0:500]
    
    save_img = osp.join(save_root, 'JPEGImages')
    save_xml = osp.join(save_root, 'Annotations')
    img_root = osp.join(root, 'JPEGImages')
    xml_root = osp.join(root, 'labelTxt')
    mkdir(save_img)
    mkdir(save_xml)
    
    f = open(osp.join(save_root, 'test.txt'), 'w')
    count = 0
    
    for img in test_images:
        
        shutil.copy(osp.join(img_root, img), osp.join(save_img, img))
        shutil.copy(osp.join(xml_root, img[:-4]+'.xml'), osp.join(save_xml, img[:-4]+'.xml'))
        f.write('{}\n'.format(osp.join(save_img, img)))
        count += 1
        
    f.close()
    print('copy {} images done...'.format(str(count)))

# VOC/sar
#process()
    
# ship
processShip()
    