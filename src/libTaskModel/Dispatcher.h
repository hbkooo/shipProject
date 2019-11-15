#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_

#include <map>
#include <memory>
#include <string>
#include "common/blockQueue.h"
#include "common/common.h"
#include "HandlerBase.h"

namespace MQ {
    class HandlerFactory;
    class DQueue;
    class BQueue;
    class Dispatcher
    {
    public:
        Dispatcher();
        ~Dispatcher();
        int dispatchDq(const Message& message_) const;
        int dispatchBq(const Message& message_) const;
        int registerDq(int message_type, const std::shared_ptr<DQueue>& mq);
        int registerBq(int message_type, const std::shared_ptr<BQueue>& bq);

        void shutdownDq();
        void shutdownBq();

    private:
        std::vector<std::shared_ptr<DQueue> > dqs;
        std::vector<std::shared_ptr<BQueue> > bqs;
    };
}
#endif