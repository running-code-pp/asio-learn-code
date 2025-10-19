
/**
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-18 22:49:19
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-18 23:37:21
 * @FilePath: \asio-learn-code\src\asio_learn\asio_ssl_example.cpp
 * @Description: asio + ssl 建议tcpserver
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */

/**
 SLL SECURE Sockets Layer
 TLS Transport Layer Security
 SSL/TLS协议是为网络通信提供安全保障的加密协议，广泛应用
 TLS是SSL的继任者，提供更强的安全性和性能
 */

#include "asio_learn/ssl/ssl_tcp_server.hpp"
#include "common/Log.hpp"

int main()
{
  // 初始化日志配置并保持其生命周期
  common::LoggerConfig config;
  common::loadLogConfig(config, "log.yaml");
  auto logger = common::create_logger();
  logger->Init(config);
  asio::io_context ioc;
  // 创建ssl上下文
  asio::ssl::context ssl_ctx(asio::ssl::context::sslv23);

  ssl_ctx.set_options(
      asio::ssl::context::default_workarounds | asio::ssl::context::no_sslv2 | asio::ssl::context::no_sslv3 |
      asio::ssl::context::no_tlsv1 | asio::ssl::context::no_tlsv1_1 | asio::ssl::context::single_dh_use);

  // 加载ca证书和私钥
  ssl_ctx.use_certificate_chain_file("resources/servercert.pem");

  ssl_ctx.use_private_key_file("resources/serverkey.pem", asio::ssl::context::pem);

  asio_learn::ssl::SslTcpServer server(ioc, 4433, std::move(ssl_ctx));
  server.run();
  ioc.run();
  
  return 0;
}
