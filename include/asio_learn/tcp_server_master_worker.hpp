/**
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-13 14:41:14
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-13 14:43:16
 * @FilePath: \asio-learn-code\include\asio_learn\tcp_server_master_worker.hpp
 * @Description: 主从架构tcp服务器
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */
#ifndef ASIO_LEARN_TCP_SERVER_MASTER_WORKER_HPP
#define ASIO_LEARN_TCP_SERVER_MASTER_WORKER_HPP
#include <atomic>
#include <exception>
#include <vector>

#include "asio_learn/public.hpp"
#include "common/Log.hpp"

namespace asio_learn
{
  namespace details
  {
    // 会话类 - 管理单个连接的生命周期
    class Session : public std::enable_shared_from_this<Session>
    {
     public:
      Session(tcp_socket socket, uint64_t worker_id) : _socket(std::move(socket)), _worker_id(worker_id), _buffer(1024)
      {
      }

      void start()
      {
        LOG_INFO("worker {} started session with {}", _worker_id, _socket.remote_endpoint().address().to_string());
        do_read();
      }

     private:
      void do_read()
      {
        auto self = shared_from_this();  // 保持会话存活
        _socket.async_read_some(
            asio::buffer(_buffer),
            [self, this](const asio::error_code& ec, std::size_t bytes_read)
            {
              if (!ec)
              {
                LOG_INFO("thread{}---worker {} read {} bytes", threadId_to_str(), _worker_id, bytes_read);
                process_data(bytes_read);
                do_write(bytes_read);
              }
              else if (ec != asio::error::eof)
              {
                LOG_ERR("worker {} read error: {}", _worker_id, ec.message());
              }
              else
              {
                LOG_INFO("worker {} client disconnected", _worker_id);
              }
            });
      }

      void do_write(std::size_t bytes_to_write)
      {
        auto self = shared_from_this();  // 保持会话存活
        asio::async_write(
            _socket,
            asio::buffer(_buffer.data(), bytes_to_write),
            [self, this](const asio::error_code& ec, std::size_t /*bytes_written*/)
            {
              if (!ec)
              {
                LOG_INFO("thread{}---worker {} write completed", threadId_to_str(), _worker_id);
                do_read();  // 继续读取
              }
              else
              {
                LOG_ERR("worker {} write error: {}", _worker_id, ec.message());
              }
            });
      }

      void process_data(std::size_t bytes_read)
      {
        LOG_INFO("worker {} processed {} bytes: {}", _worker_id, bytes_read, std::string(_buffer.data(), bytes_read));
        // 这里可以添加具体的业务逻辑
      }

      tcp_socket _socket;
      uint64_t _worker_id;
      std::vector<char> _buffer;
    };

    // 工作对象
    class Worker
    {
     public:
      Worker(uint64 id) : _id(id), _ioc(), _work_guard(asio::make_work_guard(_ioc))
      {
        LOG_INFO("worker {} has been created", _id);
      }

      ~Worker()
      {
        stop();
        LOG_INFO("worker {} has been destroyed", _id);
      }

      void run()
      {
        LOG_INFO("worker {} is running", _id);
        _ioc.run();
        LOG_INFO("worker {} has stopped", _id);
      }

      void stop()
      {
        _ioc.stop();
      }

      void handle_new_connection(tcp_socket socket)
      {
        // 方法1: 使用原始句柄重新绑定 (适用于需要真正转移执行上下文的情况)
        try
        {
          // 获取socket的协议信息
          bool is_v6 = socket.remote_endpoint().address().is_v6();
          // win8.1之前不支持
          auto native_handle = socket.release();

          auto worker_socket = tcp_socket(_ioc);
          // 根据实际协议类型分配
          if (is_v6)
          {
            worker_socket.assign(tcp_socket::protocol_type::v6(), native_handle);
          }
          else
          {
            worker_socket.assign(tcp_socket::protocol_type::v4(), native_handle);
          }

          // 创建会话并在 worker 的 io_context 中处理
          auto session = std::make_shared<Session>(std::move(worker_socket), _id);
          asio::post(_ioc, [session]() { session->start(); });
        }
        catch (const std::exception& e)
        {
          LOG_ERR("worker {} failed to handle new connection: {}", _id, e.what());
        }

        /*
        // 方法2: 简化版本 - 直接使用原socket，但在worker线程中处理
        // (socket仍绑定到master的io_context，但业务逻辑在worker线程执行)
        auto session = std::make_shared<Session>(std::move(socket), _id);
        asio::post(_ioc, [session]() {
          session->start();
        });
        */
      }

     private:
      uint64 _id;
      io_context _ioc;
      // 守护_ioc不退出
      asio::executor_work_guard<asio::io_context::executor_type> _work_guard;
    };
  }  // namespace details

  class MasterWorkerTcpServer
  {
   public:
    MasterWorkerTcpServer(const tcp_endpoint& endpoint, size_t worker_count) 
      : _ioc()
      , _acceptor(_ioc)  // 只在初始化列表中创建 acceptor
      , _next_worker_id(0)
    {
      try {
        // 在构造函数体中进行配置，避免初始化列表中的复杂操作
        initialize_acceptor(endpoint);
        initialize_workers(worker_count);
        
        LOG_INFO("MasterWorkerTcpServer initialized successfully");
      }
      catch (const std::exception& e) {
        LOG_ERR("Failed to initialize MasterWorkerTcpServer: {}", e.what());
        cleanup();
        throw;
      }
    }
    
     ~MasterWorkerTcpServer()
    {
      LOG_INFO("Destroying MasterWorkerTcpServer...");
      cleanup();
    }

    void run()
    {
      try {
        LOG_INFO("Starting to accept connections...");
        do_accept();  // 在 run 中启动 accept，而不是构造函数中
        _ioc.run();
      }
      catch (const std::exception& e) {
        LOG_ERR("Server run error: {}", e.what());
        throw;
      }
    }
    void stop()
    {
      LOG_INFO("Stopping MasterWorkerTcpServer...");
      _ioc.stop();
      cleanup();
    }

   private:
    void initialize_acceptor(const tcp_endpoint& endpoint)
    {
      // 分步骤初始化 acceptor，便于错误处理
      _acceptor.open(endpoint.protocol());
      
      // 设置 SO_REUSEADDR 选项
      _acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
      
      _acceptor.bind(endpoint);
      _acceptor.listen();
      
      LOG_INFO("Acceptor bound to {}:{}", endpoint.address().to_string(), endpoint.port());
    }
    
    void initialize_workers(size_t worker_count)
    {
      if (worker_count == 0)
      {
        LOG_WARN("param worker_count is invalid, use the default value");
        worker_count = std::thread::hardware_concurrency() == 0 ? 4 : std::thread::hardware_concurrency() * 2;
      }
      
      _workers.reserve(worker_count);
      _worker_threads.reserve(worker_count);
      
      for (size_t i = 0; i < worker_count; ++i)
      {
        auto worker = std::make_shared<details::Worker>(i);
        _workers.push_back(worker);
        _worker_threads.emplace_back([worker]() { worker->run(); });
      }
      
      LOG_INFO("Created {} worker threads", worker_count);
    }
    
    void cleanup()
    {
      // 清理资源
      if (_acceptor.is_open())
      {
        std::error_code ec;
        _acceptor.close(ec);
      }
      
      for (auto& worker : _workers)
      {
        if (worker) worker->stop();
      }
      
      for (auto& th : _worker_threads)
      {
        if (th.joinable()) th.join();
      }
    }

   
    void do_accept()
    {
      _acceptor.async_accept(
          [this](std::error_code ec, tcp_socket socket)
          {
            if (!ec)
            {
                LOG_INFO("thread{}---Accepted connection from {}", threadId_to_str(), socket.remote_endpoint().address().to_string());
              // 分配给下一个worker
              auto& worker = _workers[_next_worker_id++ % _workers.size()];
              // 让工作线程去处理
              worker->handle_new_connection(std::move(socket));
            }
            else
            {
              LOG_ERR("Accept error: {}", ec.message());
            }

            // 继续接受下一个连接
            do_accept();
          });
    }

   private:
    // 注意：成员变量的声明顺序很重要，_ioc 必须在 _acceptor 之前
    io_context _ioc;
    tcp_acceptor _acceptor;
    std::vector<std::shared_ptr<details::Worker>> _workers;
    std::vector<std::thread> _worker_threads;
    std::atomic<uint64> _next_worker_id;
  };
}  // namespace asio_learn
#endif  // ASIO_LEARN_TCP_SERVER_MASTER_WORKER_HPP