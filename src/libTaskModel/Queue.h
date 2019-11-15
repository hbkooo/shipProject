#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_
#include <list>
#include <vector>
#include <thread>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <string>
#include <deque>
#include "Dispatcher.h"
#include "HandlerFactory.h"
#include "HandlerBase.h"
#include "common/configParams.h"

namespace MQ
{
    class HandlerBase;
    template <class T>
    class Worker;

    class DQueue
    {
    public:
        explicit DQueue(dev::ConfigParams* cp_,
                        int worker_count_,
                        const std::string& model_path_,
                        int handler_id_,
                        int gpu_num_ = 0);
        ~DQueue();
        int push(const Message& message);
        bool get(Message& message, const uint16_t &time_ms = 1000);
        void shutdown();

    private:
        std::deque<Message>             msg_list;
        std::mutex                          mtx;
        std::condition_variable             condition;
        std::vector<Worker<DQueue>*>        threads;
        bool                                status;
    };

    class BQueue
    {
    public:
        BQueue(int worker_count_,
               const std::string& model_path_,
               int handler_id_,
               int gpu_num_ = 0,
               int capacity_ = 8);
        ~BQueue();

        bool push(const Message &item);
        /**
         * @brief try to get a message from message queue in time_ms, if not get return false
         *
         * @param item
         * @param time_ms
         * @return true get a message in time_ms
         * @return false
         */
        bool get(Message &item, const uint16_t &time_ms = 1000);

        /**
         * @brief get a message from queue, block until get one
         *
         * @return void
         */
        void getNow(Message &item);
        bool isEmptyWithoutLock() { return (head == tail); }
        int size() { return (tail - head + capacity) % capacity; }
        void shutdown();

    private:
        int capacity;
        int head = 0;
        int tail = 0;

        std::vector<Message> queue;

        std::mutex mtx;
        std::condition_variable notFull, notEmpty;
        std::vector<Worker<BQueue>*>        threads;
    }; // class BlockingQueue
}

#endif