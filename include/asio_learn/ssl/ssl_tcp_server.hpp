/**
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-18 23:14:48
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-18 23:17:20
 * @FilePath: \asio-learn-code\include\asio_learn\ssl\ssl_tcp_server.hpp
 * @Description: 增加ssl认证的tcp_server示例
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */
#ifndef ASIO_LEARN_SSL_SSL_TCP_SERVER_HPP_
#define ASIO_LEARN_SSL_SSL_TCP_SERVER_HPP_
#include <array>
#include <asio.hpp>
#include <asio/ssl.hpp>
#include <memory>

#include "common/Log.hpp"

namespace asio_learn::ssl
{

  class SslSession : public std::enable_shared_from_this<SslSession>
  {
   public:
    SslSession(asio::ip::tcp::socket socket, asio::ssl::context& ctx) : _socket(std::move(socket), ctx)
    {
    }

    void start()
    {
      // ssl握手
      auto self = shared_from_this();
      _socket.async_handshake(
          asio::ssl::stream_base::server,
          [this, self](const asio::error_code& ec)
          {
            if (!ec)
            {
              self->do_read();
            }
            else
            {
              // 处理握手错误
              LOG_ERR("SLL handshake failed: {}", ec.message());
            }
          });
    }

   protected:
    void do_read()
    {
      auto self = shared_from_this();
      _socket.async_read_some(
          asio::buffer(_buffer.data(), _buffer.size()),
          [this, self](const asio::error_code& ec, std::size_t bytes_transferred)
          {
            if (!ec)
            {
              LOG_INFO("SSL Read {} bytes from client", bytes_transferred);
              // echo
              do_write(bytes_transferred);
            }
            else if (ec == asio::error::eof)
            {
              LOG_INFO("SSL Connection closed by peer.");
            }
            else
            {
              LOG_ERR("SSL Read error: {}", ec.message());
            }
          });
    }

    void do_write(std::size_t bytes_transferred)
    {
      auto self = shared_from_this();
      asio::async_write(
          _socket,
          asio::buffer(_buffer.data(), bytes_transferred),
          [self](const asio::error_code& ec, std::size_t bytes_write)
          {
            if (!ec)
            {
              LOG_INFO("SSL Write {} bytes to client", bytes_write);
            }
            else
            {
              LOG_ERR("SSL Write error: {}", ec.message());
            }
          });
    }

   private:
    asio::ssl::stream<asio::ip::tcp::socket> _socket;
    std::array<char, 1024> _buffer;
  };


  class SslTcpServer{
    public:
    SslTcpServer(asio::io_context& io_ctx, unsigned short port,asio::ssl::context ssl_ctx):
    _acceptor(io_ctx,asio::ip::tcp::endpoint(asio::ip::tcp::v4(),port)),
    _ssl_ctx(std::move(ssl_ctx)){}


    void run(){
        do_accept();
    }
    
    protected:
    void do_accept(){
        _acceptor.async_accept(
            [this](const asio::error_code& ec, asio::ip::tcp::socket socket)
            {
              if (!ec)
              {
                LOG_INFO("SSL New connection accepted.");
                std::make_shared<SslSession>(std::move(socket),_ssl_ctx)->start();
              }
              else
              {
                LOG_ERR("SSL Accept error: {}", ec.message());
              }
              // 接受下一个连接
              do_accept();
            }
        );
    }

    private:
      asio::ssl::context _ssl_ctx;
      asio::ip::tcp::acceptor _acceptor;
  };
}  // namespace asio_learn::ssl

#endif  // ASIO_LEARN_SSL_SSL_TCP_SERVER_HPP_