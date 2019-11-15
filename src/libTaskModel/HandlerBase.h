#ifndef  _I_HANDLER_H_
#define  _I_HANDLER_H_

#define YOLO_METHOD_ID 0
#define SHIP_METHOD_ID 1
#define SAR_METHOD_ID 2

#include <chrono>
#include "common/tokenBucket.h"
#include "common/common.h"
#include "common/configParams.h"
#include "common/json/json.h"
//#include "common/easylog++.h"
#include "common/base64.h"
#include <opencv2/opencv.hpp>

#ifdef USE_REST
#include "restServer/RestMessage.h"
typedef MQ::RestMessage Message;
#else
#include "zmqServer/ZmqMessage.h"
typedef MQ::ZmqMessage Message;
#endif

//log等级定义
#define  LOG_DEBUG_LEVEL 4
#define  LOG_APP_LEVEL 3
#define  LOG_WARN_LEVEL 2
#define  LOG_ERROR_LEVEL 1

namespace MQ {
    class HandlerBase
    {
    public:
        HandlerBase() = default;
        explicit HandlerBase(const std::string& apiKey_){
            apiKey = apiKey_;

        }

        std::string fillResponse(
                const std::string &interface, Json::Value &response, const char *responseID = NULL,
                const dev::TIMEPOINTS &timePoints = dev::TIMEPOINTS(1, CURRENT_SYS_TIMEPOINT));

        std::string fillErrorResponse(
                const std::string &interface, Json::Value &response, const char *responseID = NULL,
                const std::string &error_message = "",
                const dev::TIMEPOINTS &timePoints = dev::TIMEPOINTS(1, CURRENT_SYS_TIMEPOINT));

        void writeLog(const std::string &interface, const Json::Value &resp, const dev::TIMEPOINTS &timePoints);

        bool auth(const std::string &api_key);

        std::string validate(const std::string &interface, std::string const &request, Json::Value &value);

        std::string generateRequestID();

        static void writeLog(int level, const char* action, const char* szFormat, ...);

        static bool base64ToMat(const std::string &s, cv::Mat &decode_img);

        void rectToLocation(const cv::Rect& rect, std::vector<int>& location)
        {
            location.clear();
            location.resize(4);
            location[0] = rect.x;
            location[1] = rect.y;
            location[3] = rect.x + rect.width;
            location[4] = rect.y + rect.height;
        }

        cv::Rect locationToRect(const std::vector<int>& location)
        {
            return cv::Rect(location[0], location[1], location[2] - location[0], location[3] - location[1]);
        }

        static float getIOU(const std::vector<int>& a, cv::Rect& b)
        {
            cv::Rect rectA(a[0], a[1], a[2] - a[0], a[3] - a[1]);
            cv::Rect iou = rectA & b;
            return (getArea(iou) / std::min(getArea(rectA), getArea(b)));
            //return getArea(iou) / (getArea(rectA) + getArea(b) - getArea(iou));
        }

        static float getArea(cv::Rect &roi)
        {
            return roi.width * roi.height;
        }


        virtual std::string handle(const Message& message_) = 0;


    public:
        static bool super_switch;
//        face::FPoints ref_96_112;
//        face::FPoints ref_112_112;
//        face::FPoints ref_200_200; // use to align face before predict gender

    protected:
        std::string apiKey;
    };
}

#endif