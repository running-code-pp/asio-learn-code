/** 
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-12 16:38:02
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-12 16:46:57
 * @FilePath: \asio-learn-code\include\asio_learn\tcp_server.hpp
 * @Description: tcp echo server
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */
#ifndef ASIO_LEARN_TCP_SERVER_HPP
#define ASIO_LEARN_TCP_SERVER_HPP

#include"asio_learn/public.hpp"
#include"asio_learn/tcp_client.hpp"

namespace asio_learn{
  class TcpServer
  {
   public:
    explicit TcpServer(asio::io_context& ioc, const tcp_endpoint& endpoint) : _acceptor(ioc, endpoint)
    {
    }
    virtual ~TcpServer()
    {
      _acceptor.close();
    };
    void start()
    {
      do_accept();
    }

   private:
    void do_accept()
    {
      _acceptor.async_accept(
          [this](const asio::error_code& ec, tcp_socket socket) -> void
          {
            if (!ec)
            {
              LOG_INFO("new connection from {}", socket.remote_endpoint().address().to_string());
              std::make_shared<Session>(std::move(socket))->start();
            }
            else
            {
              // 处理错误
              LOG_ERR("accept error: {}", ec.message());
            }
            do_accept();  // 接受下一个连接
          });
    }

   private:
    tcp_acceptor _acceptor;
  };
} // namespace asio_learn
#endif //ASIO_LEARN_TCP_SERVER_HPP