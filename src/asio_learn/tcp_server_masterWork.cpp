/** 
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-13 14:37:43
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-13 15:25:02
 * @FilePath: \asio-learn-code\src\asio_learn\tcp_server_masterWork.cpp
 * @Description: 
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */

/**
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-13 14:01:10
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-13 14:12:05
 * @FilePath: \asio-learn-code\src\asio_learn\tcp_server_masterWork.cpp
 * @Description: tcp_server 主从架构 示例
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */


 /**
  主线程:负责接受接连和派发会话
  工作线程：维护会话
  */
#include <memory>
#include <sstream>
#include <thread>
#include <vector>

#include "asio_learn/public.hpp"
#include "common/Log.hpp"
#include "asio_learn/tcp_server_master_worker.hpp"
using namespace asio_learn;


int main()
{
  // 初始化日志配置并保持其生命周期
  common::LoggerConfig config;
  common::loadLogConfig(config, "log.yaml");
  auto logger = common::create_logger();
  logger->Init(config);
  MasterWorkerTcpServer server(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 9986), std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() * 2 : 4);
  server.run();
  logger->ShutDown();
  return 0;
}