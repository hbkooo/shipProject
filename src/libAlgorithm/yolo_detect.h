//
// Created by hbk on 19-4-19.
//

#ifndef VERIFIER_YOLO_DETECT_H
#define VERIFIER_YOLO_DETECT_H


#include<iostream>
#include<vector>
#include "darknet.h"
#include <opencv2/core/core.hpp>

using namespace std;

class YoloDetector {

public:
    YoloDetector(const string& cfg_path, const string& weight_path, const string& meta_path);
    ~YoloDetector();
    std::vector<std::vector<float>> detect(const cv::Mat &img);

private:
    image transCVMatToImage(const cv::Mat& img);    // convert Mat to image


private:
    network * yolo_net = nullptr;
    metadata meta;             // net classes

};

#endif //VERIFIER_YOLO_DETECT_H
