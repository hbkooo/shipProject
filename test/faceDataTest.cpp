#include <iostream>
#include <vector>
#include <ctime>
#include <iomanip>
#include "libFaceData/faceDataHandler.h"
#include "libFaceData/faceDataUploader.h"
#include "common/easylog++.h"

INITIALIZE_EASYLOGGINGPP

using namespace facedata;
using namespace std;

const vector<string> columns = {"id", "requset id", "user id", "group id", "is uploaded", "interface ", "image-1", "image-2", "image-3", "image-4", "image-5"};

void showQueryResult(const vector<vector<string>> &queryResults)
{

    for (auto &queryResult : queryResults)
    {
        cout << "========BEGIN========" << endl;
        for (size_t i = 0; i < columns.size(); ++i)
        {
            cout << setw(12) << columns.at(i) << " : " << queryResult.at(i) << endl;
        }
        cout << "========END========" << endl;
    }
}

void checkRet(const bool &ret, const string &OKmsg, const string &ERRORmsg)
{

    string msg = ret ? OKmsg : ERRORmsg;
    cout << msg << endl;
}

int main()
{

    const string requestId1 = "000001";
    const string requestId2 = "000001";
    const string userId = "sunhao";
    const string groupId = "hust";
    const vector<string> images(4, "jpegImage");
    bool ret;

    FaceDataHandler mFaceDataHandler;
    vector<vector<string>> queryResults;
    // store data into table
    ret = mFaceDataHandler.storeFaceData(requestId1, groupId, userId, images);
    checkRet(ret, "Stroe data successed!", "store data failed!");
    ret = mFaceDataHandler.storeFaceData(requestId2, groupId, userId, images);
    checkRet(ret, "Stroe data successed!", "store data failed!");
    //query all data from table 'face'
    ret = mFaceDataHandler.queryFaceData(queryResults);
    checkRet(ret, "Query all data successed!", "Query all data failed!");
    showQueryResult(queryResults);
    queryResults.clear();
    //set 'isUploaded' to 1
    ret = mFaceDataHandler.updateUploadStatus(requestId1);
    checkRet(ret, "Update upload staus successed!", "Update upload staus  failed!");
    //query all data from table 'face'
    ret = mFaceDataHandler.queryFaceData(queryResults);
    checkRet(ret, "Query all data successed!", "Query all data failed!");
    showQueryResult(queryResults);
    queryResults.clear();
    ret = mFaceDataHandler.queryNewFaceData(queryResults);
    checkRet(ret, "Query new data successed!", "Query new data failed!");
    showQueryResult(queryResults);
    queryResults.clear();

    cout << "========Start Test upload===========" << endl;
    FaceDataUploader mFaceDataUploader("192.168.1.5:8989");
    mFaceDataUploader.doUpload();
    return 0;
}