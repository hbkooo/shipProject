#include "configParams.h"
#include "json/json.h"
#include "easylog++.h"
#include "version.h"
#include <exception>
#include <thread>
#include <boost/filesystem.hpp>
#include <map>

INITIALIZE_EASYLOGGINGPP

using namespace dev;
using namespace std;

#define CONFIG_ERROR "Config file error. "
namespace dev
{
static map<string, unsigned int> s_mlogIndex;
}

void rolloutHandler(const char *filename, size_t)
{
    stringstream ss;

    map<string, unsigned int>::iterator iter = dev::s_mlogIndex.find(filename);
    if (iter != dev::s_mlogIndex.end())
    {
        ss << filename << "." << iter->second++;
        dev::s_mlogIndex[filename] = iter->second++;
    }
    else
    {
        ss << filename << "." << 0;
        dev::s_mlogIndex[filename] = 0;
    }
    boost::filesystem::rename(filename, ss.str().c_str());
}

ConfigParams::ConfigParams(string const &configPath)
{
    cpuCores = thread::hardware_concurrency();
    if (!boost::filesystem::exists(configPath))
    {
        if (!generateDefaultConfig(configPath))
            cout << "Generate default config file failed." << endl;
        cout << "Using default config parameters." << endl;
    }
    else
        loadConfig(configPath);
    initEasylogging(logConfigParams);
    versionMajor = VERIFIER_VERSION_MAJOR;
    versionMinor = VERIFIER_VERSION_MINOR;
    versionPatch = VERIFIER_VERSION_PATCH;

#ifdef NDEBUG
    buildType = " Release";
#else
    buildType = "  Debug ";
#endif
}

void ConfigParams::loadConfig(string const &configPath)
{
    ifstream in(configPath);
    if (!in.is_open())
        return;
    Json::Reader reader;
    Json::Value config;

    if (!reader.parse(in, config, false) || !config.isObject())
    {
        LOG(ERROR) << "Config file syntax error.";
        throw invalid_argument("Config file syntax error.");
    }
    in.close();

    // config http params
    if (config.isMember("httpParams") && config["httpParams"].isObject())
    {
        if (config["httpParams"].isMember("port") && config["httpParams"]["port"].isUInt())
            port = config["httpParams"]["port"].asUInt();
        else
            LOG(ERROR) << CONFIG_ERROR << "Missing port.";

        if (config["httpParams"].isMember("apiKey"))
            apiKey = config["httpParams"]["apiKey"].asString();
        else
            LOG(ERROR) << CONFIG_ERROR << "Missing apiKey.";

        if (config["httpParams"].isMember("listenIP") && !config["httpParams"]["listenIP"].asString().empty())
            ip = config["httpParams"]["listenIP"].asString();
        else
            LOG(ERROR) << CONFIG_ERROR << "Missing listenIP.";

        if (config["httpParams"].isMember("httpThreads") && config["httpParams"]["httpThreads"].isUInt())
            threads = config["httpParams"]["httpThreads"].asInt();
        else
            LOG(ERROR) << CONFIG_ERROR << "Missing httpThreads.";
    }
    else
        LOG(ERROR) << CONFIG_ERROR << "Missing httpParams.";

    // config bucket params
    if (config.isMember("bucketParams") && config["bucketParams"].isObject())
    {
        if (config["bucketParams"].isMember("maxRequests") && config["bucketParams"]["maxRequests"].isUInt())
            maxRequests = config["bucketParams"]["maxRequests"].asInt();
        else
            LOG(ERROR) << CONFIG_ERROR << "Missing maxRequests.";
    }
    else
        LOG(ERROR) << CONFIG_ERROR << "Missing bucketParams.";

    // config caffe params
    if (config.isMember("caffeParams") && config["caffeParams"].isObject())
    {
        if (config["caffeParams"].isMember("GPUDevices") && config["caffeParams"]["GPUDevices"].isUInt())
            GPUDevices = config["caffeParams"]["GPUDevices"].asInt();
        if (config["caffeParams"].isMember("modelPath") && !config["caffeParams"]["modelPath"].asString().empty())
            modelPath = config["caffeParams"]["modelPath"].asString();
        else
            LOG(ERROR) << CONFIG_ERROR << "Missing modelPath.";
        if (config["caffeParams"].isMember("threadsShip") && config["caffeParams"]["threadsShip"].isUInt())
        {
            batchSize = config["caffeParams"]["batchSize"].asInt();

            detectShipPoolSize = config["caffeParams"]["threadsShip"].asInt();
            detectShipPoolSize = detectShipPoolSize > cpuCores ? cpuCores : detectShipPoolSize;
            detectSarPoolSize = config["caffeParams"]["threadsSar"].asInt();
            detectSarPoolSize = detectSarPoolSize > cpuCores ? cpuCores : detectSarPoolSize;
            detectYoloPoolSize = config["caffeParams"]["threadsYolo"].asInt();
            detectYoloPoolSize = detectYoloPoolSize > cpuCores ? cpuCores : detectYoloPoolSize;

        }
        else
            LOG(ERROR) << CONFIG_ERROR << "Missing caffeThreads.";
    }
    else
        LOG(ERROR) << CONFIG_ERROR << "Missing caffeParams.";

    // config log
    if (config.isMember("logConfigParams") && config["logConfigParams"].isObject())
    {
        if (config["logConfigParams"].isMember("traceEnabled") && config["logConfigParams"]["traceEnabled"].isBool())
            logConfigParams.traceEnabled = config["logConfigParams"]["traceEnabled"].asBool();
        if (config["logConfigParams"].isMember("debugEnabled") && config["logConfigParams"]["debugEnabled"].isBool())
            logConfigParams.debugEnabled = config["logConfigParams"]["debugEnabled"].asBool();
        if (config["logConfigParams"].isMember("fatalEnabled") && config["logConfigParams"]["fatalEnabled"].isBool())
            logConfigParams.fatalEnabled = config["logConfigParams"]["fatalEnabled"].asBool();
        if (config["logConfigParams"].isMember("errorEnabled") && config["logConfigParams"]["errorEnabled"].isBool())
            logConfigParams.errorEnabled = config["logConfigParams"]["errorEnabled"].asBool();
        if (config["logConfigParams"].isMember("warningEnabled") && config["logConfigParams"]["warningEnabled"].isBool())
            logConfigParams.warningEnabled = config["logConfigParams"]["warningEnabled"].asBool();
        if (config["logConfigParams"].isMember("infoEnabled") && config["logConfigParams"]["infoEnabled"].isBool())
            logConfigParams.infoEnabled = config["logConfigParams"]["infoEnabled"].asBool();
        if (config["logConfigParams"].isMember("verboseEnabled") && config["logConfigParams"]["verboseEnabled"].isBool())
            logConfigParams.verboseEnabled = config["logConfigParams"]["verboseEnabled"].asBool();
    }
    else
        LOG(ERROR) << CONFIG_ERROR << "Missing logConfigParams.";

    // config zmq params
    if (config.isMember("zmqParams") && config["zmqParams"].isObject())
    {
        if (config["zmqParams"].isMember("zmqaddr") && !config["zmqParams"]["zmqaddr"].asString().empty())
            zmqAddr = config["zmqParams"]["zmqaddr"].asString();
        else
            LOG(ERROR) << CONFIG_ERROR << "Missing zmqaddr.";

        if (config["zmqParams"].isMember("zmqaddr2") && !config["zmqParams"]["zmqaddr2"].asString().empty())
            zmqAddr2 = config["zmqParams"]["zmqaddr2"].asString();
        else
            LOG(ERROR) << CONFIG_ERROR << "Missing zmqaddr2.";

        if (config["zmqParams"].isMember("zmqThreads") && config["zmqParams"]["zmqThreads"].isUInt())
            zmqthreads = config["zmqParams"]["zmqThreads"].asInt();
        else
            LOG(ERROR) << CONFIG_ERROR << "Missing zmqThreads.";
    }
    else
        LOG(ERROR) << CONFIG_ERROR << "Missing zmqParams.";

    // config auth params
    if (config.isMember("AuthParams") && config["AuthParams"].isObject())
    {
        if (config["AuthParams"].isMember("license_path") && !config["AuthParams"]["license_path"].asString().empty())
            licensePath = config["AuthParams"]["license_path"].asString();
        else
            LOG(ERROR) << CONFIG_ERROR << "Missing  license_path.";
        
        if (config["AuthParams"].isMember("license_file") && !config["AuthParams"]["license_file"].asString().empty())
            licenseFile = licensePath + config["AuthParams"]["license_file"].asString();
        else
            LOG(ERROR) << CONFIG_ERROR << "Missing license_file.";
    }
    else
        LOG(ERROR) << CONFIG_ERROR << "Missing AuthParams.";

}

bool ConfigParams::generateDefaultConfig(string const &configPath) const
{
    Json::Value httpConfig;
    httpConfig["apiKey"] = apiKey;
    httpConfig["listenIP"] = ip;
    httpConfig["port"] = port;
    httpConfig["httpThreads"] = threads;

    Json::Value bucketConfig;
    bucketConfig["maxRequests"] = maxRequests;

    Json::Value zmqConfig;
    zmqConfig["zmqaddr"] = zmqAddr;
    zmqConfig["zmqaddr2"] = zmqAddr2;
    zmqConfig["zmqThreads"] = zmqthreads;

    Json::Value AuthConfig;
    AuthConfig["license_path"] = licensePath;
    AuthConfig["license_file"] = licenseFile;

    Json::Value caffeConfig;
    caffeConfig["modelPath"] = modelPath;
    caffeConfig["GPUDevices"] = GPUDevices;
    caffeConfig["batchSize"] = batchSize;

    caffeConfig["threadsShip"] = detectShipPoolSize;
    caffeConfig["threadsSar"] = detectSarPoolSize;
    caffeConfig["threadsYolo"] = detectYoloPoolSize;

    Json::Value logParamsConfig;
    logParamsConfig["traceEnabled"] = logConfigParams.traceEnabled;
    logParamsConfig["debugEnabled"] = logConfigParams.debugEnabled;
    logParamsConfig["fatalEnabled"] = logConfigParams.fatalEnabled;
    logParamsConfig["errorEnabled"] = logConfigParams.errorEnabled;
    logParamsConfig["warningEnabled"] = logConfigParams.warningEnabled;
    logParamsConfig["infoEnabled"] = logConfigParams.infoEnabled;
    logParamsConfig["verboseEnabled"] = logConfigParams.verboseEnabled;

    Json::Value config;
    config["httpParams"] = httpConfig;
    config["caffeParams"] = caffeConfig;
    config["logConfigParams"] = logParamsConfig;
    config["zmqParams"] = zmqConfig;
    config["bucketParams"] = bucketConfig;
    config["AuthParams"] = AuthConfig;


    Json::StyledWriter writer;
    string document = writer.write(config);

    ofstream os(configPath);
    if (!os.is_open())
        return false;
    os << document;
    os.close();
    return true;
}

//日志配置文件放到log目录
void ConfigParams::initEasylogging(const LogConfigParams &logConfig) const
{
    el::Loggers::addFlag(el::LoggingFlag::MultiLoggerSupport); // Enables support for multiple loggers
    el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);

    el::Configurations conf;
    conf.set(el::Level::Global, el::ConfigurationType::Enabled, to_string(true));
    conf.set(el::Level::Trace, el::ConfigurationType::Enabled, to_string(logConfig.traceEnabled));
    conf.set(el::Level::Debug, el::ConfigurationType::Enabled, to_string(logConfig.debugEnabled));
    conf.set(el::Level::Fatal, el::ConfigurationType::Enabled, to_string(logConfig.fatalEnabled));
    conf.set(el::Level::Error, el::ConfigurationType::Enabled, to_string(logConfig.errorEnabled));
    conf.set(el::Level::Warning, el::ConfigurationType::Enabled, to_string(logConfig.warningEnabled));
    conf.set(el::Level::Info, el::ConfigurationType::Enabled, to_string(logConfig.infoEnabled));
    conf.set(el::Level::Verbose, el::ConfigurationType::Enabled, to_string(logConfig.verboseEnabled));

    conf.set(el::Level::Global, el::ConfigurationType::ToFile, to_string(true));
    conf.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, to_string(false));
    conf.set(el::Level::Global, el::ConfigurationType::Format, "%level|%datetime{%Y-%M-%d %H:%m:%s}|%msg");
    conf.set(el::Level::Global, el::ConfigurationType::MillisecondsWidth, to_string(3));
    conf.set(el::Level::Global, el::ConfigurationType::PerformanceTracking, to_string(false));
    conf.set(el::Level::Global, el::ConfigurationType::MaxLogFileSize, to_string(209715200)); // 200MB
    conf.set(el::Level::Global, el::ConfigurationType::LogFlushThreshold, to_string(100));    // flush after every 100 logs

    if (logConfig.debugEnabled)
    {
        conf.set(el::Level::Debug, el::ConfigurationType::Filename, "logs/debug_log_%datetime{%Y%M%d%H}.log");
        conf.set(el::Level::Debug, el::ConfigurationType::Format, "%level|%datetime|%file|%func|%line|%msg");
    }
    if (logConfig.errorEnabled)
    {
        conf.set(el::Level::Error, el::ConfigurationType::Filename, "logs/error_log_%datetime{%Y%M%d%H}.log");
        conf.set(el::Level::Error, el::ConfigurationType::Format, "%level|%datetime|%file|%func|%line|%msg");
    }
    if (logConfig.infoEnabled)
    {
        conf.set(el::Level::Info, el::ConfigurationType::Filename, "logs/info_log_%datetime{%Y%M%d%H}.log");
    }
    el::Loggers::reconfigureAllLoggers(conf);

    el::Helpers::installPreRollOutCallback(rolloutHandler);
}

void ConfigParams::printParams() const
{
    stringstream ss;
    ss << "===================================" << endl;
    ss << ">>>>>> Version "
       << versionMajor << "." << versionMinor << "." << versionPatch
       << buildType << " <<<<<<" << endl;
    ss << "===================================" << endl;
    ss << "=========== Http Config ===========" << endl;
    ss << "CPU    Cores    : " << cpuCores << endl;
    ss << "Listen IP       : " << ip << endl;
    ss << "Listen Port     : " << port << endl;
    ss << "Http   Threads  : " << threads << endl;
    ss << "Max    RPS      : " << maxRequests << endl;
    ss << "Api    Key      : " << apiKey << endl;
    ss << "====================================" << endl;
    ss << "=========== Caffe Config ===========" << endl;
    ss << "GPU      Devices: " << GPUDevices << endl;
    ss << "Ship   Threads: " << detectShipPoolSize << endl;
    ss << "SAR    Threads: " << detectSarPoolSize << endl;
    // ss << "Model Path     : " << modelPath << endl;
    ss << "Decrypt  Model  : " << boolalpha << isDecryptModel << noboolalpha << endl;
    ss << "====================================" << endl;
    ss << "========= Yolo Config ==========" << endl;
    ss << "Yolo   Threads  : " << detectYoloPoolSize << endl;
    ss << "====================================" << endl;
    cout << ss.str() << endl;
    LOG(INFO) << endl
              << ss.str();
}
