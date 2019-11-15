//
// Created by hbk on 19-4-19.
//

#ifndef VERIFIER_HANDLERYOLO_H
#define VERIFIER_HANDLERYOLO_H

#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include "common/common.h"
#include "HandlerBase.h"
#include "common/base64.h"
#include "common/json/json.h"
#include <opencv2/opencv.hpp>

namespace dev {
    class ConfigParams;
}
using namespace std;

class YoloDetector;
namespace detect{
    class HandlerYolo : public MQ::HandlerBase {

    public:
        HandlerYolo();
        explicit HandlerYolo(const std::string &path, const dev::ConfigParams *cp_);
        ~HandlerYolo();

        std::string DetectYolo(std::string const &request, const dev::TIMEPOINTS &timePoints);

        void GetObjects(const cv::Mat & image, std::vector<std::vector<float>> &locations);

        std::string handle(const Message &message_);


    private:
        std::shared_ptr<YoloDetector> yoloDetector;
        const dev::ConfigParams *cp = nullptr;

    };
}



#endif //VERIFIER_HANDLERYOLO_H
