//
// Created by hbk on 19-4-19.
//

#include <glog/logging.h>
#include "libAlgorithm/Ship/ship_detect.h"
#include "libTaskModel/Queue.h"
#include "common/configParams.h"
#include "common/error.h"

#include "HandlerShip.h"


using namespace std;
using namespace MQ;
using namespace detect;

HandlerShip::HandlerShip() {

}

HandlerShip::~HandlerShip() {

}

HandlerShip::HandlerShip(const std::string &path, const dev::ConfigParams *cp_)
        : HandlerBase(cp_->apiKey), cp(cp_)
{
    std::cout << "path : " << path << "\n";
    // GLOG 输出控制
    FLAGS_minloglevel = 3;

    string prototxt = path + "ship.prototxt";
    string model = path + "ship.caffemodel";

    std::cout << "prototxt : " << prototxt << "\n";
    std::cout << "model : " << model << "\n";

    shipDetector = std::make_shared<ShipDetector>(prototxt, model);

}

std::string HandlerShip::DetectShip(std::string const &request, const dev::TIMEPOINTS &timePoints) {
    writeLog(LOG_DEBUG_LEVEL, "HandlerShip", "DetectShip begin" );
    string interfaceName("/ship");
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

    //std::cout << "received image : \n";
    //std::cout << "image row is " << image.rows << " , and col is " << image.cols << "\n";

    std::vector<std::vector<float>> locates;
    GetShips(image,locates);
    resp["result_nums"] = locates.size();
    for(size_t i =0; i < locates.size(); ++i)
    {
        Json::Value result;
        result["label"] = locates[i][0];
        result["score"] = locates[i][1];
        result["x1"] = locates[i][2];
        result["y1"] = locates[i][3];
        result["x2"] = locates[i][4];
        result["y2"] = locates[i][5];
        result["x3"] = locates[i][6];
        result["y3"] = locates[i][7];
        result["x4"] = locates[i][8];
        result["y4"] = locates[i][9];
        resp["result"].append(result);
    }
    std::cout << "detect over and detct number is " << locates.size() << "\n\n";
    return HandlerBase::fillResponse(interfaceName, resp, FINDER_SHIP_SUCCESS, timePoints);
}

void HandlerShip::GetShips(const cv::Mat &image, std::vector<std::vector<float>> &locates) {

    writeLog(LOG_DEBUG_LEVEL, "HandlerShip", " GetShips begin");

    // format : [ label score x1 y1 x2 y2 x3 y3 x4 y4]
    locates = shipDetector->Detection(image);
}

std::string HandlerShip::handle(const Message &message_) {
#ifdef USE_LICENSE
    if (!super_switch) {
        Json::Value resp;
        return HandlerBase::fillErrorResponse("/sar", resp, FINDER_NOT_AUTHORIZED, "license is error ... ");
    }
#endif

    switch (message_.msgType) {
        case INTERFACEID_SHIP:
            return DetectShip(message_.str, message_.timePoints);
        default:
            std::cout << "HandlerShip unkonown msgType: " << message_.msgType << std::endl;
            std::string response = "unkonown message!!!!";
            return response;
    }
}
