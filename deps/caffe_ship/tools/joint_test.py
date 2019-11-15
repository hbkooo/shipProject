import rotation_demo
from fast_rcnn.config import cfg
import caffe, os, sys, cv2
import numpy as np
from rotation.data_extractor import get_rroidb, test_rroidb, get_MSRA, test_ICDAR2015, get_ICDAR2013_test, get_ICDAR2013_test_ch1
from eval.MSRA_eval import eval as MSRA_eval
from rotation.merge_box import merge_write
from rotation.rotate_polygon_nms import rotate_gpu_nms

def rrpn_test_MSRA(day, exp, model_name, mode):
	
	cfg.TEST.HAS_RPN = True  # Use RPN for proposals
	
	NETS = {'rrpn': ('VGG16', day + "/" + exp + "/" + model_name)}

	# rrpn only
	prototxt = os.path.join(cfg.RRPN_MODELS_DIR, NETS['rrpn'][0],
                            'faster_rcnn_end2end', 'test.prototxt')


	caffemodel = os.path.join(cfg.DATA_DIR, 'faster_rcnn_models',
                              NETS['rrpn'][1])

	print caffemodel

	if not os.path.isfile(caffemodel):
        	raise IOError(('{:s} not found.\nDid you run ./data/script/'
                       'fetch_faster_rcnn_models.sh?').format(caffemodel))

	caffe.set_mode_gpu()
        caffe.set_device(0)
        cfg.GPU_ID = 0

	net = caffe.Net(prototxt, caffemodel, caffe.TEST)

	print '\n\nLoaded network {:s}'.format(caffemodel)

	 # Warmup on a dummy image
    	#im = 128 * np.ones((300, 500, 3), dtype=np.uint8)
    	#for i in xrange(2):
        #	_, _= rotation_demo.r_im_detect(net, im)

	im_names = []#['IMG_0030.JPG','IMG_0059.JPG','IMG_0063.JPG']

    	roidb = test_rroidb(mode)
	
    	gt_boxes = []	
    
	

    	for rdb in roidb:
		im_names.append(rdb['image'])
        	gt_boxes.append(rdb['boxes'])
        
	if not os.path.isdir("./result/" + day + "/"):
		os.mkdir("./result/" + day + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/")

	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/")
	
    	for im_idx in range(len(im_names)):
        	print '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~'
        	print 'Demo for data/demo/{}'.format(im_names[im_idx])
		print "model: ", model_name
		#print im_names[im_idx], gt_boxes[im_idx]
        	rotation_demo.demo(net, im_names[im_idx], gt_boxes[im_idx], "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/", "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/")
	
	
	
	precision, recall = MSRA_eval(mode, "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/")

	f_measure = (2 * precision * recall) / (precision + recall)

	if not os.path.isdir("./exper"):
		os.mkdir("./exper")
	if not os.path.isdir("./exper/" + day + "/"):
		os.mkdir("./exper/" + day + "/")
	if not os.path.isdir("./exper/" + day + "/" + exp + "/"):
		os.mkdir("./exper/" + day + "/" + exp + "/")
	
	PR_file = "./exper/" + day + "/" + exp + "/" + day + "_" + exp + "_" + model_name + "_" + mode
	if not os.path.isfile(PR_file):
		os.mknod(PR_file)

	PR_obj = open(PR_file, "w")

	PR_obj.write("precision: "+ str(precision) + "\n" + "recall: " + str(recall) + "\nf-measure: " + str(f_measure))
    	PR_obj.close()

	
	precision_ori, recall_ori = MSRA_eval(mode, "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/")

	f_measure_ori = (2 * precision_ori * recall_ori) / (precision_ori + recall_ori)

	
	if not os.path.isdir("./exper/" + day + "/" + exp + "_origin/"):
		os.mkdir("./exper/" + day + "/" + exp + "_origin/")
	
	PR_file_ori = "./exper/" + day + "/" + exp + "_origin/" + day + "_" + exp + "_" + model_name + "_" + mode
	if not os.path.isfile(PR_file_ori):
		os.mknod(PR_file_ori)

	PR_obj_ori = open(PR_file_ori, "w")

	PR_obj_ori.write("precision: "+ str(precision_ori) + "\n" + "recall: " + str(recall_ori) + "\nf-measure: " + str(f_measure_ori))
    	PR_obj_ori.close()
	
	print "precision: "+ str(precision_ori) + "\n" + "recall: " + str(recall_ori) + "\nf-measure: " + str(f_measure_ori)



def rrpn_test_ICDAR2015_multiScale(day, exp, model_name, mode):
	
	cfg.TEST.HAS_RPN = True  # Use RPN for proposals
	
	NETS = {'rrpn': ('VGG16', day + "/" + exp + "/" + model_name)}

	# rrpn only
	prototxt = os.path.join(cfg.RRPN_MODELS_DIR, NETS['rrpn'][0],
                            'faster_rcnn_end2end', 'test.prototxt')


	caffemodel = os.path.join(cfg.DATA_DIR, 'faster_rcnn_models',
                              NETS['rrpn'][1])

	print caffemodel

	if not os.path.isfile(caffemodel):
        	raise IOError(('{:s} not found.\nDid you run ./data/script/'
                       'fetch_faster_rcnn_models.sh?').format(caffemodel))

	caffe.set_mode_gpu()
        caffe.set_device(0)
        cfg.GPU_ID = 0

	net = caffe.Net(prototxt, caffemodel, caffe.TEST)

	print '\n\nLoaded network {:s}'.format(caffemodel)

	 # Warmup on a dummy image
    	#im = 128 * np.ones((300, 500, 3), dtype=np.uint8)
    	#for i in xrange(2):
       # 	_, _= rotation_demo.r_im_detect(net, im)

	im_names = []#['IMG_0030.JPG','IMG_0059.JPG','IMG_0063.JPG']

    	roidb = test_ICDAR2015(mode)

    	gt_boxes = []	
    
	

    	for rdb in roidb:
		im_names.append(rdb['image'])
        	gt_boxes.append([0,0,0,0,0])
        
	if not os.path.isdir("./result/" + day + "/"):
		os.mkdir("./result/" + day + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/")

	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/")
	
	scales = [900, 1000]
	max_sizes = [1500, 1700]

	

    	for im_idx in range(len(im_names)):
        	print '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~'
        	print 'Demo for data/demo/{}'.format(im_names[im_idx])
		print "model: ", model_name
		#print im_names[im_idx], gt_boxes[im_idx]

		result_queue = []

		for i in range(len(scales)):
			cfg.TEST.SCALES = (scales[i],)
			cfg.TEST.MAX_SIZE = max_sizes[i]
			print "result for scale", scales[i], ":"
			result = rotation_demo.demo(net, im_names[im_idx], gt_boxes[im_idx], "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/", "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/")
			result_queue.extend(result)

		im_file = im_names[im_idx]
		im = cv2.imread(im_file)

		im_height = im.shape[0]
		im_width = im.shape[1]

		dets = det_fusion(result_queue)
		rotation_demo.write_result_ICDAR(im_names[im_idx], dets, 0.9,"./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/", im_height, im_width)
	
	'''
	precision, recall = MSRA_eval(mode, "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/")

	f_measure = (2 * precision * recall) / (precision + recall)

	if not os.path.isdir("./exper"):
		os.mkdir("./exper")
	if not os.path.isdir("./exper/" + day + "/"):
		os.mkdir("./exper/" + day + "/")
	if not os.path.isdir("./exper/" + day + "/" + exp + "/"):
		os.mkdir("./exper/" + day + "/" + exp + "/")
	
	PR_file = "./exper/" + day + "/" + exp + "/" + day + "_" + exp + "_" + model_name + "_" + mode
	if not os.path.isfile(PR_file):
		os.mknod(PR_file)

	PR_obj = open(PR_file, "w")

	PR_obj.write("precision: "+ str(precision) + "\n" + "recall: " + str(recall) + "\nf-measure: " + str(f_measure))
    	PR_obj.close()


	precision_ori, recall_ori = MSRA_eval(mode, "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/")

	f_measure_ori = (2 * precision_ori * recall_ori) / (precision_ori + recall_ori)

	
	if not os.path.isdir("./exper/" + day + "/" + exp + "_origin/"):
		os.mkdir("./exper/" + day + "/" + exp + "_origin/")
	
	PR_file_ori = "./exper/" + day + "/" + exp + "_origin/" + day + "_" + exp + "_" + model_name + "_" + mode
	if not os.path.isfile(PR_file_ori):
		os.mknod(PR_file_ori)

	PR_obj_ori = open(PR_file_ori, "w")

	PR_obj_ori.write("precision: "+ str(precision_ori) + "\n" + "recall: " + str(recall_ori) + "\nf-measure: " + str(f_measure_ori))
    	PR_obj_ori.close()
	'''

def rrpn_test_ICDAR2015_modelfusion(models, mode):
	
	cfg.TEST.HAS_RPN = True  # Use RPN for proposals
	
	NETS = {'rrpn': ('VGG16', )}

	# rrpn only
	prototxt = os.path.join(cfg.RRPN_MODELS_DIR, NETS['rrpn'][0],
                            'faster_rcnn_end2end', 'test.prototxt')

	results_matrix = []

	roidb = test_ICDAR2015(mode)

	for model in models:

		day = model["day"]
		exp = model["exp"]
		model_name = model["model_name"]

		caffemodel = os.path.join(cfg.DATA_DIR, 'faster_rcnn_models',
                              day + "/" + exp + "/" + model_name)

		print caffemodel

		if not os.path.isfile(caffemodel):
			raise IOError(('{:s} not found.\nDid you run ./data/script/'
		               'fetch_faster_rcnn_models.sh?').format(caffemodel))

		caffe.set_mode_gpu()
		caffe.set_device(0)
		cfg.GPU_ID = 0

		net = caffe.Net(prototxt, caffemodel, caffe.TEST)

		print '\n\nLoaded network {:s}'.format(caffemodel)

		 # Warmup on a dummy image
	    	#im = 128 * np.ones((300, 500, 3), dtype=np.uint8)
	    	#for i in xrange(2):
	       # 	_, _= rotation_demo.r_im_detect(net, im)

		im_names = []#['IMG_0030.JPG','IMG_0059.JPG','IMG_0063.JPG']

	    	cfg.TEST.RATIO_GROUP = model["ratio"]

    		gt_boxes = []	
    
	

	    	for rdb in roidb:
			im_names.append(rdb['image'])
			gt_boxes.append([0,0,0,0,0])
		
		if not os.path.isdir("./result/" + day + "/"):
			os.mkdir("./result/" + day + "/")
		if not os.path.isdir("./result/" + day + "/" + exp + "/"):
			os.mkdir("./result/" + day + "/" + exp + "/")
		if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/"):
			os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/")
		if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/"):
			os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/")

		if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/"):
			os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/")
	
	
		
	
		result_queue = []

	    	for im_idx in range(len(im_names)):
			print '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~'
			print 'Demo for data/demo/{}'.format(im_names[im_idx])
			print "model: ", model_name
			#print im_names[im_idx], gt_boxes[im_idx]

					
			
			result = rotation_demo.demo(net, im_names[im_idx], gt_boxes[im_idx], "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/", "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/", model["conf"])

			im_file = im_names[im_idx]
			im = cv2.imread(im_file)
			im_height = im.shape[0]
			im_width = im.shape[1]
			result_queue.append({"result": result, "image": im_file, "height": im_height, "width": im_width})
			#print result_queue[0]

		results_matrix.append({"result_queue":result_queue, "day": day, "exp": exp, "model_name": model_name})

		

		#dets = det_fusion(result_queue)
		#rotation_demo.write_result_ICDAR(im_names[im_idx], dets, 0.9,"./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/", im_height, im_width)


	print "fusioning..."

	for i in range(len(roidb)):
		bboxes_arr = []

		im_name = ""
		day = ""
		exp = ""
		model_name = ""
		im_height = 0
		im_width = 0

		for results_queue in results_matrix:
			bboxes_arr.extend(results_queue["result_queue"][i]["result"])	
			
			im_name = results_queue["result_queue"][i]["image"]
			day = results_queue["day"]
			exp = results_queue["exp"]
			model_name = results_queue["model_name"]
			im_height = results_queue["result_queue"][i]["height"]
			im_width = results_queue["result_queue"][i]["width"]

		dets = det_fusion(bboxes_arr)

		rotation_demo.write_result_ICDAR(im_name, dets, 0.7,"./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/", im_height, im_width)
	print "done"

def det_fusion(result_queue):

	fused_result = []

	dets = np.array(result_queue).astype(np.float32)
	
	

	if dets.shape[0] > 0:

		scores = dets[:, 5]
		asc_score = np.argsort(scores)
		des_score = asc_score[::-1] # reverse
		dets = dets[des_score, :]

		keep = rotate_gpu_nms(dets, 0.3) # D
	
		fused_result = dets[keep, :]

	return fused_result

def rrpn_test_ICDAR2015(day, exp, model_name, mode, net = "VGG16"):
	
	cfg.TEST.HAS_RPN = True  # Use RPN for proposals
	
	NETS = {'VGG16': ('VGG16', day + "/" + exp + "/" + model_name),
		'RES101': ('RES101', day + "/" + exp + "/" + model_name),							}

	# rrpn only
	prototxt = os.path.join(cfg.RRPN_MODELS_DIR, NETS[net][0],
                            'faster_rcnn_end2end', 'test.prototxt')


	caffemodel = os.path.join(cfg.DATA_DIR, 'faster_rcnn_models',
                              NETS[net][1])

	print caffemodel

	if not os.path.isfile(caffemodel):
        	raise IOError(('{:s} not found.\nDid you run ./data/script/'
                       'fetch_faster_rcnn_models.sh?').format(caffemodel))

	caffe.set_mode_gpu()
        caffe.set_device(0)
        cfg.GPU_ID = 0

	net = caffe.Net(prototxt, caffemodel, caffe.TEST)

	print '\n\nLoaded network {:s}'.format(caffemodel)

	 # Warmup on a dummy image
    	#im = 128 * np.ones((300, 500, 3), dtype=np.uint8)
    	#for i in xrange(2):
       # 	_, _= rotation_demo.r_im_detect(net, im)

	im_names = []#['IMG_0030.JPG','IMG_0059.JPG','IMG_0063.JPG']

    	roidb = test_ICDAR2015(mode)

    	gt_boxes = []	
    
	

    	for rdb in roidb:
		im_names.append(rdb['image'])
        	gt_boxes.append([0,0,0,0,0])
        
	if not os.path.isdir("./result/" + day + "/"):
		os.mkdir("./result/" + day + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/")

	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/")
	
    	for im_idx in range(len(im_names)):
        	print '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~'
        	print 'Demo for data/demo/{}'.format(im_names[im_idx])
		print "model: ", model_name
		#print im_names[im_idx], gt_boxes[im_idx]
        	rotation_demo.demo(net, im_names[im_idx], gt_boxes[im_idx], "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/", "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/")
	
	
	'''
	precision, recall = MSRA_eval(mode, "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/")

	f_measure = (2 * precision * recall) / (precision + recall)

	if not os.path.isdir("./exper"):
		os.mkdir("./exper")
	if not os.path.isdir("./exper/" + day + "/"):
		os.mkdir("./exper/" + day + "/")
	if not os.path.isdir("./exper/" + day + "/" + exp + "/"):
		os.mkdir("./exper/" + day + "/" + exp + "/")
	
	PR_file = "./exper/" + day + "/" + exp + "/" + day + "_" + exp + "_" + model_name + "_" + mode
	if not os.path.isfile(PR_file):
		os.mknod(PR_file)

	PR_obj = open(PR_file, "w")

	PR_obj.write("precision: "+ str(precision) + "\n" + "recall: " + str(recall) + "\nf-measure: " + str(f_measure))
    	PR_obj.close()


	precision_ori, recall_ori = MSRA_eval(mode, "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/")

	f_measure_ori = (2 * precision_ori * recall_ori) / (precision_ori + recall_ori)

	
	if not os.path.isdir("./exper/" + day + "/" + exp + "_origin/"):
		os.mkdir("./exper/" + day + "/" + exp + "_origin/")
	
	PR_file_ori = "./exper/" + day + "/" + exp + "_origin/" + day + "_" + exp + "_" + model_name + "_" + mode
	if not os.path.isfile(PR_file_ori):
		os.mknod(PR_file_ori)

	PR_obj_ori = open(PR_file_ori, "w")

	PR_obj_ori.write("precision: "+ str(precision_ori) + "\n" + "recall: " + str(recall_ori) + "\nf-measure: " + str(f_measure_ori))
    	PR_obj_ori.close()
	'''

def rrpn_test_ICDAR2013(day, exp, model_name, mode, net = "VGG16"):
	
	cfg.TEST.HAS_RPN = True  # Use RPN for proposals
	
	NETS = {'VGG16': ('VGG16', day + "/" + exp + "/" + model_name),
		'RES101': ('RES101', day + "/" + exp + "/" + model_name),							}

	# rrpn only
	prototxt = os.path.join(cfg.RRPN_MODELS_DIR, NETS[net][0],
                            'faster_rcnn_end2end', 'test.prototxt')


	caffemodel = os.path.join(cfg.DATA_DIR, 'faster_rcnn_models',
                              NETS[net][1])


	print caffemodel

	if not os.path.isfile(caffemodel):
        	raise IOError(('{:s} not found.\nDid you run ./data/script/'
                       'fetch_faster_rcnn_models.sh?').format(caffemodel))

	caffe.set_mode_gpu()
        caffe.set_device(0)
        cfg.GPU_ID = 0

	net = caffe.Net(prototxt, caffemodel, caffe.TEST)

	print '\n\nLoaded network {:s}'.format(caffemodel)

	 # Warmup on a dummy image
    	#im = 128 * np.ones((300, 500, 3), dtype=np.uint8)
    	#for i in xrange(2):
       # 	_, _= rotation_demo.r_im_detect(net, im)

	im_names = []#['IMG_0030.JPG','IMG_0059.JPG','IMG_0063.JPG']

    	roidb = get_ICDAR2013_test(mode)

    	gt_boxes = []	
    
    	for rdb in roidb:
		im_names.append(rdb['image'])
        	gt_boxes.append([0,0,0,0,0])
        
	if not os.path.isdir("./result/" + day + "/"):
		os.mkdir("./result/" + day + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/")

	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/")
	
    	for im_idx in range(len(im_names)):
        	print '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~'
        	print 'Demo for data/demo/{}'.format(im_names[im_idx])
		print "model: ", model_name
		#print im_names[im_idx], gt_boxes[im_idx]
        	rotation_demo.demo(net, im_names[im_idx], gt_boxes[im_idx], "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/", "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/")

def rrpn_test_ICDAR2013_ch1(day, exp, model_name, mode):
	
	cfg.TEST.HAS_RPN = True  # Use RPN for proposals
	
	NETS = {'rrpn': ('VGG16', day + "/" + exp + "/" + model_name)}

	# rrpn only
	prototxt = os.path.join(cfg.RRPN_MODELS_DIR, NETS['rrpn'][0],
                            'faster_rcnn_end2end', 'test.prototxt')


	caffemodel = os.path.join(cfg.DATA_DIR, 'faster_rcnn_models',
                              NETS['rrpn'][1])

	print caffemodel

	if not os.path.isfile(caffemodel):
        	raise IOError(('{:s} not found.\nDid you run ./data/script/'
                       'fetch_faster_rcnn_models.sh?').format(caffemodel))

	caffe.set_mode_gpu()
        caffe.set_device(0)
        cfg.GPU_ID = 0

	net = caffe.Net(prototxt, caffemodel, caffe.TEST)

	print '\n\nLoaded network {:s}'.format(caffemodel)

	 # Warmup on a dummy image
    	#im = 128 * np.ones((300, 500, 3), dtype=np.uint8)
    	#for i in xrange(2):
       # 	_, _= rotation_demo.r_im_detect(net, im)

	im_names = []#['IMG_0030.JPG','IMG_0059.JPG','IMG_0063.JPG']

    	roidb = get_ICDAR2013_test_ch1(mode)

    	gt_boxes = []	
    
    	for rdb in roidb:
		im_names.append(rdb['image'])
        	gt_boxes.append([0,0,0,0,0])
        
	if not os.path.isdir("./result/" + day + "/"):
		os.mkdir("./result/" + day + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/")

	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/")
	
    	for im_idx in range(len(im_names)):
        	print '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~'
        	print 'Demo for data/demo/{}'.format(im_names[im_idx])
		print "model: ", model_name
		#print im_names[im_idx], gt_boxes[im_idx]
        	rotation_demo.demo(net, im_names[im_idx], gt_boxes[im_idx], "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/", "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/")


def rrpn_test_COCOTEXT(day, exp, model_name, mode):
	
	cfg.TEST.HAS_RPN = True  # Use RPN for proposals
	
	NETS = {'rrpn': ('VGG16', day + "/" + exp + "/" + model_name)}

	# rrpn only
	prototxt = os.path.join(cfg.RRPN_MODELS_DIR, NETS['rrpn'][0],
                            'faster_rcnn_end2end', 'test.prototxt')


	caffemodel = os.path.join(cfg.DATA_DIR, 'faster_rcnn_models',
                              NETS['rrpn'][1])

	print caffemodel

	if not os.path.isfile(caffemodel):
        	raise IOError(('{:s} not found.\nDid you run ./data/script/'
                       'fetch_faster_rcnn_models.sh?').format(caffemodel))

	caffe.set_mode_gpu()
        caffe.set_device(0)
        cfg.GPU_ID = 0

	net = caffe.Net(prototxt, caffemodel, caffe.TEST)

	print '\n\nLoaded network {:s}'.format(caffemodel)

	 # Warmup on a dummy image
    	#im = 128 * np.ones((300, 500, 3), dtype=np.uint8)
    	#for i in xrange(2):
       # 	_, _= rotation_demo.r_im_detect(net, im)

	im_names = []#['IMG_0030.JPG','IMG_0059.JPG','IMG_0063.JPG']

    	roidb = get_test_coco_img(mode)

    	gt_boxes = []	
    
    	for rdb in roidb:
		im_names.append(rdb['image'])
        	gt_boxes.append([0,0,0,0,0])
        
	if not os.path.isdir("./result/" + day + "/"):
		os.mkdir("./result/" + day + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/coco_" + mode + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/coco_" + mode + "/")

	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/coco_" + mode + "_origin/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/coco_" + mode + "_origin/")
	
    	for im_idx in range(len(im_names)):
        	print '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~'
        	print 'Demo for data/demo/{}'.format(im_names[im_idx])
		print "model: ", model_name
		#print im_names[im_idx], gt_boxes[im_idx]
        	rotation_demo.demo(net, im_names[im_idx], gt_boxes[im_idx], "./result/" + day + "/" + exp + "/" + model_name + "/coco_" + mode + "/", "./result/" + day + "/" + exp + "/" + model_name + "/coco_" + mode + "_origin/")

def rrpn_test_BOT(day, exp, model_name, mode):
	
	cfg.TEST.HAS_RPN = True  # Use RPN for proposals
	
	NETS = {'rrpn': ('VGG16', day + "/" + exp + "/" + model_name)}

	# rrpn only
	prototxt = os.path.join(cfg.RRPN_MODELS_DIR, NETS['rrpn'][0],
                            'faster_rcnn_end2end', 'test.prototxt')


	caffemodel = os.path.join(cfg.DATA_DIR, 'faster_rcnn_models',
                              NETS['rrpn'][1])

	print caffemodel

	if not os.path.isfile(caffemodel):
        	raise IOError(('{:s} not found.\nDid you run ./data/script/'
                       'fetch_faster_rcnn_models.sh?').format(caffemodel))

	caffe.set_mode_gpu()
        caffe.set_device(0)
        cfg.GPU_ID = 0

	net = caffe.Net(prototxt, caffemodel, caffe.TEST)

	print '\n\nLoaded network {:s}'.format(caffemodel)

	 # Warmup on a dummy image
    	#im = 128 * np.ones((300, 500, 3), dtype=np.uint8)
    	#for i in xrange(2):
       # 	_, _= rotation_demo.r_im_detect(net, im)

	im_names = []#['IMG_0030.JPG','IMG_0059.JPG','IMG_0063.JPG']

    	roidb = get_bot_img(mode)

    	gt_boxes = []	
    
    	for rdb in roidb:
		im_names.append(rdb['image'])
        	gt_boxes.append([0,0,0,0,0])
        
	if not os.path.isdir("./result/" + day + "/"):
		os.mkdir("./result/" + day + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/")

	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/")
	
    	for im_idx in range(len(im_names)):
        	print '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~'
        	print 'Demo for data/demo/{}'.format(im_names[im_idx])
		print "model: ", model_name
		#print im_names[im_idx], gt_boxes[im_idx]
        	rotation_demo.demo(net, im_names[im_idx], gt_boxes[im_idx], "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/", "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/")


def joint_test_rrpn_MSRA(day, exp):
	model_dir = "/home/shiki-alice/workspace/Rotation-Faster-RCNN/py-faster-rcnn/data/faster_rcnn_models/" + day + "/" + exp + "/"
	exper_dir = "/home/shiki-alice/workspace/Rotation-Faster-RCNN/py-faster-rcnn/exper/" + day + "/" + exp + "/"
	exper_dir_ori = "/home/shiki-alice/workspace/Rotation-Faster-RCNN/py-faster-rcnn/exper/" + day + "/" + exp + "_origin/"
	for model in os.listdir(model_dir):
		print model
		rrpn_test_MSRA(day, exp, model, "test")
	#for model in os.listdir(model_dir):
		#print model
		#rrpn_test_MSRA(day, exp, model, "train")

	#fusion_num(day + "_" + exp, exper_dir)
	#fusion_num(day + "_" + exp + "_origin", exper_dir_ori)

def rrpn_test_BOT(day, exp, model_name, mode):
	
	cfg.TEST.HAS_RPN = True  # Use RPN for proposals
	
	NETS = {'rrpn': ('VGG16', day + "/" + exp + "/" + model_name)}

	# rrpn only
	prototxt = os.path.join(cfg.RRPN_MODELS_DIR, NETS['rrpn'][0],
                            'faster_rcnn_end2end', 'test.prototxt')


	caffemodel = os.path.join(cfg.DATA_DIR, 'faster_rcnn_models',
                              NETS['rrpn'][1])

	print caffemodel

	if not os.path.isfile(caffemodel):
        	raise IOError(('{:s} not found.\nDid you run ./data/script/'
                       'fetch_faster_rcnn_models.sh?').format(caffemodel))

	caffe.set_mode_gpu()
        caffe.set_device(0)
        cfg.GPU_ID = 0

	net = caffe.Net(prototxt, caffemodel, caffe.TEST)

	print '\n\nLoaded network {:s}'.format(caffemodel)

	 # Warmup on a dummy image
    	#im = 128 * np.ones((300, 500, 3), dtype=np.uint8)
    	#for i in xrange(2):
       # 	_, _= rotation_demo.r_im_detect(net, im)

	im_names = []#['IMG_0030.JPG','IMG_0059.JPG','IMG_0063.JPG']

    	roidb = get_bot_img(mode)

    	gt_boxes = []	
    
    	for rdb in roidb:
		im_names.append(rdb['image'])
        	gt_boxes.append([0,0,0,0,0])
        
	if not os.path.isdir("./result/" + day + "/"):
		os.mkdir("./result/" + day + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/")
	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/")

	if not os.path.isdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/"):
		os.mkdir("./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/")
	
    	for im_idx in range(len(im_names)):
        	print '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~'
        	print 'Demo for data/demo/{}'.format(im_names[im_idx])
		print "model: ", model_name
		#print im_names[im_idx], gt_boxes[im_idx]
        	rotation_demo.demo(net, im_names[im_idx], gt_boxes[im_idx], "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "/", "./result/" + day + "/" + exp + "/" + model_name + "/" + mode + "_origin/")


def joint_test_rrpn_ICDAR2015(day, exp, net):
	model_dir = "/home/shiki-alice/workspace/Rotation-Faster-RCNN/py-faster-rcnn/data/faster_rcnn_models/" + day + "/" + exp + "/"
	exper_dir = "/home/shiki-alice/workspace/Rotation-Faster-RCNN/py-faster-rcnn/exper/" + day + "/" + exp + "/"
	exper_dir_ori = "/home/shiki-alice/workspace/Rotation-Faster-RCNN/py-faster-rcnn/exper/" + day + "/" + exp + "_origin/"
	for model in os.listdir(model_dir):
		print model
		rrpn_test_ICDAR2015(day, exp, model, "test", net)
	#for model in os.listdir(model_dir):
		#print model
		#rrpn_test_MSRA(day, exp, model, "train")

	#fusion_num(day + "_" + exp, exper_dir)
	#fusion_num(day + "_" + exp + "_origin", exper_dir_ori)


def fusion_num(fusion_file, result_dir):

	fusion_str = ""	
	
	train_list = []
	test_list = []

	for f in os.listdir(result_dir):
		f_l = f.split("_")
		if f_l[len(f_l) - 1] == "train":
			train_list.append(f)
		elif f_l[len(f_l) - 1] == "test":
			test_list.append(f)

	train_list.sort()
	test_list.sort()

	for re_file in train_list:
		re_obj = open(result_dir + "/" + re_file)
		re_content = re_obj.read()
		
		fusion_str += re_file + "\n" + re_content + "\n\n"
		
	for re_file in test_list:
		re_obj = open(result_dir + "/" + re_file)
		re_content = re_obj.read()
		
		fusion_str += re_file + "\n" + re_content + "\n\n"

	if not os.path.isfile(fusion_file):
		os.mknod(fusion_file)	
	fu_fobj = open(fusion_file, "w")
	fu_fobj.write(fusion_str)
	fu_fobj.close()


def get_test_coco_img(mode):
	coco_train_dir = '/var/www/html/data/MSCOCO/train2014/'
	coco_val_dir = '/var/www/html/data/MSCOCO/val2014/'
	
	train_list = os.listdir(coco_train_dir)
	val_list = os.listdir(coco_val_dir)

	img_file_type = "jpg"
	im_infos = []

	if mode == 'train':	
		for file_name in train_list:
			split = file_name.split(".")
			if split[len(split) - 1] == img_file_type:
			

				im_info = {
				'set': 'train',
				'image': coco_train_dir + file_name
				}
				im_infos.append(im_info)
	if mode == 'val':
		for file_name in val_list:
			split = file_name.split(".")
			if split[len(split) - 1] == img_file_type:
			
				im_info = {
				'set': 'val',
				'image': coco_val_dir + file_name
				}
				im_infos.append(im_info)

	return im_infos



def get_bot_img(mode):
	
	bot_img_dir = {
			'noodle': '/var/www/html/data/BOT2/bot_train1/Train/Instant Noodle/',
			'shampoo': '/var/www/html/data/BOT2/bot_train1/Train/Shampoo',
			'potato': '/var/www/html/data/BOT2/bot_train1/Train/Potato Chips'
			}

	im_infos = []
	file_list = os.listdir(bot_img_dir[mode])

	for file_name in file_list:
		split = file_name.split(".")
		
		
		im_info = {
		'set': mode,
		'image': bot_img_dir[mode] + file_name
		}
		im_infos.append(im_info)
	return im_infos

if __name__ == "__main__":
	rrpn_test_ICDAR2015("test", "exp", "vgg16_faster_rcnn_iter_450000.caffemodel", "test")

	
