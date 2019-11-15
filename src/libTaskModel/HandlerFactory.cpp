#include "HandlerFactory.h"
#include "HandlerShip.h"
#include "HandlerSAR.h"
#include "HandlerYolo.h"

using namespace MQ;

std::shared_ptr<HandlerFactory> HandlerFactory::factory = 0;
HandlerFactory* HandlerFactory::instance()
{
    if (!factory)
        factory.reset(new HandlerFactory());

    return factory.get();
}

HandlerFactory::HandlerFactory()
{

}

HandlerFactory::~HandlerFactory()
{
}

std::shared_ptr<HandlerBase> HandlerFactory::create(int handler_id_, const std::string& path_, const dev::ConfigParams *cp_)
{
    //std::cout << "hbk in HandlerFactory::creat() ... and handler_id is " << handler_id_ << " \n";
    std::unique_lock<std::mutex> lock(mtx);

    switch(handler_id_) {
// check option BUILD_SHIP if ON or OFF
#ifdef USE_SHIP
        case SHIP_METHOD_ID:
            //std::cout << "in HandelFactory build with ship ... \n";
            //std::cout << "\n case SHIP_METHOD_ID ... \n\n";
            //return nullptr ;
            return std::make_shared<detect::HandlerShip>(path_, cp_);
#endif

// check option BUILD_SAR if ON or OFF
#ifdef USE_SAR
        case SAR_METHOD_ID:
            //std::cout << "\n case SAR_METHOD_ID ... \n\n";
            return std::make_shared<detect::HandlerSAR>(path_, cp_);
#endif

// check option BUILD_YOLO if ON or OFF
#ifdef USE_YOLO
        case YOLO_METHOD_ID:
            //std::cout << "in HandelFactory build with yolo ... \n";
            //std::cout << "case Yolo_METHOD_ID ... \n";
            return std::make_shared<detect::HandlerYolo>(path_, cp_);
#endif
        default:
            return 0;
    }
}
