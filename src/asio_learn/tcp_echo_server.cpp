/** 
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-12 00:31:20
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-12 15:07:37
 * @FilePath: \asio-learn-code\src\asio_learn\tcp_echo_server.cpp
 * @Description: 
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */


#include <asio.hpp>
#include "common/Log.hpp"
#include <vector>
#include <memory>
#include <functional>
#include"asio_learn/tcp_server.hpp"


int main(){
// 加载日志配置
  common::LoggerConfig config;
  common::loadLogConfig(config, "log.yaml");
  common::create_logger()->Init(config);
  asio::io_context ioc;
  auto tcp_server = std::make_shared<asio_learn::TcpServer>(ioc, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 9527));
  tcp_server->start();
  ioc.run();
}