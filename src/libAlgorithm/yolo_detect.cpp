//
// Created by hbk on 19-4-19.
//

#include "yolo_detect.h"



using namespace std;

YoloDetector::YoloDetector(const string& cfg_path, const string& weight_path, const string& meta_path) {

//    std::cout << "hbk in YoloDetector() ... \n";
//    std::cout << "cfg_path = " << cfg_path << "\n";
//    std::cout << "weight_path = " << weight_path << "\n";
//    std::cout << "meta_path = " << meta_path << "\n";

    if (yolo_net == nullptr) {
        std::cout << "loading yolo network ... " << std::endl;

        yolo_net = load_network(const_cast<char *>(cfg_path.data()), const_cast<char *>(weight_path.data()), 0);
        meta = get_metadata(const_cast<char *>(meta_path.data()));
    }
}

YoloDetector::~YoloDetector() {

    // free yolo_net
    if(yolo_net != nullptr) {
        free_network(yolo_net);
        yolo_net = nullptr;
    }

}

// format:[label , confidence, xmin, ymin, xmax, ymax ]      the first label index is 1
std::vector<std::vector<float>> YoloDetector::detect(const cv::Mat &img) {

    float thresh = 0.5, hier_thresh = 0.5;
    float nms = 0.45;

    // load image
    //image im = load_image_color(image_path,0,0);
    image im = transCVMatToImage(img);

    // start predict
    network_predict_image(yolo_net,im);

    int nboxes = 0;  // the number of the predicted bounding box
    detection * dets = get_network_boxes(yolo_net,im.w,im.h,thresh,hier_thresh,NULL,0,&nboxes);

    if(nms) do_nms_obj(dets,nboxes,meta.classes,nms);

    // the result of the detection
    // formate:[label , confidence , x , y , w , h]
    std::vector<std::vector<float>> res;

    for (int i = 0; i < nboxes; i++) {
        for(int j = 0; j < meta.classes; j++) {
            if(dets[i].prob[j] > 0) {
                box b = dets[i].bbox;     // format:[label , confidence, centerX, centerY, w, h ]
                std::vector<float> detection(6);
                detection[0] = j + 1;           // class num  ===>  add 1 to make sure the first index is 1
                detection[1] = dets[i].prob[j]; // confidence
                detection[2] = b.x - b.w / 2;   // xmin
                detection[3] = b.y - b.h / 2;   // ymin
                detection[4] = b.x + b.w / 2;   // xmax
                detection[5] = b.y + b.h / 2;   // ymax
                res.push_back(detection);
            }
        }
    }

    free_image(im);         // free the resource of the image
    free_detections(dets,nboxes);  // free detection results

    //free_detections(dets,0)

    return res;
}

// convert Mat to image
image YoloDetector::transCVMatToImage(const cv::Mat& img) {

    IplImage * src = new IplImage(img);

    int h = src->height;
    int w = src->width;
    int c = src->nChannels;
    unsigned char *data = (unsigned char *)src->imageData;
    int step = src->widthStep; //size of aligned image row in bytes
    image out = make_image(w, h, c);   // create empty image container,(w,h,c)

    int i, j, k;

    for(i = 0; i < h; ++i){
        for(k= 0; k < c; ++k){
            for(j = 0; j < w; ++j){
                //归一化后存入一维数组
                out.data[k*w*h + i*w + j] = data[i*step + j*c + k]/255.;
            }
        }
    }

    rgbgr_image(out);               //convert BGR to RGB

    delete src;
    src = nullptr;


    return out;
}
