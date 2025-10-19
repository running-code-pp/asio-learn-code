/**
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-13 13:38:12
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-13 13:38:16
 * @FilePath: \asio-learn-code\src\asio_learn\tcp_server_onethonecli.cpp
 * @Description: 每个客户端连接分配一个线程的tcp_server示例
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */

#include <memory>
#include <thread>
#include <vector>
#include <sstream>

#include "asio_learn/public.hpp"
#include "common/Log.hpp"

using namespace asio_learn;

void process_data(const std::vector<char>& data, size_t read_bytes)
{
  LOG_INFO("Received {} bytes: {}", read_bytes, std::string(data.data(), read_bytes));
}



void handle_connection(tcp_socket socket)
{
  // 单独开一个线程处理该连接
  std::thread(
      [socket = std::move(socket)]() mutable
      {
        // 处理 socket 的逻辑
        try
        {
          LOG_INFO("thread {} start serving client {}",threadId_to_str(),socket.remote_endpoint().address().to_string());
          for(;;){
            std::vector<char> data(1024);
            // 实际读取数据,如果没有传入错误码会抛出异常
            asio::error_code ec;
            size_t bytes_read = socket.read_some(asio::buffer(data),ec);
            if (ec == asio::error::eof) {
              LOG_INFO("Connection closed by peer: {}", socket.remote_endpoint().address().to_string());
              socket.close();
              break; // 连接关闭
            }
            // 处理接收数据
            process_data(data, bytes_read);
            // 回写接收到的数据（echo server）
            asio::write(socket, asio::buffer(data, bytes_read));
          }
        }
        catch (const std::exception& e)
        {
          LOG_ERR("Exception in connection handler: {}", e.what());
          socket.close();
        }
      })
      .detach();
}

void start_accept(std::shared_ptr<tcp_acceptor> acceptor, std::shared_ptr<io_context> ioc)
{
  acceptor->async_accept(
      [acceptor, ioc](const asio::error_code& ec, tcp_socket socket)
      {
        if (!ec)
        {
          LOG_INFO("New connection accepted from {}", socket.remote_endpoint().address().to_string());
          handle_connection(std::move(socket));
        }
        else
        {
          LOG_ERR("Accept error: {}", ec.message());
        }
        start_accept(acceptor, ioc);
      });
}

int main()
{
  // 初始化日志配置并保持其生命周期
  common::LoggerConfig config;
  common::loadLogConfig(config, "log.yaml");
  auto logger = common::create_logger();
  logger->Init(config);
  auto ioc = std::make_shared<io_context>();
  auto server_point = asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 9996);
  std::shared_ptr<tcp_acceptor> acceptor = std::make_shared<tcp_acceptor>(*ioc,server_point);
  start_accept(acceptor, ioc);
  ioc->run();
  return 0;
}
