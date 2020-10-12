#include <zmq.h>
#include <iostream>
#include <common/json/json.h>
#include <string>
#include <thread>
#include <czmq.h>
#include "common/base64.h"
#include "common/common.h"

#include <time.h>

using std::endl;
using std::cout;
using namespace std;

int num = 0;

static void create_client(void *ReqSock);

static void create_send(void *ReqSock);

int main() {

    //cout << " hbk in main function ... " << endl;
    clock_t start,finish;
    double totaltime;
    start=clock();



    void *context = zmq_ctx_new();
    void *ReqSock = zmq_socket(context, ZMQ_DEALER);
    //zmq_setsockopt(ReqSock, ZMQ_IDENTITY, "asctity", 10);
    zmq_connect(ReqSock, "tcp://127.0.0.1:5563");  //localhost:5559

//    std::string head = "tcp://";
//    std::string server = SERVER;
//    std::string dot = ":";
//    std::string port = PORT;
//    std::string url = head + server + dot + port;
//    zmq_connect(ReqSock,url.c_str());

    // create broker
    auto client = make_shared<thread>(create_client, ReqSock);
    auto clientsend = make_shared<thread>(create_send, ReqSock);

    //cout << " hbk test2 ... " << endl;

    if (client) {
        client->join();
    }

    //cout << " hbk test3 ... " << endl;

    if (clientsend) {
        clientsend->join();
    }

    //cout << " hbk test4 ... " << endl;

    zmq_close(ReqSock);
    zmq_close(context);





    finish=clock();
    totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
    cout << "start : " << start << " , finish : " << finish << ", CLOCKS_PER_SEC : " << CLOCKS_PER_SEC << "\n";
    cout<<"\n此程序的运行时间为"<<totaltime<<"秒！"<<endl;

    return 0;
}


void getResult(char * result) {

    Json::Reader reader;
    Json::Value value;
    if (reader.parse(result,value)) {
        string request_id = value["request_id"].asString();
        int response_id = atoi(value["response_id"].asString().c_str());
        Json::Value res = value["result"];
        int result_nums = atoi(value["result_nums"].asString().c_str());
        int time_used = atoi(value["time_used"].asString().c_str());


        std::cout << "request_id : " << request_id << "\n";
        std::cout << "response_id : " << response_id << "\n";
        std::cout << "time_used : " << time_used << "\n";
        std::cout << "result_nums : " << result_nums << "\n";
        std::cout << "label \t score \t\t x1 \t x2 \t x3 \t x4 \t y1 \t y2 \t y3 \t y4 \n";
        Json::Value location;
        for (int i = 0; i < result_nums; ++i) {
            location = res[i];
            int label = atoi(location["label"].asString().c_str());
            float score = atof(location["score"].asString().c_str());
            float x1 = atof(location["x1"].asString().c_str());
            float x2 = atof(location["x2"].asString().c_str());
            float x3 = atof(location["x3"].asString().c_str());
            float x4 = atof(location["x4"].asString().c_str());
            float y1 = atof(location["y1"].asString().c_str());
            float y2 = atof(location["y2"].asString().c_str());
            float y3 = atof(location["y3"].asString().c_str());
            float y4 = atof(location["y4"].asString().c_str());
            std::cout << label << " \t " << score << " \t " << x1 << " \t " << x2 << " \t " << x3 << " \t " << x4
                      << " \t " << y1 << " \t " << y2 << " \t " << y3 << " \t " << y4 << " \n ";
        }

    }

}

static void create_client(void *ReqSock)
{
    //cout << "hbk in create_client : test0 ... " << endl;
    zmq_pollitem_t items[] = {{ReqSock, 0, ZMQ_POLLIN, 0}};
    while (1) {
        {
            zmq_poll(items, 1, 10 * ZMQ_POLL_MSEC);
            //cout << "hbk in create_client : test1 ... " << endl; 
            if (items[0].revents & ZMQ_POLLIN)
            {
                zmsg_t *msg = zmsg_recv(ReqSock);
                zframe_t *cont = zmsg_pop(msg);
                
                cout << "receive the response ... " << endl;
                
                if (cont) {
                    char *cont2 = zframe_strdup(cont);
                    cout<<cont2<<endl;

                    //getResult(cont2);

                    delete[] cont2;
                    cont2 = nullptr;
                }
                zmsg_destroy(&msg);
                num ++;
                if (num >= 500) break;
                //break;
            }
        }
    }
}


void create_send(void *ReqSock) {
    /*-------------------------------读图�?-------------------------------------*/
    //cout << "hbk in create_send : test0 ... " << endl;
    //FILE *pFile;
//    long lSize;
//    char *buffer;
//    size_t result;
//
//    const char* yolo_img = "/home/hbk/data/image/helmet/4.jpg";
//    const char* sar_img = "/home/hbk/data/image/SARImage/Img118.bmp";
//    const char* ship_img = "/home/hbk/data/image/dota/ship_are.jpg";//ship_are.jpg  ships_1105_2
//
//    pFile = fopen(ship_img, "rb");
//    if (pFile == NULL)
//        fputs("File error", stderr);
//    //cout << "hbk in create_send : test1 ... " << endl;
//    fseek(pFile, 0, SEEK_END);
//    lSize = ftell(pFile);
//    rewind(pFile);
//    buffer = new char[lSize];
//    if (buffer == NULL)
//        fputs("Memory error", stderr);
//    result = fread(buffer, 1, lSize, pFile);
//    if (result != lSize)
//        fputs("Reading error", stderr);
//    //cout << "hbk in create_send : test2 ... " << endl;
//    /*----------------------------base64编码------------------------------------*/
//    string imgBase64 = dev::base64_encode(buffer, lSize);
//    /*------------------------------Json打包-------------------------------------*/
//
////    //1.detect yolo (Interface_ID : 1)
////    Json::Value root;
////    root["api_key"] = "";
////    root["interface"] = "5";  //SAR:7  SHIP:6  YOLO:5
////    root["image_base64"] = imgBase64;
////    std::string tmpstr = root.toStyledString();
//
////    //2.detect ship (Interface_ID : 2)
////    Json::Value root;
////    root["api_key"] = "";
////    root["interface"] = "6";  //SAR:7  SHIP:6  YOLO:5
////    root["image_base64"] = imgBase64;
////    std::string tmpstr = root.toStyledString();
//
////    //3.detect SAR ship (Interface_ID : 3)
//    Json::Value root;
//    root["api_key"] = "";
//    root["interface"] = 6;
//    root["image_base64"] = imgBase64;
//    std::string tmpstr = root.toStyledString();


    /*------------------------------发送数�?------------------------------------*/
//    std::cout << "open file ..." << std::endl;

    const char* yolo_img = "/home/hbk/data/image/files.txt";
    const char* sar_img = "/home/hbk/data/image/SARImage/files.txt";
    const char* ship_img = "/home/hbk/data/image/dota/files.txt";

    FILE * img_path = fopen(yolo_img,"r");
    FILE *img_file;
    if (!img_path) {
        std::cout << "open file failed" << std::endl;
        return;
    }

//    std::cout << "test1 ... \n";

    char path[100];
    long lSize;
    char *buffer;
    size_t result;

    while (!feof(img_path)) {

        fscanf(img_path, "%s", &path);
        //std::cout << "path : " << path << "\n";

        img_file = fopen(path, "rb");
        if (img_file == NULL)
            fputs("File error", stderr);
        fseek(img_file, 0, SEEK_END);
        lSize = ftell(img_file);
        rewind(img_file);
        buffer = new char[lSize];
        if (buffer == NULL)
            fputs("Memory error", stderr);
        result = fread(buffer, 1, lSize, img_file);
        if (result != lSize)
            fputs("Reading error", stderr);
        //cout << "hbk in create_send : test2 ... " << endl;
        /*----------------------------base64编码------------------------------------*/
        string imgBase64 = dev::base64_encode(buffer, lSize);

        Json::Value root;
        root["api_key"] = "";
        root["interface"] = 1;
        root["image_base64"] = imgBase64;
        std::string tmpstr = root.toStyledString();

        zstr_sendf(ReqSock, tmpstr.c_str());
        fclose(img_file);

    }

    fclose(img_path);

    //cout << tmpstr.c_str();
    delete[] buffer;
    buffer = nullptr;
    
    //cout << "hbk in create_send : test4 loading over ... " << endl;
    
}
