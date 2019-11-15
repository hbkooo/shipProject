//
// Created by hbk on 19-4-19.
//

#ifndef VERIFIER_HANDLERSHIP_H
#define VERIFIER_HANDLERSHIP_H

#include <vector>
#include <memory>
#include <chrono>
#include "common/common.h"
#include "common/base64.h"
#include "common/json/json.h"
#include "libTaskModel/HandlerBase.h"
#include <opencv2/opencv.hpp>

namespace dev {
    class ConfigParams;
}

using namespace std;

class ShipDetector;
namespace detect {
    class HandlerShip : public MQ::HandlerBase {

    public:
        HandlerShip();
        explicit HandlerShip(const std::string &path, const dev::ConfigParams *cp_);
        ~HandlerShip();

        std::string DetectShip(std::string const &request, const dev::TIMEPOINTS &timePoints);

        void GetShips(const cv::Mat &image, std::vector<std::vector<float>> &locates);

        std::string handle(const Message &message_);

    private:
        std::shared_ptr<ShipDetector> shipDetector;
        const dev::ConfigParams *cp = nullptr;

    };
}




#endif //VERIFIER_HANDLERSHIP_H
