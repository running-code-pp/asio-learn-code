/** 
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-11 20:32:05
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-12 22:01:50
 * @FilePath: \asio-learn-code\include\common\internal\LoggerImpl.hpp
 * @Description: 日志实现类
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */
#ifndef COMMON_INTERNAL_LOGGER_HPP
#define COMMON_INTERNAL_LOGGER_HPP

#include <spdlog/async.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

#include <format>
#include <map>
#include <memory>
#include <vector>

#include "common/ILogger.hpp"

namespace common::details
{
  class LoggerImpl : public ILogger
  {
   public:
    LoggerImpl():_logger(nullptr) {}
    ~LoggerImpl() override
    {
      // ShutDown是可以重入的
      ShutDown();
    }
    void Init(const LoggerConfig& config) override
    {
      std::vector<spdlog::sink_ptr> _sinks;
      if (!_initialized)
      {
        _config = config;
        // 是否开启控制台日志
        if (_config.enable_console)
        {
          if (_config.enable_color)
          {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_pattern("[L]%Y-%m-%d %H:%M:%S.%e|%^%l%$%t|%P|%s|%#|%!|%v");
            _sinks.push_back(console_sink);
          }
          else
          {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
            console_sink->set_pattern("[L]%Y-%m-%d %H:%M:%S.%e|%^%l%$%t|%P|%s|%#|%!|%v");
            _sinks.push_back(console_sink);
          }
        }
        // 是否开启文件日志
        if (_config.enable_file)
        {
          auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(_config.log_file, 0, 0, false, (uint16_t)_config.max_files);
          file_sink->set_pattern("[L]%Y-%m-%d %H:%M:%S.%e|%l|%t|%P|%s|%#|%!|%v");
          _sinks.push_back(file_sink);
        }
        // 是否开启异步日志
        if (_config.enable_async)
        {
          spdlog::init_thread_pool(8192, 1);
          _logger = std::make_shared<spdlog::async_logger>(
              "Logger", _sinks.begin(), _sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
        }
        else
        {
          _logger = std::make_shared<spdlog::logger>("Logger", _sinks.begin(), _sinks.end());
        }
        _logger->set_level(transLogLevelToSpdLevel(_config.log_level));
        spdlog::set_default_logger(_logger);
        _initialized = true;
        SPDLOG_LOGGER_INFO(_logger, "Logger Initialized Success,Config:{}", FormatLoggerConfig(_config));
      }
    }

    void ShutDown() override
    {
      // 避免线程安全问题，使shutDown可重入
      static std::mutex mtx;
      std::lock_guard<std::mutex> lock(mtx);
      if (_logger && _initialized)
      {
        log(common::LogLevel::INFO, __FILE__, __LINE__, __FUNCTION__, "Logger Shutdown");
        // 这里不要调用_logger->flush() 这个函数似乎不是同步的,会造成崩溃
        spdlog::shutdown();
        _initialized = false;
      }
    }

    void SetLogLevel(LogLevel logLevel) override
    {
      _config.log_level = logLevel;
      // 更新spdlog
      if (_logger)
      {
        _logger->set_level(transLogLevelToSpdLevel(_config.log_level));
      }
    }

    LogLevel GetLogLevel() const override
    {
      return _config.log_level;
    }

    void log(LogLevel level, const char* file, int line, const char* func, const char* msg) override
    {
      if (!_logger || (level < _config.log_level))
        return;
      // 记录日志
      spdlog::source_loc loc{ file, line, func };
      _logger->log(loc, transLogLevelToSpdLevel(level), msg);
    }

   
    protected:
    spdlog::level::level_enum transLogLevelToSpdLevel(LogLevel level)
    {
      switch (level)
      {
        case LogLevel::TRACE: return spdlog::level::level_enum::trace;
        case LogLevel::DEBUG: return spdlog::level::level_enum::debug;
        case LogLevel::INFO: return spdlog::level::level_enum::info;
        case LogLevel::WARN: return spdlog::level::level_enum::warn;
        case LogLevel::ERR: return spdlog::level::level_enum::err;
        case LogLevel::FATAL: return spdlog::level::level_enum::critical;
        default: return spdlog::level::level_enum::info;
      }
    }

   private:
    std::shared_ptr<spdlog::logger> _logger;
  };
}  // namespace common::details
#endif  // COMMON_INTERNAL_LOGGER_HPP