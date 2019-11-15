//
// Created by hbk on 19-4-19.
//

#ifndef VERIFIER_HANDLERSAR_H
#define VERIFIER_HANDLERSAR_H

#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include "common/common.h"
#include "libTaskModel/HandlerBase.h"
#include "common/base64.h"
#include "common/json/json.h"
#include <opencv2/opencv.hpp>

namespace dev {
    class ConfigParams;
}
using namespace std;

class SarDetector;
namespace detect {
    class HandlerSAR : public MQ::HandlerBase {
    public:
        HandlerSAR();
        explicit HandlerSAR(const std::string &path, const dev::ConfigParams *cp_);

        ~HandlerSAR();

        std::string Detectsar(std::string const &request, const dev::TIMEPOINTS &timePoints);

        void GetSARs(const cv::Mat &image, std::vector<std::vector<float>> &locates);

        std::string handle(const Message &message_);


    private:
        std::shared_ptr<SarDetector> sarDetector;

        const dev::ConfigParams *cp = nullptr;




    };
}




#endif //VERIFIER_HANDLERSAR_H
