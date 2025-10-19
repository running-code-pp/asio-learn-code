/**
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-12 15:20:36
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-12 15:35:26
 * @FilePath: \asio-learn-code\include\asio_learn\tcp_client.hpp
 * @Description:
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */
#ifndef ASIO_LEARN_TCP_CLIENT_HPP
#define ASIO_LEARN_TCP_CLIENT_HPP

#include <array>
#include <memory>
#include <vector>

#include "asio_learn/public.hpp"
#include "common/Log.hpp"
namespace asio_learn
{
  // tcp 会话
  class Session : public std::enable_shared_from_this<Session>
  {
   public:
    explicit Session(tcp_socket socket) : _socket(std::move(socket))
    {
    }
    Session() = delete;
    virtual ~Session()
    {
      _socket.close();
    }
    void start()
    {
      // 开启异步读取
      do_read();
    }

   protected:
    void do_read()
    {
      auto self = shared_from_this();
      _socket.async_read_some(
          asio::buffer(_dynamic_buf),
          [this, self](const asio::error_code& ec, std::size_t bytes_transferred)
          {
            asio::transfer_at_least(1);
            // 处理读取到的数据
            if (!ec)
            {
              LOG_DEBUG("async_read_some success bytes:{}", bytes_transferred);
              do_write(bytes_transferred);
            }
            else if (ec == asio::error::eof)
            {  // 断连
              LOG_INFO("client {} disconnected", _socket.remote_endpoint().address().to_string());
            }
            else
            {
              LOG_ERR("session {} async_read_some err:{}", _socket.remote_endpoint().address().to_string(), ec.message());
            }
          });
    }

    void do_write(size_t len)
    {
      auto self = shared_from_this();
      asio::async_write(
          _socket,
          asio::dynamic_buffer(_dynamic_buf),
          [this, self](const asio::error_code& ec, std::size_t bytes_transferred)
          {
            asio::transfer_at_least(1);  // 至少传输一个字节
            if (!ec)
            {
              LOG_DEBUG("async_write success bytes:{}", bytes_transferred);
              // 写成功
              do_read();
            }
            else
            {
              LOG_ERR("async_write err: {}", ec.message());
            }
          });
    }

   private:
    std::vector<char> _dynamic_buf;  // 动态缓冲区
    tcp_socket _socket;
  };
}  // namespace asio_learn

#endif  // ASIO_LEARN_TCP_CLIENT_HPP