#include <iostream>
#include <fstream>
#include <zmq.h>
#include <thread>
#include <boost/filesystem.hpp>
#include "common/json/json.h"
#include "common/easylog++.h"

#define CONFIG_ERROR "Config file error. "
#define printError()    printf("%s[%d]: %d: %s\n", __FILE__, __LINE__, \
zmq_errno(), zmq_strerror(zmq_errno()));

static void create_router(std::string front_addr, std::string back_addr);
static void create_router_person(std::string front_addr, std::string back_addr);


bool generateDefaultConfig(std::string const &configPath)
{
    Json::Value routerConfig;
    routerConfig["front_addr"] = "tcp://*:5563";
    routerConfig["back_addr"] = "tcp://*:5564";

    Json::Value config;
    config["routerParams"] = routerConfig;

    Json::StyledWriter writer;
    std::string document = writer.write(config);

    std::ofstream os(configPath);
    if (!os.is_open())
        return false;
    os << document;
    os.close();
    return true;
}

int main(int argc, char *argv[])
{
    int major, minor, patch;
    zmq_version (&major, &minor, &patch);
    printf("ØMQ version   : %d.%d.%d \n",major, minor, patch);
    std::string configPath("router.json");
    if (argc == 2)
    {
        configPath = argv[1];
        std::cout << "Config file   : " << configPath <<std::endl;
    }
    else if (argc == 1)
    {
        std::cout << "Config file   : router.json" <<std::endl;
    }
    else
    {
        return -1;
    }

    std::string front_addr;
    std::string back_addr;
    std::string front_addr_person;
    std::string back_addr_person;
    if (!boost::filesystem::exists(configPath))
    {
        if (!generateDefaultConfig(configPath))
            std::cout << "Generate default config file failed." << std::endl;
        front_addr = "tcp://*:5563";
        back_addr = "tcp://*:5564";
        std::cout << "Using default config parameters." << std::endl;
    }
    else
    {
        std::ifstream in(configPath);
        if (!in.is_open())
            return -1;
        Json::Reader reader;
        Json::Value config;

        if (!reader.parse(in, config, false) || !config.isObject())
        {
            LOG(ERROR) << "Config file syntax error.";
            throw std::invalid_argument("Config file syntax error.");
        }
        in.close();

        if (config.isMember("routerParams") && config["routerParams"].isObject())
        {
            if (config["routerParams"].isMember("front_addr"))
                front_addr = config["routerParams"]["front_addr"].asString();
            else
                LOG(ERROR) << CONFIG_ERROR << "Missing front_addr.";

            if (config["routerParams"].isMember("back_addr"))
                back_addr = config["routerParams"]["back_addr"].asString();
            else
                LOG(ERROR) << CONFIG_ERROR << "Missing back_addr.";
        }
        else
            LOG(ERROR) << CONFIG_ERROR << "Missing routerParams.";
    }

    printf("front_addr             : %s \n",front_addr.c_str());
    printf("back_addr              : %s \n",back_addr.c_str());

    auto broker_verifier = std::make_shared<std::thread>(create_router, front_addr, back_addr);

    if (broker_verifier) {
        broker_verifier->join();
    }

    return 0;
}

void create_router(std::string front_addr, std::string back_addr)
{
    void *context = zmq_ctx_new();
    void* frontEnd = zmq_socket(context, ZMQ_ROUTER);
    void* backEnd = zmq_socket(context, ZMQ_DEALER);
    bool bRet = true;
    int r = zmq_bind(frontEnd, front_addr.c_str());
    if (r!=0) {
        bRet = false;
        std::cout<<"error:router_verifier frontEnd bind failure"<<std::endl;
    }
    r = zmq_bind(backEnd, back_addr.c_str());
    if (r!=0) {
        bRet = false;
        std::cout<<"error:router_verifier backEnd bind failure"<<std::endl;
    }
    if (bRet)
        printf("verifier_router started ...\n");


    zmq_proxy(frontEnd, backEnd, nullptr);


#if 0
    zmq_pollitem_t items[] =
    {
        { frontend, 0, ZMQ_POLLIN, 0 },
        { backend, 0, ZMQ_POLLIN, 0 },
    };
    while (true)
    {
        std::cout << "hbk ==> in while true" << std::endl;
        zmq_msg_t message;
        r = zmq_poll(items, 2, -1);
        if (r == -1)
        {
            printError();
            break;
        }
        if (items[0].revents & ZMQ_POLLIN)
        {
            while (true)
            {
                zmq_msg_init(&message);
                r = zmq_msg_recv(&message, frontend, 0);
                if (r == -1)
                {
                    printError();
                    break;
                }
                int more = zmq_msg_more(&message);
                r = zmq_msg_send(&message, backend, more ? ZMQ_SNDMORE : 0);
                if (r == -1)
                {
                    printError();
                    zmq_msg_close(&message);
                    break;
                }
                zmq_msg_close(&message);
                if (!more)
                    break;
            }
        }
        if (items[1].revents & ZMQ_POLLIN)
        {
            while (true)
            {
                zmq_msg_init(&message);
                r = zmq_msg_recv(&message, backend, 0);
                if (r == -1)
                {
                    printError();
                    zmq_msg_close(&message);
                    break;
                }
                int more = zmq_msg_more(&message);
                r = zmq_msg_send(&message, frontend, more ? ZMQ_SNDMORE : 0);
                if (r == -1)
                {
                    printError();
                    zmq_msg_close(&message);
                    break;
                }
                zmq_msg_close(&message);
                if (!more)
                    break;
            }
        }
    }
#endif

    printf("verifier_router ended ...\n");
    zmq_close(frontEnd);
    zmq_close(backEnd);
    zmq_ctx_destroy(context);
}