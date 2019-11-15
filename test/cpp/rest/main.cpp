#include <boost/filesystem.hpp>
#include "common/configParams.h"
#include "common/json/json.h"
#include "libAuth/modelCrypto.h"
#include "handler.h"

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <chrono>

using namespace std;
using namespace dev;

static string defaultConfig = "{\
\"apiKey\": \"\",\
\"listenip\": \"0.0.0.0\",\
\"port\": 33388,\
\"gpuid\": 0,\
\"httpThreads\": 2,\
\"caffeThreads\": 4,\
\"maxRequests\": 10,\
\"modelpath\": \"model/\",\
\"logconf\": \"log.conf\",\
\"searchToolParams\": {\
\"redis_db_id\": 5,\
\"redis_passwd\": \"\",\
\"topK\": 80,\
\"distance\": 250,\
\"simThreshold\": 0.66,\
\"searchThreshold\": 0.75}}";

void help()
{
    cout << "Usage: ./wuhanTest [image-list filename] [section size] [GPU id] [id offset] [write destination(0:file,1:redis)]" << endl;
    cout << "Example : ./wuhanTest image_list.base 10000 1 5000 0" << endl;
}

int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        help();
        return -1;
    }
    using namespace boost;
    const string imageList(argv[1]);
    const int blockSize = atoi(argv[2]);
    const int GPUId = atoi(argv[3]);
    const size_t offset = (unsigned)atoi(argv[4]);
    const int writeType = atoi(argv[5]);
    std::shared_ptr<dev::ConfigParams> cp = nullptr;

    try
    {
        cp.reset(new ConfigParams("config.json"));
    }
    catch (std::exception &e)
    {
        cerr << e.what() << endl;
        return -1;
    }

    cp->printParams();
    string type = imageList.substr(imageList.find_last_of(".") + 1);
    string outputDir = string("data/") + type + "Feature/";
    string photoDir = string("data/") + type + "Photo/";
    cout << "Photos dir  : " << photoDir << endl;
    cout << "Feature dir : " << outputDir << endl;
    filesystem::create_directories(outputDir);
    // filesystem::permissions(outputDir, filesystem::add_perms | filesystem::owner_write | filesystem::group_write | filesystem::others_write);
    Handler handler(cp.get(), type, blockSize, writeType);
    handler.setWriterOffset(offset);

    auto start = chrono::steady_clock::now();

    vector<string> imageLists;
    ifstream fin(imageList, ios::in);
    string imageName;

    while (getline(fin, imageName))
        imageLists.push_back(imageName);

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Read image list cost time : " << double(duration.count()) << "ms."
         << " Total : " << imageLists.size() << endl;

    start = chrono::steady_clock::now();
    size_t num = 0;

    for (auto &image : imageLists)
    {
        handler.GetFeature(image);
        auto tmpTP = chrono::steady_clock::now();
        auto t = chrono::duration_cast<chrono::milliseconds>(tmpTP - start);
        cout << right << setw(10) << "Processed: " << ++num << setw(10) << " TimeUsed: " << t.count() << "ms" << "\r";
    }
    cout << endl;
    fin.close();
    handler.shutdown();
    end = chrono::steady_clock::now();

    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Total : " << num << ". cost : " << double(duration.count())/36000000 << "hours." << endl;

    return 0;
}
