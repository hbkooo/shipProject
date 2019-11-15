#include "Queue.h"
#include "HandlerFactory.h"
#include "Dispatcher.h"

using namespace MQ;

Dispatcher::Dispatcher()
{
    dqs.resize(INTERFACEID_COUNT);
    bqs.resize(INTERFACEID_COUNT);
}

Dispatcher::~Dispatcher()
{
    dqs.clear();
    bqs.clear();
}

int Dispatcher::dispatchDq(const Message& message_) const
{
    return dqs[message_.msgType] ? dqs[message_.msgType]->push(message_) : -1;
}

int Dispatcher::dispatchBq(const Message& message_) const
{
    return bqs[message_.msgType] ? bqs[message_.msgType]->push(message_) : -1;
}

int Dispatcher::registerDq(int message_type, const std::shared_ptr<DQueue>& mq)
{
    if (dqs[message_type])
        return -1;

    dqs[message_type] = mq;
    return 0;
}

int Dispatcher::registerBq(int message_type, const std::shared_ptr<BQueue>& bq)
{
    if (bqs[message_type])
        return -1;

    bqs[message_type] = bq;
    return 0;
}

void Dispatcher::shutdownDq()
{
    for (auto& dq : dqs) {
        if (!dq)
        dq->shutdown();
    }
}

void Dispatcher::shutdownBq()
{
    for (auto& bq : bqs) {
        if (!bq)
            bq->shutdown();
    }
}
