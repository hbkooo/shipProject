#ifndef HANDLER_H_
#define HANDLER_H_
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include "common/blockQueue.h"
#include "photoMessage.h"
#include "writer.h"

namespace Json
{
class Value;
};

namespace dev
{
class CaffePool;
class ConfigParams;
class Handler
{

public:
  Handler(const dev::ConfigParams *_cp, const std::string &type, const int featuresPerFile = 1000000, int writeType = WRITE_INTO_FILE);
  ~Handler() = default;
  void GetFeature(std::string const &photoName);
  void shutdown();
  void setWriterOffset(size_t offset);
  int queueFactor = 40;

private:
  std::string photoDir;
  const dev::ConfigParams *cp = nullptr;
  std::shared_ptr<CaffePool> caffeThreads = nullptr;
  std::shared_ptr<Writer> writer = nullptr;
  BlockingQueue<PhotoMessage> procQueue;
  BlockingQueue<PhotoMessage> writeQueue;
};
};

#endif //HANDLER_H_