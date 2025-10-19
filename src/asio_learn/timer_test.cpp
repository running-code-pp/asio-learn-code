/**
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-12 23:18:27
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-12 23:19:05
 * @FilePath: \asio-learn-code\src\asio_learn\timer_test.cpp
 * @Description: asio 中几种定时器的使用
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */
#include "asio_learn/public.hpp"
#include "common/Log.hpp"
using namespace asio_learn;

// 定时器/超时间隔/允许超时次数
void trigger_timer_times(std::shared_ptr<steady_timer> timer, uint32 expiredMs, uint32 count)
{
  // 创建一个计数器用于记录输出次数，和超时异步任务共享计数器所有权
  auto counterPtr = std::make_shared<uint32>(0);
  timer->expires_after(std::chrono::milliseconds(expiredMs));
  timer->async_wait(
      [expiredMs, timer, count, counterPtr](const asio::error_code& ec)
      {
        if (!ec)
        {
          LOG_INFO("much triggered timer was triggered once", *counterPtr + 1);
          ++(*counterPtr);
          if (*counterPtr >= count)
          {
            LOG_INFO("much triggered timer has triggered the last time, stop now", count);
          }
          else
          {
            // 继续设置超时时间
            timer->expires_after(std::chrono::milliseconds(expiredMs));
            trigger_timer_times(timer, expiredMs, count - *counterPtr);
          }
          return;
        }
        else if (ec == asio::error::operation_aborted)
        {
          LOG_INFO("timer was cancelled");
          return;
        }
        else
        {
          LOG_ERR("timer set expired_after error:{}", ec.message());
          return;
        }
      });
}

int main()
{
  // 初始化日志配置并保持其生命周期
  common::LoggerConfig config;
  common::loadLogConfig(config, "log.yaml");
  auto logger = common::create_logger();
  logger->Init(config);
  io_context ioc;
  // 创建稳定时钟计时器,两秒钟之后出发超时
  auto steadyTimer = std::make_shared<steady_timer>(ioc, std::chrono::seconds(2));
  LOG_INFO("steady_timer started, will expire in 2 seconds");

  steadyTimer->async_wait(
      [steadyTimer](const asio::error_code& ec)
      {
        if (!ec)
        {
          LOG_INFO("steady_timer expired");
        }
        else if (ec == asio::error::operation_aborted)
        {
          LOG_INFO("steady_timer was cancelled");
        }
        else
        {
          LOG_ERR("steady_timer error: {}", ec.message());
        }
      });

  // 创建系统时钟定时器
  auto systemTimer = std::make_shared<system_timer>(ioc, std::chrono::system_clock::now() + std::chrono::seconds(3));
  LOG_INFO("system_timer started, will expire in 3 seconds");
  systemTimer->async_wait(
      [systemTimer](const asio::error_code& ec)
      {
        if (!ec)
        {
          LOG_INFO("system_timer expired");
        }
        else if (ec == asio::error::operation_aborted)
        {
          LOG_INFO("system_timer was cancelled");
        }
        else
        {
          LOG_ERR("system_timer error: {}", ec.message());
        }
      });

  auto steadyTimer1 = std::make_shared<steady_timer>(ioc);
  LOG_INFO("steady_timer1 started, will expire every 1 second for 10 times");
  trigger_timer_times(steadyTimer1, 1000, 10);
  ioc.run();
  logger->ShutDown();
  return 0;
}