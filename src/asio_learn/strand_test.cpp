/** 
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-12 22:51:21
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-12 23:01:20
 * @FilePath: \asio-learn-code\src\asio_learn\strand_test.cpp
 * @Description: asio::io_context::strand 线性调用示例
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */
#include <csignal>
#include <cstdint>

#include "asio_learn/public.hpp"
#include "common/Log.hpp"
using namespace asio_learn;

// 辅助函数：将 thread_id 转换为字符串以便格式化
std::string thread_id_str()
{
  std::ostringstream oss;
  oss << std::this_thread::get_id();
  return oss.str();
}

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

int global_counter = 0;

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
  // 防止ioc立即退出，增加一个守护任务
  g_iocPtr = iocPtr;
  strand strand_(*iocPtr);
  for (int i = 0; i < 50; ++i)
  {
    asio::post(strand_,[i](){  // 捕获局部变量 i
    ++global_counter;  // 全局变量直接访问，无需捕获
    LOG_INFO("task {} incremented global_counter to {} on thread-{}",i,global_counter,thread_id_str());
    });
  }
  iocPtr->run();
  LOG_INFO("all task finished global_counter:{}",global_counter);
  logger->ShutDown();
  return 0;
}