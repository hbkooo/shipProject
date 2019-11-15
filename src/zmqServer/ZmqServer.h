#ifndef ZMQ_SERVER_H_
#define ZMQ_SERVER_H_

#include <string>
#include <algorithm>
#include <vector>
#include "common/configParams.h"
#include "libTaskModel/Dispatcher.h"
#include "ZmqMessage.h"

namespace dev
{
class TokenBucket;
}

class ZmqWorker;

class ZmqServer
{
  public:
    ZmqServer(dev::ConfigParams *_cp);

    ~ZmqServer();

    void start();

    void shutdown();

    friend ZmqWorker;

  private:
    std::vector<ZmqWorker*>                     threads;
    dev::ConfigParams*                          cp;
    std::shared_ptr<MQ::Dispatcher>             dispatcher;
};

#endif