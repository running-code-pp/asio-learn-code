/**
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-13 14:01:10
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-13 14:12:05
 * @FilePath: \asio-learn-code\src\asio_learn\tcp_server_threadpool.cpp
 * @Description: tcp_server 线程池版本 示例
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */

#include <memory>
#include <sstream>
#include <thread>
#include <vector>

#include "asio_learn/public.hpp"
#include "common/Log.hpp"
using namespace asio_learn;



struct tcp_session : public std::enable_shared_from_this<tcp_session>
{
  tcp_session(tcp_socket socket) : _buffer(1024), _socket(std::move(socket))
  {
  }
  void start()
  {
    do_read();  // 启动读取
  }

 private:
  void do_read()
  {
    auto self = shared_from_this();

    // 方式1: 使用 read_some (读取可用的数据，数量不定)
    _socket.async_read_some(
        asio::buffer(_buffer),
        [self, this](const asio::error_code& ec, std::size_t read_bytes)
        {
          if (!ec)
          {
            LOG_INFO("thread_id {} ---- read_some: Read {} bytes from client", threadId_to_str(), read_bytes);
            do_write(read_bytes);
          }
          else if (ec == asio::error::eof)
          {
            LOG_INFO("thread_id {} ---- Connection closed by peer.", threadId_to_str());
          }
          else
          {
            LOG_ERR("Read error: {}", ec.message());
          }
        });

    /*
    // 方式2: transfer_at_least(1) - 至少读1字节就完成，但可能读更多
    asio::async_read(_socket, asio::dynamic_buffer(_buffer),
                     asio::transfer_at_least(1),
                     [self, this](const asio::error_code& ec, std::size_t read_bytes)
                     {
                       if (!ec)
                       {
                         LOG_INFO("transfer_at_least(1): Read {} bytes", read_bytes);
                         // 可能读取 1-N 字节，取决于当时可用数据
                         do_write(read_bytes);
                       }
                     });

    // 方式3: transfer_exactly(100) - 必须读取恰好100字节
    asio::async_read(_socket, asio::dynamic_buffer(_buffer),
                     asio::transfer_exactly(100),
                     [self, this](const asio::error_code& ec, std::size_t read_bytes)
                     {
                       if (!ec)
                       {
                         LOG_INFO("transfer_exactly(100): Read {} bytes", read_bytes);
                         // read_bytes 总是 100，或者出错
                         do_write(read_bytes);
                       }
                     });

    // 方式4: 无条件 - 尝试填满整个缓冲区
    asio::async_read(_socket, asio::dynamic_buffer(_buffer),
                     [self, this](const asio::error_code& ec, std::size_t read_bytes)
                     {
                       if (!ec)
                       {
                         LOG_INFO("no condition: Read {} bytes (buffer full)", read_bytes);
                         // 缓冲区被填满，或者连接关闭
                         do_write(read_bytes);
                       }
                     });
    */
  }

  void do_write(std::size_t bytes_to_write)
  {
    auto self = shared_from_this();
    asio::async_write(
        _socket,
        asio::buffer(_buffer.data(), bytes_to_write),  // 只写实际读取的字节数
        [self, this](const asio::error_code& ec, std::size_t bytes_to_write)
        {
          if (!ec)
          {
            LOG_INFO("thread_id {} ---- write success {} bytes", threadId_to_str(), bytes_to_write);
            // 继续读取数据
            do_read();
          }
          else
          {
            LOG_ERR("Write error: {}", ec.message());
          }
        });
  }

 private:
  std::vector<char> _buffer;
  tcp_socket _socket;
};

void do_accept(std::shared_ptr<asio::thread_pool> th_pool, std::shared_ptr<tcp_acceptor> acceptor)
{
  if (!th_pool || !acceptor)
  {
    LOG_ERR("thread_pool or acceptor is nullptr");
    return;
  }
  acceptor->async_accept(
      [th_pool, acceptor](const asio::error_code& ec, tcp_socket socket)
      {
        if (!ec)
        {
          LOG_INFO("New connection accepted from {}", socket.remote_endpoint().address().to_string());

          // 创建会话并在线程池中处理
          auto session = std::make_shared<tcp_session>(std::move(socket));
          asio::post(*th_pool, [session]() { session->start(); });

          // 继续接受下一连接
          do_accept(th_pool, acceptor);
        }
        else
        {
          LOG_ERR("Accept error: {}", ec.message());
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
  auto ioc = std::make_shared<io_context>();
  auto server_point = asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 9996);
  auto th_pool = std::make_shared<asio::thread_pool>(
      std::thread::hardware_concurrency() == 0 ? 4 : std::thread::hardware_concurrency() << 1);
  auto acceptor = std::make_shared<tcp_acceptor>(th_pool->get_executor(), server_point);
  do_accept(th_pool, acceptor);
  th_pool->join();
  logger->ShutDown();
  return 0;
}