/** 
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-12 22:37:17
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-12 22:52:36
 * @FilePath: \asio-learn-code\src\asio_learn\io_context_concurrency.cpp
 * @Description: 
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */
/**
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-12 22:37:17
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-12 22:42:20
 * @FilePath: \asio-learn-code\src\asio_learn\io_context_concurrency.cpp
 * @Description: 多个线程调用io_context::run
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */

#include <chrono>
#include <csignal>
#include <thread>
#include <vector>

#include "asio_learn/public.hpp"
#include "common/Log.hpp"
using namespace asio_learn;

std::weak_ptr<asio::io_context> g_iocPtr;
void signal_handler(int sig)
{
  if (sig == SIGINT)
  {
    // CTLR+C结束
    if (auto iocPtr = g_iocPtr.lock())
    {
      iocPtr->stop();
    }
  }
}

int main()
{
  // 注册信号
  std::signal(SIGINT, signal_handler);
  // 初始化日志配置并保持其生命周期
  common::LoggerConfig config;
  common::loadLogConfig(config, "log.yaml");
  auto logger = common::create_logger();
  logger->Init(config);
  auto iocPtr = std::make_shared<asio::io_context>();
  g_iocPtr = iocPtr;
  // 防止ioc立即退出，增加一个守护任务
  asio::io_context::work work_guard(*iocPtr);

  std::vector<std::thread> threads;
  for (int i = 0; i < (std::thread::hardware_concurrency() << 1); ++i)
  {
    std::thread t([iocPtr]() { iocPtr->run(); });
    threads.emplace_back(std::move(t));
  }
  for (auto& t : threads)
  {
    t.join();
  }
  logger->ShutDown();
  return 0;
}