/**
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-12 15:22:30
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-12 22:55:16
 * @FilePath: \asio-learn-code\include\asio_learn\public.hpp
 * @Description:
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */
/**
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-12 15:22:30
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-12 15:29:00
 * @FilePath: \asio-learn-code\include\asio_learn\public.hpp
 * @Description: 定义asio中类型的别名，方便书写
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */

#ifndef ASIO_LEARN_PUBLIC_HPP
#define ASIO_LEARN_PUBLIC_HPP
#include <asio.hpp>
#include <cstdint>
#include <sstream>
#include <thread>
#define DEFAULT_BUFFER_LEN 1024
namespace asio_learn
{
  using int16 = std::int16_t;
  using int32 = std::int32_t;
  using uint16 = std::uint16_t;
  using uint32 = std::uint32_t;
  using uint64 = std::uint64_t;
  using uint64 = std::uint64_t;
  using tcp_socket = asio::ip::tcp::socket;
  using tcp_endpoint = asio::ip::tcp::endpoint;
  using tcp_acceptor = asio::ip::tcp::acceptor;
  using udp_socket = asio::ip::udp::socket;
  using udp_endpoint = asio::ip::udp::endpoint;
  using error_code = asio::error_code;
  using io_context = asio::io_context;
  using strand = asio::io_context::strand;
  using steady_timer = asio::steady_timer;
  using system_timer = asio::system_timer;
  using high_resolution_timer = asio::high_resolution_timer;

  std::string threadId_to_str()
  {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return ss.str();
  }
}  // namespace asio_learn

#endif  // ASIO_LEARN_PUBLIC_HPP