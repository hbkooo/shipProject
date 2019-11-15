#include <iostream>
#include <algorithm>
#include <memory>
#include <assert.h>
#include "Queue.h"

// caffe
#include <caffe/caffe.hpp>
#include <caffe/layers/memory_data_layer.hpp>

using namespace MQ;

namespace MQ {
    template <class T>
    class Worker
    {
    public:
        Worker(T & queue_, int gpu_id_, int gpu_num_,
               int handler_id_, const std::string& model_path_,
               dev::ConfigParams* cp_ = nullptr)
                : queue(queue_), gpu_id(gpu_id_), gpu_num(gpu_num_),
                  handler_id(handler_id_), model_path(model_path_),
                  status(true), cp(cp_)
        {
            //std::cout << "hbk in work() and start to create ... \n";
            thread = new std::thread(&Worker::run, this);
            //std::cout << "hbk in work() and end create ... \n";
        }

        ~Worker()
        {
            if (thread) {
                delete thread;
                thread = nullptr;
            }
        }

        void run()
        {
#ifdef USE_CAFFE_CPU
            std::cout << "Worker::Run CPU" << std::endl;
            caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
            std::cout << "Worker::Run GPU" << std::endl;
            caffe::Caffe::set_mode(caffe::Caffe::GPU);
            caffe::Caffe::SetDevice(gpu_id);
            caffe::Caffe::set_solver_count(gpu_num);
#endif
            //std::cout << "hbk in work::run() ... \n";
            Message message;
            std::shared_ptr<HandlerBase> handler =
                    HandlerFactory::instance()->create(handler_id, model_path, cp);
            while (status) {
                if (queue.get(message)) {
                    //std::cout << "\nget one message in Queue and message type is : " << message.msgType << "\n";
                    std::string response;
                    response = handler->handle(message);
                    message.call(response);
                    //std::cout << "have done the detect and the result is : \n" << response << "\n\n";
                }
            }
        }

        void shutdown()
        {
            status = false;
            join();
        }


    private:
        void join()
        {
            if (thread->joinable())
                thread->join();
        }

        T&                  queue;
        std::thread*        thread;
        int                 gpu_id;
        int                 gpu_num;
        int                 handler_id;
        std::string         model_path;
        bool                status;
        dev::ConfigParams*  cp;
    };

}

DQueue::DQueue(dev::ConfigParams* cp_, int worker_count_, const std::string& model_path_,
               int handler_id_, int gpu_num_) : status(true)
{
    HandlerFactory::instance();
    // assert(workerCount_ > 0);
    try {
        //std::cout << "hbk in DQueue()... \n";
        for (int i = 0; i < worker_count_; i++)
            threads.push_back(
                    new Worker<DQueue>(*this, gpu_num_ > 1 ? i%gpu_num_ : 0,
                                       gpu_num_, handler_id_, model_path_, cp_));
        //std::cout << "hbk in DQueue() and end ... \n";
    } catch (std::exception &e) {
        LOG(ERROR) << "Thread init failed!";
        LOG(ERROR) << e.what();
    } catch (...) {
        LOG(ERROR) << "Thread init failed!";
    }
}

DQueue::~DQueue()
{
    shutdown();
    for (auto it : threads) {
        if (!it) {
            delete it;
            it = 0;
        }
    }
    threads.clear();
}

int DQueue::push(const Message& message)
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        msg_list.emplace_back(message);
    }
    condition.notify_one();
    return 0;
}

bool DQueue::get(Message& message, const uint16_t &time_ms)
{
    std::unique_lock<std::mutex> lock(mtx);
    if (!status)
        return false;

    if (!condition.wait_for(lock, std::chrono::milliseconds(time_ms), [&] { return !msg_list.empty(); }) )
        return false;

    // while (msg_list.empty())
    //    condition.wait(lock);

    //std::cout << "hbk in DQueue::get() : \n";
    //std::cout << "before pop msg_list.size() : " << msg_list.size() << "\n";

    message = msg_list.front();
    msg_list.pop_front();

    //std::cout << "after pop msg_list.size() : " << msg_list.size() << "\n";

    return true;
}

void DQueue::shutdown()
{
    std::lock_guard<std::mutex> lock(mtx);
    status = false;
    condition.notify_all();

    for (auto it : threads) {
        if (!it) {
            it->shutdown();
        }
    }
}


BQueue::BQueue(int worker_count_,
               const std::string& model_path_,
               int handler_id_,
               int gpu_num_,
               int capacity_)
        : capacity(capacity_), queue(std::vector<Message>(capacity))
{
    HandlerFactory::instance();
    // assert(workerCount_ > 0);
    try {
        for (int i = 0; i < worker_count_; i++)
            threads.push_back(
                    new Worker<BQueue>(*this, gpu_num_ > 1 ? i%gpu_num_ : 0,
                                       gpu_num_, handler_id_, model_path_));
    } catch (std::exception &e) {
        LOG(ERROR) << "Thread init failed!";
        LOG(ERROR) << e.what();
    } catch (...) {
        LOG(ERROR) << "Thread init failed!";
    }
}

BQueue::~BQueue()
{
    shutdown();
    // join();
    for (auto it : threads) {
        if (!it) {
            delete it;
            it = 0;
        }
    }
    threads.clear();
}

bool BQueue::push(const Message &item)
{
    {
        std::unique_lock<std::mutex> lck(mtx);
        notFull.wait(lck, [&] { return (tail + 1) % capacity != head; });
        // if (!notFull.wait_for(lck, std::chrono::milliseconds(1000), [&] { return (tail + 1) % capacity != head; }))
        //     return false;

        // while ((tail + 1) % capacity == head) //is full
        //     notFull.wait(lck);
        queue[tail] = item;
        tail = (tail + 1) % capacity;
    }

    //wake up get thread
    notEmpty.notify_one();

    return true;
}

bool BQueue::get(Message &msg, const uint16_t &time_ms)
{
    {
        std::unique_lock<std::mutex> lck(mtx);
        if (!notEmpty.wait_for(lck, std::chrono::milliseconds(time_ms), [&] { return head != tail; }))
            return false;
        msg = queue[head];
        head = (head + 1) % capacity;
    }
    notFull.notify_one();
    return true;
}

void BQueue::getNow(Message& msg)
{
    {
        std::unique_lock<std::mutex> lck(mtx);
        notEmpty.wait(lck, [&] { return head != tail; });
        // while (head == tail) // is empty
        //     notEmpty.wait(lck);
        msg = queue[head];
        head = (head + 1) % capacity;
        DEBUGLOG(std::cout << "get..." << std::endl);
    }

    //wake up push thread
    notFull.notify_one();
}

void BQueue::shutdown()
{
    std::lock_guard<std::mutex> lock(mtx);

    for (auto it : threads) {
        if (!it) {
            it->shutdown();
        }
    }

    notFull.notify_all();
    notEmpty.notify_all();
}
