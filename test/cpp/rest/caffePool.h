#ifndef CAFFE_POOL_H
#define CAFFE_POOL_H

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include "common/blockQueue.h"
#include "libTaskModel/HandlerDetect.h"
#include "common/configParams.h"
#include <caffe/caffe.hpp>

namespace dev
{
class CaffePool
{
    std::vector<std::thread> threads;
    std::atomic<bool> done;
    BlockingQueue<dev::PhotoMessage> *msgQueue;
    BlockingQueue<dev::PhotoMessage> *writeQueue;
    std::string modelPath;
    const int gpuid;
    const dev::ConfigParams *cp = nullptr;

    void doWork()
    {
#ifdef CPU_ONLY
        std::cout << "Worker::Run CPU" << std::endl;
        caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
        std::cout << "Worker::Run GPU" << std::endl;
        caffe::Caffe::set_mode(caffe::Caffe::GPU);
        caffe::Caffe::SetDevice(gpuid);
        caffe::Caffe::set_solver_count(1);
#endif
        std::shared_ptr<face::HandlerDetect> worker(new face::HandlerDetect(modelPath, cp));
        face::ImageFacesInfo facesInfo;

        while (!done)
        {
            // dev::PhotoMessage msg = msgQueue->get();
            dev::PhotoMessage msg;
            if (msgQueue->tryGet(msg, 30))
            {
                msg.makeTimePoint();
                int res = 1;
                switch (msg.msgType)
                {
                case SHUTDOWN:
                    done = true;
                    break;
                default:
                    // 检测不到人脸则提取图片特征
                    if (!worker->doDetect(msg.image, facesInfo, 80, 300))
                        continue;
                        // msg.feature = worker->GetFeatureOnly(msg.image);
                    msg.doL2Norm();
                    break;
                }

                writeQueue->push(msg);
            }
        }
    }

  public:
    CaffePool(const ConfigParams *_cp, BlockingQueue<dev::PhotoMessage> *_msgQueue, BlockingQueue<dev::PhotoMessage> *_writeQueue)
        : cp(_cp), done(false), msgQueue(_msgQueue), writeQueue(_writeQueue), modelPath(_cp->modelPath), gpuid(0)
    {

        // 创建线程
        try
        {
            for (int i = 0; i < _cp->detectPoolSize; ++i)
            {
                threads.push_back(std::thread(&CaffePool::doWork, this));
            }
        }
        catch (std::exception &e)
        {
            std::cerr << "Thread init failed!" << std::endl;
            std::cerr << e.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "Thread init failed!" << std::endl;
        }

    }
    void shutdown()
    {
        for (auto &th : threads)
        {
            if (th.joinable())
                th.join();
        }
    }
    ~CaffePool()
    {
        done = true;
        shutdown();
    }
};
}
#endif // !CAFFE_POOL_H
