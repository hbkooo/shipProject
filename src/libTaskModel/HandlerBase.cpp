//
// Created by ubuntu on 18-5-17.
//
#include "HandlerBase.h"
#include "common/uuid.h"
#include <boost/lexical_cast.hpp>
#include "common/error.h"
#include "common/easylog++.h"
#include <opencv2/opencv.hpp>

using namespace cv;

using namespace MQ;
using namespace std;
using namespace dev;

static boost::uuids::random_generator uuidGenerator;

vector<unsigned char> Decode(const string &str)
{
    //解码表
    const char DecodeTable[] =
            {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    62, // '+'
                    0, 0, 0,
                    63,                                     // '/'
                    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
                    0, 0, 0, 0, 0, 0, 0,
                    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
                    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
                    0, 0, 0, 0, 0, 0,
                    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
                    39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
            };

    vector<unsigned char> strDecode;
    const unsigned char *Data = (const unsigned char*)str.data();
    size_t DataByte = str.size();
    int nValue;
    size_t i = 0;
    while (i < DataByte)
    {
        if (*Data != '\r' && *Data != '\n' && *Data > 0  && is_base64(*Data))
        {
            nValue = DecodeTable[*Data++] << 18;
            nValue += DecodeTable[*Data++] << 12;
            strDecode.push_back(static_cast<unsigned char>((nValue & 0x00FF0000) >> 16));
            if (*Data != '=')
            {
                nValue += DecodeTable[*Data++] << 6;
                strDecode.push_back(static_cast<unsigned char>((nValue & 0x0000FF00) >> 8));
                if (*Data != '=')
                {
                    nValue += DecodeTable[*Data++];
                    strDecode.push_back(static_cast<unsigned char>(nValue & 0x000000FF));
                }
            }
            i += 4;
        }
        else // 回车换行,跳过
        {
            Data++;
            i++;
        }
    }
    return strDecode;
}

bool HandlerBase::super_switch = false;

string HandlerBase::generateRequestID()
{
    return boost::lexical_cast<string>(uuidGenerator());
}

std::string HandlerBase::fillResponse(const string &interface, Json::Value &response, const char *responseID,
        const TIMEPOINTS &timePoints)
{
    response["error_message"].asString().c_str();
    response["request_id"] = generateRequestID();
    response["time_used"] = (chrono::duration_cast<chrono::milliseconds>(CURRENT_SYS_TIMEPOINT - timePoints.front())).count();
    response["response_id"] = responseID;
    writeLog(interface, response, timePoints);
    Json::FastWriter w;
    return w.write(response);
}

std::string HandlerBase::fillErrorResponse(const std::string &interface, Json::Value &response, const char *responseID,
                                           const string &error_message, const TIMEPOINTS &timePoints) {
    response["error_message"] = error_message;
    response["request_id"] = generateRequestID();
    response["time_used"] = (chrono::duration_cast<chrono::milliseconds>(CURRENT_SYS_TIMEPOINT - timePoints.front())).count();
    response["response_id"] = responseID;
    writeLog(interface, response, timePoints);
    Json::FastWriter w;
    return w.write(response);
}

void HandlerBase::writeLog(const string &interface, const Json::Value &resp, const TIMEPOINTS &timePoints)
{
    char timeStr[10];
    auto timeLong = chrono::duration_cast<chrono::milliseconds>(CURRENT_SYS_TIMEPOINT - timePoints.front());
    auto tp = chrono::system_clock::to_time_t(timePoints.front());
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", std::localtime(&tp));
    LOG(INFO) << "|Req Time: " << timeStr << left << "|Interface: " << interface << "|Time Used: " << timeLong.count() << " ms"
              << "|Result: " << resp << endl;
}

bool HandlerBase::auth(const string &api_key)
{
    return (apiKey == api_key);
}

std::string HandlerBase::validate(const std::string &interface, std::string const &request, Json::Value &value)
{
    Json::Reader reader;
    if (!reader.parse(request, value, false))
    {
        Json::Value resp;
        return fillErrorResponse(interface, resp, ERROR_JSON_SYNTAX, "json 数据格式有误!");
    }
    if (!value.isMember("api_key") || !auth(value["api_key"].asString()))
    {
        Json::Value resp;
        return fillErrorResponse(interface, resp, FINDER_API_KEY_INVALID,"api_key 有误!");
    }
    return string();
}

void HandlerBase::writeLog(int level, const char* action, const char* szFormat,...)
{
    char szHeade[200] = {0};
    char szParam[500] = {0} ;
    char szLeve[10] = {0} ;
    bool isError = false;
    switch (level)
    {
        case LOG_APP_LEVEL:
            strcpy(szLeve,"<app>");
            break;
        case LOG_DEBUG_LEVEL:
            strcpy(szLeve,"<dbg>");
            break;
        case LOG_WARN_LEVEL:
            strcpy(szLeve,"<war>");
            break;
        case LOG_ERROR_LEVEL: {
            isError = true;
            strcpy(szLeve,"<err>");
        }
            break;
    }
    sprintf(szHeade,"%s[%10s]", szLeve, action);

//    va_list args;
//    va_start(args,szFormat);
//    vsnprintf(szParam,300,szFormat,args);
//    va_end(args);
    char buf[1024] = {0};
//    sprintf(buf,"%s%s",szHeade, szParam);
    sprintf(buf,"%s%s",szHeade, szFormat);
    if (isError)
        LOG(ERROR) << buf;
    else
        LOG(DEBUG) << buf;
}

bool HandlerBase::base64ToMat(const string &s, Mat &decode_img)
{
    if(s.empty())
        return false;
    vector<unsigned char> x(Decode(s));
    if(x.empty())
        return false;

    try {
        // Mat是浅拷贝
        decode_img = imdecode(x, CV_LOAD_IMAGE_COLOR);
    } catch (cv::Exception &e) {
        cerr << "Caught exception in Base64Converter" << e.msg << endl;
    }

    return !decode_img.empty();
}

