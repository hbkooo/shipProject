#include <stdio.h>
#include <string.h>
#include <boost/filesystem.hpp>
#include "ZmqServer.h"
#include "common/configParams.h"
#include "common/easylog++.h"
#include "common/json/json.h"

using namespace std;
using namespace dev;

void help() {
    cout << "Usage verifier [config.json]" << endl;
}

int main(int argc, char *argv[]) {
    int major, minor, patch;
    zmq_version(&major, &minor, &patch);
    printf("ØMQ version   : %d.%d.%d \n", major, minor, patch);
   
    string configPath("config.json");
    if (argc == 2) {
        configPath = argv[1];
        cout << "Config file   : " << configPath << endl;
    } else if (argc == 1) {
        cout << "Config file   : config.json" << endl;
    } else {
        help();
        return -1;
    }

    std::shared_ptr<dev::ConfigParams> cp = nullptr;
    try {
        cp.reset(new ConfigParams(configPath));
    } catch (std::exception &e) {
        cerr << e.what() << endl;
        return -1;
    }

    cp->printParams();

    try {
        ZmqServer zmqServer(cp.get());
        zmqServer.start();
        zmqServer.shutdown();
    } catch (std::exception &e) {
        LOG(ERROR) << e.what();
        LOG(ERROR) << "Please check redis service or port " << cp->port;
        return -1;
    }
    return 0;
}
