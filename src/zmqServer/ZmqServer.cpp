#include "ZmqServer.h"
#include <thread>
#include "libTaskModel/Queue.h"
#include "common/easylog++.h"
#include "zmq.h"
#include "czmq.h"
#include "common/json/json.h"
#include "common/common.h"
#include "common/error.h"
#include "ZmqSender.h"

using namespace std;
class ZmqWorker
{
public:
    ZmqWorker(const dev::ConfigParams *cp_, std::shared_ptr<MQ::Dispatcher> dispatcher_, void *context_)
            : cp(cp_), dispatcher(dispatcher_),context(context_)
    {
        thread = new std::thread(&ZmqWorker::run, this);
    }

    ~ZmqWorker() {
        delete thread;
        thread = nullptr;
    }

    void run()
    {
        void* rspSock = zmq_socket(context, ZMQ_DEALER);
        int r = zmq_connect(rspSock, cp->zmqAddr.c_str());
        std::cout << "ZmqServer ZmqWorker address is : " << cp->zmqAddr.c_str() << "\n";
        if (r == -1) {
            LOG(ERROR) << "zmq_connect failure ";
            zmq_close(rspSock);
            return;
        }

        void* rspSockSend = zmq_socket(context,  ZMQ_DEALER);
        int rt = zmq_bind (rspSockSend, "inproc://#1");
        if (rt == -1) {
            zmq_close(rspSockSend);
            return;
        }

        zmqSender = std::make_shared<ZmqSender>(context, "inproc://#1");
        zmq_pollitem_t items[] = { {rspSock, 0, ZMQ_POLLIN, 0} ,
                                   {rspSockSend, 0, ZMQ_POLLIN, 0}};
        while (true) {
            int rc = zmq_poll(items, 2, 10 * ZMQ_POLL_MSEC );
            if (rc == -1) {
                LOG(ERROR) << "Error zmq_poll return -1: " << zmq_strerror(errno);
                break;
            }

            if (items [1].revents & ZMQ_POLLIN) {
                zmsg_t *msg = zmsg_recv(rspSockSend);
                zframe_t *addr = zmsg_pop(msg);
                zframe_t *cont = zmsg_pop(msg);
                zframe_send(&addr, rspSock, ZFRAME_REUSE + ZFRAME_MORE);
                zframe_send(&cont, rspSock, ZFRAME_REUSE);
                if (addr)
                    zframe_destroy(&addr);
                if (cont)
                    zframe_destroy(&cont);
                zmsg_destroy(&msg);
            }

            if (items [0].revents & ZMQ_POLLIN) {

                zmsg_t *msg = zmsg_recv(rspSock);
                zframe_t *addr = zmsg_pop(msg);
                zframe_t *cont = zmsg_pop(msg);
                assert (cont);

                ///��ʹ��zframe_strdup
                assert (zframe_is (cont));
                size_t size = zframe_size (cont);
                char *contstr = (char *) malloc (size + 1);
                assert (contstr);
                memcpy (contstr, zframe_data (cont), size);
                contstr [size] = 0;

                //char* contstr = zframe_strdup(cont);
                if (strlen(contstr) > 0)
                {
                    //�����߼�
                    Json::Reader reader;
                    Json::Value value;
                    if (reader.parse(contstr, value)) {
                        int interface = atoi(value["interface"].asString().c_str());
                        std::cout << "\nget one message in zmqServer and interface is " << interface << "\n";
                        if (interface >= 0 && interface < INTERFACEID_COUNT){
                            LOG(DEBUG) << "contstr: "<< contstr << std::endl;
                            MQ::ZmqMessage zmqmsg(zmqSender.get(), interface, contstr, 0, addr);
                            int result = dispatcher->dispatchDq(zmqmsg);
                            //std::cout << "dispatcher result is " << result << "\n";
                            if (result == -1) {
                                // the service is not start
                                std::string response = fillResponse("This service is not start, please contact the administrator ...",FINDER_NO_SERVICE);
                                zframe_send(&addr, rspSock, ZFRAME_REUSE + ZFRAME_MORE);
                                zframe_reset(cont, response.c_str(), response.size() + 1);
                                zframe_send(&cont, rspSock, ZFRAME_REUSE);
                                zframe_destroy(&addr);
                                zframe_destroy(&cont);
                            }

                        } else{
                            std::string response = fillResponse("Json参数错误，interface_id不对(0-9)",PARAMS_ERROR);
                            zframe_send(&addr, rspSock, ZFRAME_REUSE + ZFRAME_MORE);
                            zframe_reset(cont, response.c_str(), response.size()+1);
                            zframe_send(&cont, rspSock, ZFRAME_REUSE);
                            zframe_destroy(&addr);
                            zframe_destroy(&cont);
                        }
                    }
                    else {
                        std::string response = fillResponse("Json参数错误，缺少参数或类型不对",PARAMS_ERROR);
                        zframe_send(&addr, rspSock, ZFRAME_REUSE + ZFRAME_MORE);
                        zframe_reset(cont, response.c_str(), response.size()+1);
                        zframe_send(&cont, rspSock, ZFRAME_REUSE);
                        zframe_destroy(&addr);
                        zframe_destroy(&cont);
                    }
                }
                if (cont)
                    zframe_destroy(&cont);
                zmsg_destroy(&msg);
                if (contstr) {
                    free(contstr);
                    contstr = nullptr;
                }
            }
        }
        zmq_close(rspSock);
        zmq_close(rspSockSend);
    }
    void Join()
    {
        if (thread->joinable())
            thread->join();
    }

    std::string fillResponse(const char *errorMsg, const char *rep_id)
    {
        Json::Value response;
        response["response_id"] = rep_id;
        response["request_id"] = "";
        response["time_used"] = 0;
        if (errorMsg)
            response["error_message"] = errorMsg;
        Json::FastWriter w;
        return w.write(response);
    }

    std::string fillResponse(const char *errorMsg)
    {
        Json::Value response;
        response["request_id"] = "";
        response["time_used"] = 0;
        if (errorMsg)
            response["error_message"] = errorMsg;
        Json::FastWriter w;
        return w.write(response);
    }

private:
    const dev::ConfigParams*        cp = nullptr;
    std::thread*                    thread;
    std::shared_ptr<MQ::Dispatcher> dispatcher;
    void*                           context;
    std::shared_ptr<ZmqSender>      zmqSender;
};

ZmqServer::ZmqServer(dev::ConfigParams *_cp)
: cp(_cp)
{
    //std::cout << "hbk in ZmqServer.cpp() ... \n" ;
    dispatcher = std::make_shared<MQ::Dispatcher>();

#ifdef USE_SHIP
    //std::cout << "build with ship ... \n";
    if (cp->detectShipPoolSize > 0) {
        // ship detect
        std::shared_ptr<MQ::DQueue> shipQueue
                = std::make_shared<MQ::DQueue>(cp, cp->detectShipPoolSize, cp->modelPath, SHIP_METHOD_ID, cp->GPUDevices);
        dispatcher->registerDq(INTERFACEID_SHIP, shipQueue);
    }
#endif

#ifdef USE_SAR
    if (cp->detectSarPoolSize > 0) {
        // sar detect
        std::shared_ptr<MQ::DQueue> sarQueue
                = std::make_shared<MQ::DQueue>(cp, cp->detectSarPoolSize, cp->modelPath, SAR_METHOD_ID, cp->GPUDevices);
        dispatcher->registerDq(INTERFACEID_SAR, sarQueue);
    }
#endif

#ifdef USE_YOLO
    //std::cout << "build with yolo ... \n";
    if (cp->detectYoloPoolSize > 0) {
        // yolo detect
        std::shared_ptr<MQ::DQueue> yoloQueue
                = std::make_shared<MQ::DQueue>(cp, cp->detectYoloPoolSize, cp->modelPath, YOLO_METHOD_ID, cp->GPUDevices);
        dispatcher->registerDq(INTERFACEID_YOLO, yoloQueue);
    }
#endif

}

ZmqServer::~ZmqServer()
{
    for (ZmqWorker* thread : threads){
        delete thread;
        thread = nullptr;
    }

}

void ZmqServer::start()
{

    void *context = zmq_ctx_new();

    //threads2.push_back(new ZmqWorker2(cp, dispatcher, context));
    threads.push_back(new ZmqWorker(cp, dispatcher, context));

    //for (ZmqWorker2* thread : threads2)
    //    thread->Join();

    for (ZmqWorker* thread : threads)
        thread->Join();
}

void ZmqServer::shutdown()
{
    dispatcher->shutdownDq();
}
