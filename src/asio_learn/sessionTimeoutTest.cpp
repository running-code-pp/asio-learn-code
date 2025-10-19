
#include "asio_learn/sessionTimeoutManager.hpp"
#include "common/Log.hpp"
using namespace asio_learn;

int main()
{
  // 初始化日志配置并保持其生命周期
  common::LoggerConfig config;
  common::loadLogConfig(config, "log.yaml");
  auto logger = common::create_logger();
  logger->Init(config);
  return 0;
}