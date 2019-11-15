#ifndef _HANDLER_FACTORY_
#define _HANDLER_FACTORY_
#include <map>
#include <memory>
#include "Dispatcher.h"
#include "common/configParams.h"

namespace MQ {
    class HandlerBase;

    class HandlerFactory
    {
    public:
        static HandlerFactory* instance();
        ~HandlerFactory();
        std::shared_ptr<HandlerBase> create(int handler_id_, const std::string &path, const dev::ConfigParams *cp_);

    protected:
        HandlerFactory();

    private:
        static std::shared_ptr<HandlerFactory> factory;
        std::mutex mtx;
    };
}

#endif