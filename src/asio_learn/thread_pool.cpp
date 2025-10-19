/** 
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-12 22:07:31
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-12 22:32:45
 * @FilePath: \asio-learn-code\src\asio_learn\thread_pool.cpp
 * @Description: 
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */
/**
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-12 22:07:31
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-12 22:08:29
 * @FilePath: \asio-learn-code\src\asio_learn\thread_pool.cpp
 * @Description:  asio线程池的简单使用
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */
#include <chrono>
#include <thread>
#include <sstream>
#include "asio_learn/public.hpp"
#include "common/Log.hpp"

using namespace asio_learn;

// 辅助函数：将 thread_id 转换为字符串以便格式化
std::string thread_id_str() {
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    return oss.str();
}

int main()
{
  // 初始化日志配置并保持其生命周期
  common::LoggerConfig config;
  common::loadLogConfig(config, "log.yaml");
  auto logger = common::create_logger();
  logger->Init(config);
  auto pool = std::make_shared<asio::thread_pool>(std::thread::hardware_concurrency() << 1);
  // 提交最简单的任务到线程池
  asio::post(
      *pool,
      []()
      {
        LOG_INFO("task running on thread {}", thread_id_str()); 
        std::this_thread::sleep_for(std::chrono::seconds(1));
      });
    // 提交带参数的任务到线程池,注意asio::post 不直接支持带参数的任务，如果需要的话可以采用lambda+捕获列表原地初始化的方式
  asio::post(
      *pool,
      [a = 42, c = 'x'](){ 
        LOG_INFO("task with params: a={}, c={}, thread={}", a, c, thread_id_str()); 
      });
  logger->ShutDown();
  // 等待所有任务结束关闭线程池
  pool->stop();
  return 0;
}