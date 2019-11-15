/*********************************************************
*   Copyright (C) 2017 All rights reserved.
*   
* File Name: processMessage.h
* Purpose:
* Creation Date: 2017-10-17
* Created By: bxq2011hust@qq.com
* Compile :
*********************************************************/

#ifndef PHOTO_MESSAGE_STRUCT_H_
#define PHOTO_MESSAGE_STRUCT_H_

#include <vector>
#include <string>
#include <chrono>
#include <cmath>
#include <opencv2/opencv.hpp>
#include "common/json/json.h"
#include "common/common.h"

namespace dev
{
#define SHUTDOWN 1

struct PhotoMessage
{
    PhotoMessage() { makeTimePoint(); }
    PhotoMessage(cv::Mat &image_, const std::string &name_, const int &msgType_ = 0) : image(image_), name(name_),
                                                                                       msgType(msgType_)
    {
        makeTimePoint();
    }

    PhotoMessage(const PhotoMessage &msg)
    {
        *this = msg;
    }

    PhotoMessage(PhotoMessage &&msg) noexcept
    {
        *this = msg;
    }

    PhotoMessage &operator=(const PhotoMessage &msg)
    {
        if (&msg == this)
            return *this;
        image = msg.image;
        name = msg.name;
        msgType = msg.msgType;
        L2Norm = msg.L2Norm;
        timePoints = msg.timePoints;
        feature = msg.feature;
        return *this;
    }

    PhotoMessage &operator=(PhotoMessage &&msg) noexcept
    {
        if (&msg == this)
            return *this;
        image = msg.image;
        name = std::move(msg.name);
        msgType = msg.msgType;
        L2Norm = msg.L2Norm;
        timePoints = std::move(msg.timePoints);
        feature = std::move(msg.feature);
        return *this;
    }

    void doL2Norm()
    {
        for (const auto &i : feature)
            L2Norm += i * i;
        L2Norm = sqrt(L2Norm);
        for (auto &i : feature)
            i /= L2Norm;
    }
    
    cv::Mat image;
    std::string name;
    std::vector<float> feature;
    int msgType;
    float L2Norm = 0;
    TIMEPOINTS timePoints;
    void makeTimePoint() { timePoints.push_back(std::chrono::system_clock::now()); }
};
}
#endif //PHOTO_MESSAGE_STRUCT_H_
