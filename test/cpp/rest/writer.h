#ifndef WRITER_H
#define WRITER_H

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <memory>
#include "common/blockQueue.h"
#include "photoMessage.h"
#include <fstream>

#define WRITE_INTO_FILE 0
#define WRITE_INTO_REDIS 1

using std::string;

class SearchTool;
namespace dev
{
class ConfigParams;

class Writer
{
protected:
  std::vector<std::thread> threads;
  std::atomic<bool> done;
  BlockingQueue<dev::PhotoMessage> *msgQueue;
  std::string path;
  size_t featuresPerFile;
  size_t offset = 0;
  int gpuID;
  unsigned int dim = 512;
  std::vector<std::vector<float>> features;
  bool check = false; //检测写入数据
  std::shared_ptr<SearchTool> search = nullptr;

  string genFileName(const int i);
  void writeHeader(std::ofstream &f, const size_t num, const int firstVecId = 0);
  // 校验文件内特征是否和实际提取特征一致
  bool checkData(const string &filename);
  void writeFile();
  void writeRedis();
  void doWork(int writeType);

public:
  Writer(BlockingQueue<dev::PhotoMessage> *_msgQueue, const string &_path, const ConfigParams *cp,
         const int len, const int _gputID = 0, int writeType = WRITE_INTO_FILE);
  void shutdown();
  void setOffset(size_t _offset = 0);
  Writer();
  ~Writer();
};
}
#endif // !WRITER_H
