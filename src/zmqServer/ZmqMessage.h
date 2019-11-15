#ifndef _ZMQ_MESSAGE_H_
#define _ZMQ_MESSAGE_H_

#include <memory>
#include <string>
#include "common/common.h"
#include "czmq.h"
#include "ZmqSender.h"

namespace MQ {
    class ZmqMessage {
    public:
        ZmqMessage() { makeTimePoint(); }

        ~ZmqMessage() {}

        ZmqMessage(ZmqSender *zmqSender_, int msgType_, std::string str_, int flags_,
                   zframe_t *addr_ = nullptr)
                :msgType(msgType_), str(str_), flags(flags_),
                 addr(addr_), zmqSender(zmqSender_){
            makeTimePoint();
        }

        ZmqMessage(const ZmqMessage &msg) {
            addr = msg.addr;
            str = msg.str;
            msgType = msg.msgType;
            zmqSender = msg.zmqSender;
            response = msg.response;
            timePoints = std::move(msg.timePoints);
        }

        ZmqMessage(ZmqMessage &&msg) noexcept {
            addr = msg.addr;
            str = msg.str;
            msgType = msg.msgType;
            zmqSender = msg.zmqSender;
            response = msg.response;
            timePoints = std::move(msg.timePoints);
        }

        ZmqMessage &operator=(const ZmqMessage &msg) {
            if (&msg == this)
                return *this;
            addr = msg.addr;
            str = msg.str;
            msgType = msg.msgType;
            zmqSender = msg.zmqSender;
            response = msg.response;
            timePoints = std::move(msg.timePoints);
            return *this;
        }

        ZmqMessage &operator=(ZmqMessage &&msg) noexcept {
            if (&msg == this)
                return *this;
            addr = msg.addr;
            str = msg.str;
            msgType = msg.msgType;
            zmqSender = msg.zmqSender;
            response = msg.response;
            timePoints = std::move(msg.timePoints);
            return *this;
        }

        void call(std::string &response_) {
            response = response_;
            zmqSender->push(*this);
        }

        void makeTimePoint() {
            timePoints.push_back(std::chrono::system_clock::now());
        }
    public:
        int                             msgType;
        std::string                     str;
        int                             flags;  //返回类型；0：router模式返回 1：Dealer模式返回
        zframe_t*                       addr;
        std::string                     response;
        ZmqSender*                      zmqSender;
        dev::TIMEPOINTS                 timePoints;
    };
}

#endif