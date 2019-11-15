#include <iostream>
#include <fstream>
#include <vector>
#include "libAuth/license.h"
#include "common/json/json.h"
#include "common/base64.h"

using namespace std;
using namespace auth;

void enbase64()
{
    License license;
    vector<string> mac_address;
    license.get_mac_address(mac_address);
    if(mac_address.empty()) {
        cerr << "get machine code failed" << endl;
        return;
    }

    Json::Value AuthParams;
    for (auto mac : mac_address)
        AuthParams["mac_address"].append(mac);

    Json::Value config;
    config["AuthParams"] = AuthParams;

    Json::StyledWriter writer;
    string macString = writer.write(config);
    string base64String = dev::base64_encode(macString.data(), macString.length());
    string encrypt = license.encrypt_public_key(base64String.c_str());

    ofstream machineInfo("machineInfo.txt", ios::out | ios::binary);
    machineInfo << encrypt << endl;
    machineInfo.close();
}

void debase64()
{
    ifstream machinfInfo("machineInfo.txt");

    string macString;
    machinfInfo >> macString;

    cout << macString << endl;

    cout << dev::base64_decode((char *)macString.data(), macString.length()) << endl;
}

int main() {
    enbase64();

//    debase64();

    return 0;
}