//
// Created by hbk on 19-4-19.
//

#include "HandlerYolo.h"
#include <glog/logging.h>
#include "libAlgorithm/yolo_detect.h"
#include "Queue.h"
#include "common/configParams.h"
#include "common/error.h"

using namespace detect;
using namespace MQ;
using namespace dev;

HandlerYolo::HandlerYolo() {

}

HandlerYolo::~HandlerYolo() {

}

HandlerYolo::HandlerYolo(const std::string &path, const dev::ConfigParams *cp_)
        : HandlerBase(cp_->apiKey), cp(cp_)
{

//    std::cout << "hbk in HandlerYolo() ... \n";
//    std::cout << "path = " << path << " \n";

    // GLOG 输出控制
    FLAGS_minloglevel = 3;
    yoloDetector = std::make_shared<YoloDetector>(path + "yolo.cfg", path + "yolo.weights", path + "yolo.data");

}

std::string HandlerYolo::DetectYolo(std::string const &request, const dev::TIMEPOINTS &timePoints) {
    writeLog(LOG_DEBUG_LEVEL, "HandlerYolo", "DetectYolo begin" );
    string interfaceName("/yolo");
    Json::Value value;
    string responseBody = std::move(validate(interfaceName, request, value));
    if(!responseBody.empty())
    {
        return responseBody;
    }
    Json::Value a;
    Json::Value resp;
    if(!value.isMember("image_base64") || !value["image_base64"].isString())
    {
        responseBody = HandlerBase::fillErrorResponse(interfaceName, resp, FINDER_PARAMETERS_ERROR, "Json参数错误，缺少参数或类型不对!");
        return responseBody;
    }
    cv::Mat image;
    if (!base64ToMat(value["image_base64"].asString(), image))
    {
        responseBody = HandlerBase::fillErrorResponse(interfaceName, resp, ERROR_IMAGE_BASE64,"图片base64编码错误!");
        return responseBody;
    }

    std::vector<std::vector<float>> locates;
    GetObjects(image,locates);
    resp["result_nums"] = locates.size();
    for(size_t i =0; i < locates.size(); ++i)
    {
        Json::Value result;
        result["label"] = locates[i][0];
        result["score"] = locates[i][1];
        result["xmin"] = locates[i][2];
        result["ymin"] = locates[i][3];
        result["xmax"] = locates[i][4];
        result["ymax"] = locates[i][5];
        resp["result"].append(result);
    }
    std::cout << "detect over and detct number is " << locates.size() << "\n\n";
    return HandlerBase::fillResponse(interfaceName, resp, FINDER_YOLO_SUCCESS, timePoints);
}

void HandlerYolo::GetObjects(const cv::Mat &image, std::vector<std::vector<float>> &locations) {

    writeLog(LOG_DEBUG_LEVEL, "HandlerYolo", " GetYolo begin");

    // format:[label , score, xmin, ymin, xmax, ymax ]
    locations = yoloDetector->detect(image);
}

std::string HandlerYolo::handle(const Message &message_) {
#ifdef USE_LICENSE
    if (!super_switch) {
        Json::Value resp;
        return HandlerBase::fillErrorResponse("/yolo", resp, FINDER_NOT_AUTHORIZED, "license is error ... ");
    }
#endif

    switch (message_.msgType) {
        case INTERFACEID_YOLO:
            return DetectYolo(message_.str, message_.timePoints);
        default:
            std::cout << "HandlerYolo unkonown msgType: " << message_.msgType << std::endl;
            std::string response = "unkonown message!!!!";
            return response;
    }
}



