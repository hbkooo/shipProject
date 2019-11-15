#include <iostream>
#include "common/json/json.h"
#include "libAuth/license.h"
#include <boost/filesystem.hpp>
#include "common/base64.h"

using namespace auth;

bool generateDefaultConfig(std::string const &configPath)
{
    Json::Value rsaConfig;
    rsaConfig["auth_file"] = "machineInfo.txt";
    rsaConfig["license_pem"] = "../license/rsa_private_key.pem";
    rsaConfig["monitor_pem"] = "../license/monitor_private.pem";

    Json::Value config;
    config["rsaParams"] = rsaConfig;

    Json::StyledWriter writer;
    std::string document = writer.write(config);

    std::ofstream os(configPath);
    if (!os.is_open())
        return false;
    os << document;
    os.close();
    return true;
}

std::string machineCodedebase64(std::string filename, const char *sk_filename) {
}

int main(int argc, char *argv[])
{
    std::string configPath("rsa.json");
    if (argc == 2) {
        configPath = argv[1];
        std::cout << "Config file   : " << configPath <<std::endl;
    } else if (argc == 1) {
        std::cout << "Config file   : rsa.json" <<std::endl;
    } else {
        return -1;
    }

    std::string auth_file;
    std::string license_pem;
    std::string monitor_pem;
    if (!boost::filesystem::exists(configPath)) {
        if (!generateDefaultConfig(configPath))
            std::cout << "Generate default config file failed." << std::endl;

        auth_file = "machineInfo.txt";
        license_pem = "rsa_private_key.pem";
        monitor_pem = "monitor_private.pem";
        std::cout << "Using default config parameters." << std::endl;
    } else {
        std::ifstream in(configPath);
        if (!in.is_open())
            return -1;

        Json::Reader reader;
        Json::Value config;

        if (!reader.parse(in, config, false) || !config.isObject()) {
            std::cout<< "rsa.json file syntax error.";
            throw std::invalid_argument("rsa.json file syntax error.");
        }
        in.close();

        if (config.isMember("rsaParams") && config["rsaParams"].isObject()) {
            if (config["rsaParams"].isMember("auth_file"))
                auth_file = config["rsaParams"]["auth_file"].asString();
            else
                std::cout<< "Missing auth_file."<<std::endl;

            if (config["rsaParams"].isMember("license_pem"))
                license_pem = config["rsaParams"]["license_pem"].asString();
            else
                std::cout<< "Missing private_key."<<std::endl;

            if (config["rsaParams"].isMember("monitor_pem"))
                monitor_pem = config["rsaParams"]["monitor_pem"].asString();
            else
                std::cout<< "Missing maxRequests."<<std::endl;
        } else {
            std::cout<< "Missing rsaParams."<<std::endl;
        }
    }

    License license;

    std::ifstream machinfInfo(auth_file);
    if (!machinfInfo)
        std::cerr << "open " << auth_file << " failed" << std::endl;

    std::string licenseStr = license.read_file(auth_file.c_str());
    std::string base64String = license.decrypt_private_pem(licenseStr, license_pem.c_str());

    std::string text = dev::base64_decode((char*)base64String.data(), base64String.length());

    Json::Reader reader;
    Json::Value config;
    Json::Value mac_address;
    if (text.empty() || !reader.parse(text, config, false) || !config.isObject())
    {
        std::cout << "machineinfo parse error!!" << std::endl;
        return false;
    }
    if (config["AuthParams"].isMember("mac_address") && config["AuthParams"]["mac_address"].isArray() && config["AuthParams"]["mac_address"].size() > 0)
        mac_address = config["AuthParams"]["mac_address"];

    Json::Value AuthConfig;
    AuthConfig["mac_address"] = mac_address;

    std::string monitor_key = license.read_file(monitor_pem.c_str());
    AuthConfig["monitor_key"] = dev::base64_encode(monitor_key.c_str(), monitor_key.size());
    config["AuthParams"] = AuthConfig;

    Json::StyledWriter writer;
    std::string macString = writer.write(config);

    //std::cout << text << std::endl;
    std::cout << macString << std::endl;
    std::string encrypt = license.encrypt_private_pem(macString.c_str(), license_pem.c_str());
    if(encrypt.empty())
    {
        printf("rsa_sign error!!\n");
        exit(-1);
    }
    remove("license-key.lic");

    std::ofstream licenseFile("license-key.lic", std::ios::out | std::ios::binary);
    licenseFile << encrypt;
    licenseFile.close();

    printf("The license-key.lic file was generated successfully!\n");

    return 0;
}
