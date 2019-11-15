#include "common/configParams.h"
#include "libSearchTool/searchTool.h"
#include "writer.h"

using namespace dev;

string Writer::genFileName(const int i)
{
    return path + "/" + std::to_string(i) + "_" + std::to_string(gpuID) + ".fmem";
}

void Writer::writeHeader(std::ofstream &f, const size_t num, const int firstVecId)
{
    static unsigned int header[5] = {0};
    header[0] = dim;
    header[1] = num;
    header[2] = firstVecId;
    f.write((char *)header, sizeof(header));
}
// 校验文件内特征是否和实际提取特征一致
bool Writer::checkData(const string &filename)
{
    std::ifstream is(filename, std::fstream::in | std::fstream::binary);
    if (!is.is_open())
        return false;
    unsigned int header[5] = {0};
    // 读取文件并比对
    is.read((char *)header, sizeof(header));
    if (header[0] != dim || header[1] != featuresPerFile || header[2] % featuresPerFile != 0)
    {
        is.close();
        return false;
    }
    std::vector<float> feature(dim, 0);
    for (size_t i = 0; i < featuresPerFile; ++i)
    {
        is.read((char *)feature.data(), dim * sizeof(float));
        if (i >= features.size() || feature != features[i])
        {
            is.close();
            return false;
        }
    }
    is.close();
    return true;
}
void Writer::writeFile()
{
    size_t num = 0;
    size_t id = offset;
    int fileId = 0;
    std::ofstream id_name_list(path + "/id_name_list_" + std::to_string(gpuID), std::fstream::out);
    std::ofstream featureStream;
    featureStream.open(genFileName(fileId++), std::fstream::out | std::fstream::binary | std::fstream::trunc);
    writeHeader(featureStream, featuresPerFile, id);
    while (!done)
    {
        dev::PhotoMessage msg;
        if (msgQueue->tryGet(msg, 30))
        {
            switch (msg.msgType)
            {
            case SHUTDOWN:
            {
                // 重写文件头,关闭文件
                done = true;
                featureStream.seekp(0);
                writeHeader(featureStream, num, id - num);
                featureStream.close();
                id_name_list.close();
                break;
            }
            default:
            {
                // 写文件
                id_name_list << id << " " << msg.name << std::endl;
                ++id;
                ++num;
                featureStream.write((char *)msg.feature.data(), msg.feature.size() * sizeof(float));
                if (check)
                    features.push_back(msg.feature);
                break;
            }
            }
            if (num == featuresPerFile)
            {
                // 关闭旧文件，生成新文件
                num = 0;
                featureStream.close();
                if (check && !checkData(genFileName(fileId - 1)))
                    std::cerr << "feature check error." << std::endl;
                features.clear();
                featureStream.open(genFileName(fileId++), std::fstream::out | std::fstream::binary | std::fstream::trunc);
                writeHeader(featureStream, featuresPerFile, id);
            }
        }
    }
}

void Writer::writeRedis()
{
    while (!done)
    {
        dev::PhotoMessage msg;
        if (msgQueue->tryGet(msg, 30))
        {
            switch (msg.msgType)
            {
            case SHUTDOWN:
                done = true;
                break;
            default: // redis存储
                string faceID = msg.name.substr(0, msg.name.find_last_of("/"));
                auto pos = faceID.find_last_of("/");
                if (pos != string::npos)
                    faceID = faceID.substr(faceID.find_last_of("/") + 1);
                search->addFace(string("01001"), faceID, msg.feature);
                break;
            }
        }
    }
}

void Writer::doWork(int writeType)
{
    switch (writeType)
    {
    case WRITE_INTO_FILE:
        writeFile();
        break;
    case WRITE_INTO_REDIS:
        writeRedis();
        break;
    default:
        break;
    }
}

Writer::Writer(BlockingQueue<dev::PhotoMessage> *_msgQueue, const string &_path, const ConfigParams *cp, const int len, const int _gputID, int writeType)
    : done(false), msgQueue(_msgQueue), path(_path), featuresPerFile(len), gpuID(_gputID)
{
    if (writeType == WRITE_INTO_REDIS)
    {
        search.reset(new SearchTool(cp->featureSize, cp->redisPasswd.c_str()));
        search->initSearchTool(cp->topK, cp->distance, cp->searchThreshold, cp->redis_db_id);
    }

    // 创建线程
    try
    {
        for (unsigned i = 0; i < 1; ++i)
        {
            threads.push_back(std::thread(&Writer::doWork, this, writeType));
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Thread init failed!" << std::endl;
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Thread init failed!" << std::endl;
    }

}
void Writer::shutdown()
{
    for (auto &th : threads)
    {
        if (th.joinable())
            th.join();
    }
}

void Writer::setOffset(size_t _offset)
{
    offset = _offset;
}

Writer::Writer()
{
}

Writer::~Writer()
{
    done = true;
    shutdown();
}