//
// Created by ubtun on 18-6-14.
//

#ifndef VERIFIER_ZMQSENDER_H
#define VERIFIER_ZMQSENDER_H

#include <thread>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <list>
#include <vector>
#include <memory>
#include <string>
#include "zmq.h"
#include "czmq.h"
#include "common/json/json.h"
#include "common/common.h"
#include "common/error.h"

namespace MQ
{
    class ZmqMessage;
}

class ZmqSender
{
public:
    ZmqSender(void *context_, std::string zmqAddr_);

    ~ZmqSender();

    void run();

    void Join();

    void push(MQ::ZmqMessage &msg);

    bool get(MQ::ZmqMessage &msg, const uint16_t &time_ms = 1000);

    void shutdown();

private:
    std::thread*                    thread;
    std::deque<MQ::ZmqMessage>      msg_list;
    std::mutex                      mtx;
    std::condition_variable         condition;
    bool                            status;
    void*                           context;
    std::string                     zmqAddr;
};

#endif //VERIFIER_ZMQSENDER_H
