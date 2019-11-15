#include "handler.h"
#include "common/configParams.h"
#include "caffePool.h"

#include <exception>
#include <iostream>

using namespace std;
using namespace dev;

Handler::Handler(const ConfigParams *_cp, const std::string &type, const int featuresPerFile, const int writeType) : cp(_cp), procQueue(cp->detectPoolSize * queueFactor), writeQueue(cp->detectPoolSize * queueFactor * 2)
{
    photoDir = string("data/") + type + "Photo/";
    caffeThreads.reset(new CaffePool(cp, &procQueue, &writeQueue));
    writer.reset(new Writer(&writeQueue, string("data/") + type + "Feature/", cp, featuresPerFile, 0, writeType));
}

void Handler::GetFeature(std::string const &photoName)
{
    cv::Mat img = cv::imread(photoDir + photoName);
    if (img.empty())
    {
        cout << "cv::imread failed." << endl;
        return;
    }
    PhotoMessage msg(img, photoName);
    procQueue.push(msg);
}

void Handler::shutdown()
{
    while (procQueue.size() != 0 || writeQueue.size() != 0)
        this_thread::sleep_for(chrono::milliseconds(50));
    this_thread::sleep_for(chrono::milliseconds(100));
    PhotoMessage msg;
    msg.msgType = SHUTDOWN;
    procQueue.push(msg);
    caffeThreads->shutdown();
    writer->shutdown();
}

void Handler::setWriterOffset(size_t offset)
{
    writer->setOffset(offset);
}
