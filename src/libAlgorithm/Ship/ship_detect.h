//
// Created by hbk on 19-4-19.
//

#ifndef VERIFIER_SHIP_DETECT_H
#define VERIFIER_SHIP_DETECT_H

#include <string>
#include <caffe/net.hpp>
#include <caffe/common.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <memory>
#include <vector>
//#include <boost/python.hpp>
#include <math.h>

using namespace caffe;


/*
 * ===  Class  ======================================================================
 *         Name:  Detector
 *  Description:  FasterRCNN CXX Detector
 * =====================================================================================
 */
class ShipDetector {
public:
    ShipDetector(const string& model_file, const string& weights_file);
    std::vector<vector<float> > Detection(const cv::Mat &image);
private:
    void bbox_transform_inv(const int num, const float* box_deltas, const float* pred_cls, float* boxes, float* pred);
    void vis_detections(cv::Mat image, int* keep, int num_out, float* sorted_pred_cls, float CONF_THRESH,int class_label);
    void boxes_sort(int num, const float* pred, float* sorted_pred);
    std::vector<cv::Point> computeLocation(float cx, float cy, float h, float w, float angle);
    std::shared_ptr<Net<float> > net_;
    ShipDetector(){}
};

#endif //VERIFIER_SHIP_DETECT_H
