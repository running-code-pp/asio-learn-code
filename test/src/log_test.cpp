#include"common/log.hpp"

int main(){
    common::LoggerConfig config;
    common::loadLogConfig(config,"log.yaml");
    auto logger = common::create_logger();
    logger->Init(config);
    LOG_TRACE("This is a {} message with num{}","trace",1);
    LOG_DEBUG("This is a {} message with num{}","debug",2);
    LOG_INFO("This is a {} message with num{}","info",3);
    LOG_WARN("This is a {} message with num{}","warn",4);
    LOG_ERR("This is a {} message with num{}","err",5);
    LOG_FATAL("This is a {} message with num{}","fatal",6);
    logger->ShutDown();
    return 0;
}