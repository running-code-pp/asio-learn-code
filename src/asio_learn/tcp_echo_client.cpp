/**
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-11 23:41:07
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-12 15:07:47
 * @FilePath: \asio-learn-code\src\asio_learn\tcp_echo_client.cpp
 * @Description: tcp echo client
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */

#include "common/Log.hpp"
#include <asio/error_code.hpp>
#include "asio_learn/public.hpp"
#include "asio_learn/tcp_client.hpp"
#include "yaml-cpp/yaml.h"
using namespace asio_learn;
int main()
{
  // 加载日志配置
  common::LoggerConfig config;
  common::loadLogConfig(config, "log.yaml");
  common::create_logger()->Init(config);

  asio::io_context ioc;  // 核心事件循环
  tcp_endpoint server_point(asio::ip::make_address_v4("127.0.0.1"), 9527);
  tcp_socket client_socket(ioc);
  // 连接到服务器
  std::error_code ec;
  client_socket.connect(server_point, ec);
  if (ec)
  {
    LOG_ERR("connect to server failed: {}", ec.message());
    return -1;
  }
  else
  {
    LOG_INFO("connect to server {} success ", server_point.address().to_string());
  }
  auto client_session = std::make_shared<Session>(std::move(client_socket));
  client_session->start();                            // 启动会话
  // 彻底跑完剩余事件
  ioc.run();
  return 0;
}
