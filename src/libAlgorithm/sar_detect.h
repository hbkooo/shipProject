//
// Created by hbk on 19-4-19.
//

#ifndef VERIFIER_SAR_DETECT_H
#define VERIFIER_SAR_DETECT_H

//#include <caffe/caffe.hpp>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <algorithm>
//#include <iomanip>
//#include <iosfwd>
//#include <memory>
//#include <string>
//#include <utility>
//#include <vector>

#include <string>
#include <caffe/net.hpp>
#include <caffe/common.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <memory>
#include <vector>

using namespace caffe;

class SarDetector {

public:
    SarDetector(const string& model_file,
                const string& weights_file);
    ~SarDetector();

    // Detection format: [label, score, xmin, ymin, xmax, ymax]
    vector<vector<float> > Detect(const cv::Mat& img);

private:
    void SetMean(const string& mean_value);  //瀵筸ean_杩涜魀鍒濆糍鍖?

    void WrapInputLayer(std::vector<cv::Mat>* input_channels);        //灏?input_channels涓庣綉缁滆緭鍏ョ粦瀹?
    void Preprocess(const cv::Mat& img,
                    std::vector<cv::Mat>* input_channels);           //涓€绯诲垪鐨勯㻫澶勭悊



private:
    std::shared_ptr<Net<float> > net_;                                   //缃戠粶鎸囬拡
    cv::Size input_geometry_;                                       //杈撳叆鍥剧墖鐨剆ize
    Blob<float>* input_layer;
    int num_channels_;                                             //杈撳叆鍥剧墖鐨刢hannel
    cv::Mat mean_;                                                 //鍧囧€煎浘鐗?

};


#endif //VERIFIER_SAR_DETECT_H
