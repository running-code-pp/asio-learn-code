/**
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-12 21:20:59
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-12 21:21:40
 * @FilePath: \asio-learn-code\src\asio_learn\resolver.cpp
 * @Description: 解析器的简单应用
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */

// 修改本地host文件，添加	127.0.0.1       test.pp.com
#include <string>
#include <memory>

#include "asio_learn/public.hpp"
#include "common/Log.hpp"

using namespace asio_learn;
void connectToServer(io_context& ioc, const std::string& host, const std::string& service)
{
  LOG_INFO("Starting DNS resolution for {}:{}", host, service);
  
  // 使用shared_ptr管理resolver，防止在异步操作完成前被销毁
  auto resolver = std::make_shared<asio::ip::tcp::resolver>(ioc);
  resolver->async_resolve(
      host,
      service,
      [resolver, &ioc](const asio::error_code& ec, asio::ip::tcp::resolver::results_type results)
      {
        if (!ec)
        {
          LOG_INFO("resolve success, found {} endpoints", results.size());
          for (const auto& entry : results)
          {
            LOG_INFO("endpoint: {}:{}", entry.endpoint().address().to_string(), entry.endpoint().port());
            
            // 使用shared_ptr管理socket，防止在异步操作完成前被销毁
            auto socket = std::make_shared<tcp_socket>(ioc);
            socket->async_connect(
                entry.endpoint(),
                [socket](const asio::error_code& ec)
                {
                  if (!ec)
                  {
                    LOG_INFO("connect to server success");
                    // 断开连接
                    socket->close();
                    LOG_INFO("shutdown socket success");
                  }
                  else
                  {
                    LOG_ERR("connect to server failed: {}", ec.message());
                  }
                });
            
            // 只连接第一个可用的endpoint
            break;
          }
        }
        else
        {
          LOG_ERR("resolve failed: {}", ec.message());
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
  
  LOG_INFO("Starting resolver test...");
  
  // 方式1: 直接使用端口号
  connectToServer(ioc,"test.pp.com","9527");
  
  // 方式2: 使用服务名称（需要系统支持）
  // connectToServer(ioc,"test.pp.com","http");   // 默认80端口
  // connectToServer(ioc,"test.pp.com","https");  // 默认443端口
  // connectToServer(ioc,"test.pp.com","ftp");    // 默认21端口
  
  // 方式3: 其他常用端口示例
  // connectToServer(ioc,"test.pp.com","22");     // SSH
  // connectToServer(ioc,"test.pp.com","3306");   // MySQL
  // connectToServer(ioc,"test.pp.com","5432");   // PostgreSQL
  // connectToServer(ioc,"test.pp.com","6379");   // Redis
  
  LOG_INFO("Starting event loop...");
  ioc.run();
  
  LOG_INFO("Event loop completed, program exiting...");
  
  // 确保日志完全写入 - 使用您自定义的日志系统的 ShutDown 方法
  logger->ShutDown();
  
  return 0;
}