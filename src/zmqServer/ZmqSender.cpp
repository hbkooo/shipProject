//
// Created by ubtun on 18-6-14.
//

#include "ZmqSender.h"
#include "ZmqMessage.h"

ZmqSender::ZmqSender(void *context_, std::string zmqAddr_)
        :context(context_), zmqAddr(zmqAddr_), status(true)
{
    thread = new std::thread(&ZmqSender::run, this);
}

ZmqSender::~ZmqSender() {
    delete thread;
    thread = nullptr;
}

void ZmqSender::run()
{
    void* rspSock = zmq_socket(context,  ZMQ_DEALER);
    int ret = zmq_connect (rspSock, zmqAddr.c_str());
    if (ret == -1) {
        zmq_close(rspSock);
        return;
    }

    MQ::ZmqMessage msg;
    while(status){
        if(get(msg))
        {
            if (msg.flags == 0)
                zframe_send(&msg.addr, rspSock, ZFRAME_REUSE + ZFRAME_MORE);
            zframe_t *frame = zframe_new(msg.response.c_str(), msg.response.size()+1);
            zframe_send(&frame, rspSock, ZFRAME_REUSE);
            if (msg.addr)
                zframe_destroy(&msg.addr);
            if (frame)
                zframe_destroy(&frame);
        }
    }
    zmq_close(rspSock);
}

void ZmqSender::Join()
{
    if (thread->joinable())
        thread->join();
}

void ZmqSender::push(MQ::ZmqMessage &msg)
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        msg_list.emplace_back(msg);
    }
    condition.notify_one();
}

bool ZmqSender::get(MQ::ZmqMessage &msg, const uint16_t &time_ms)
{
    std::unique_lock<std::mutex> lock(mtx);
    if (!status)
        return false;

    if (!condition.wait_for(lock, std::chrono::milliseconds(time_ms), [&] { return !msg_list.empty(); }) )
        return false;

    // while (msg_list.empty())
    //    condition.wait(lock);

    msg = msg_list.front();
    msg_list.pop_front();
    return true;
}

void ZmqSender::shutdown()
{
    std::lock_guard<std::mutex> lock(mtx);
    status = false;
    condition.notify_all();
    Join();
}
