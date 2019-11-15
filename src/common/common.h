#ifndef DEV_COMMON_H_
#define DEV_COMMON_H_

#include <chrono>

#define PI 3.1415926

// request interface
// INTERFACEID_COUNT : the info queue's size
// the others : the index of each method in queue
enum INTERFACEID
{
    INTERFACEID_MONITOR = 0,
    INTERFACEID_YOLO = 1,  // 5
    INTERFACEID_SHIP = 2,  // 6
    INTERFACEID_SAR = 3,   // 7
    INTERFACEID_COUNT = 4

};

namespace dev
{
    typedef std::vector<std::chrono::system_clock::time_point> TIMEPOINTS;
}

#define CURRENT_SYS_TIMEPOINT std::chrono::system_clock::now()
#define CURRENT_STEADY_TIMEPOINT std::chrono::steady_clock::now()

const int TRACK_DROP_NUM = 5;
const int TRACK_CATCH_NUM = 3;

#endif //DEV_COMMON_H_
