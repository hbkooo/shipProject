//
// Created by hbk on 19-4-19.
//

//#include <caffe/caffe.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>
#include <iomanip>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "sar_detect.h"


using namespace caffe;  // NOLINT(build/namespaces)


SarDetector::SarDetector(const string& model_file,
                         const string& weights_file) {

#ifdef CPU_ONLY                         // work model CPU or GPU
    Caffe::set_mode(Caffe::CPU);
    std::cout << "use CPU to caculate SAR .... \n";
#else
    Caffe::set_mode(Caffe::GPU);
    std::cout << "use GPU to caculate SAR .... \n";
#endif


    /* Load the network. */

    net_.reset(new Net<float>(model_file, TEST));                        //��model_file�ж�ȡ����ṹ����ʼ������
    net_->CopyTrainedLayersFrom(weights_file);                                     //��Ȩ�ļ��ж�ȡ�����������ʼ��net_

    std::cout << "sar model is loading over ... \n";

    CHECK_EQ(net_->num_inputs(), 1) << "Network should have exactly one input.";
    CHECK_EQ(net_->num_outputs(), 1) << "Network should have exactly one output.";

    input_layer = net_->input_blobs()[0];
    num_channels_ = input_layer->channels();                                      //��ȡ����ͼƬ��channel
    CHECK(num_channels_ == 3 || num_channels_ == 1)
      << "Input layer should have 1 or 3 channels.";
    input_geometry_ = cv::Size(input_layer->width(), input_layer->height());      //��ȡ����ͼƬ��size


    // Load the binaryproto mean file.
    SetMean("104,117,123");                                               //��ʼ����ͼƬ���������г�����������ʼ����



}

// detector : get information formation : [label, score, xmin, ymin, xmax, ymax]
vector<vector<float> > SarDetector::Detect(const cv::Mat& img) {

    input_layer->Reshape(1, num_channels_, input_geometry_.height, input_geometry_.width);
    net_->Reshape();                                          // net reshape

    std::vector<cv::Mat> input_channels;
    WrapInputLayer(&input_channels);                          // bind net and input_channels
    Preprocess(img, &input_channels);                         // pre process
    net_->ForwardFrom(0);                                     // net forward

    /* Copy the output layer to a std::vector */
    Blob<float>* result_blob = net_->output_blobs()[0];
    const float* result = result_blob->cpu_data();            //the detect out information
    const int num_det = result_blob->height();                // height : the number of detection
    vector<vector<float> > detections;
    for (int k = 0; k < num_det; ++k) {
        if (result[0] == -1) {                                //-1 is background
            // Skip invalid detection.
            result += 7;                                      // 7 infos represent one location
            continue;
        }

        // Detection format: [label, score, xmin, ymin, xmax, ymax]
        vector<float> detection(result + 1, result + 7);      //vector to save location
        detection[2] = detection[2] * img.cols;
        detection[3] = detection[3] * img.rows;
        detection[4] = detection[4] * img.cols;
        detection[5] = detection[5] * img.rows;
        detections.push_back(detection);
        // Detection format: [image_id, label, score, xmin, ymin, xmax, ymax]
        result += 7;
    }

    result_blob = nullptr;
    result = nullptr;

    return detections;
}

/* Load the mean file in binaryproto format. */
void SarDetector::SetMean(const string& mean_value) {
    //mean_file or mean_value
    cv::Scalar channel_mean;

    stringstream ss(mean_value);
    vector<float> values;
    string item;
    while (getline(ss, item, ',')) {
        float value = std::atof(item.c_str());
        values.push_back(value);
    }
    CHECK(values.size() == 1 || values.size() == num_channels_) <<
            "Specify either 1 mean_value or as many as channels: " << num_channels_;

    std::vector<cv::Mat> channels;
    for (int i = 0; i < num_channels_; ++i) {
        /* Extract an individual channel. */
        cv::Mat channel(input_geometry_.height, input_geometry_.width, CV_32FC1,
                            cv::Scalar(values[i]));
        channels.push_back(channel);
    }
    cv::merge(channels, mean_);

}

/* Wrap the input layer of the network in separate cv::Mat objects
 * (one per channel). This way we save one memcpy operation and we
 * don't need to rely on cudaMemcpy2D. The last preprocessing
 * operation will write the separate channels directly to the input //bang ding
 * layer. */
void SarDetector::WrapInputLayer(std::vector<cv::Mat>* input_channels) {    //���ｫ����input_channels�����������󶨣�wrap��

    int width = input_layer->width();
    int height = input_layer->height();
    float* input_data = input_layer->mutable_cpu_data();
    for (int i = 0; i < input_layer->channels(); ++i) {
        cv::Mat channel(height, width, CV_32FC1, input_data);
        input_channels->push_back(channel);
        input_data += width * height;
    }
    input_data = nullptr;
}


void SarDetector::Preprocess(const cv::Mat& img,
                             std::vector<cv::Mat>* input_channels) {
    /* Convert the input image to the input image format of the network. */
    cv::Mat sample;
    if (img.channels() == 3 && num_channels_ == 1)
        cv::cvtColor(img, sample, cv::COLOR_BGR2GRAY);
    else if (img.channels() == 4 && num_channels_ == 1)
        cv::cvtColor(img, sample, cv::COLOR_BGRA2GRAY);
    else if (img.channels() == 4 && num_channels_ == 3)
        cv::cvtColor(img, sample, cv::COLOR_BGRA2BGR);
    else if (img.channels() == 1 && num_channels_ == 3)
        cv::cvtColor(img, sample, cv::COLOR_GRAY2BGR);
    else
        sample = img;

    cv::Mat sample_resized;
    if (sample.size() != input_geometry_)
        cv::resize(sample, sample_resized, input_geometry_);
    else
        sample_resized = sample;

    cv::Mat sample_float;
    if (num_channels_ == 3)
        sample_resized.convertTo(sample_float, CV_32FC3);
    else
        sample_resized.convertTo(sample_float, CV_32FC1);

    cv::Mat sample_normalized;
    cv::subtract(sample_float, mean_, sample_normalized);

    /* This operation will write the separate BGR planes directly to the
     * input layer of the network because it is wrapped by the cv::Mat
     * objects in input_channels. */
    cv::split(sample_normalized, *input_channels);

    CHECK(reinterpret_cast<float*>(input_channels->at(0).data)
          == net_->input_blobs()[0]->cpu_data())
            << "Input channels are not wrapping the input layer of the network.";
}

SarDetector::~SarDetector() {
    input_layer = nullptr;

}
