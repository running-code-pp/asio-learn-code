/** 
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-11 20:18:25
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-12 15:08:43
 * @FilePath: \asio-learn-code\include\common\ILogger.hpp
 * @Description: 日志接口类
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */
#ifndef COMMON_ILOGGER_HPP
#define COMMON_ILOGGER_HPP
#include <string>
#include <format>
#include "LoggerConfig.hpp"

namespace common
{
  /**
   * @brief 日志接口
   */
  class ILogger
  {
   public:
    ILogger():_initialized(false) {};
    virtual ~ILogger() = default;
    // 通过配置初始化日志系统
    virtual void Init(const LoggerConfig& config) = 0;
    // 是否初始化
    virtual bool IsInitialized() const
    {
      return _initialized;
    }
    // 获取当前日志配置
    virtual LoggerConfig GetConfig() const
    {
      return _config;
    }
    // 关闭日志
    virtual void ShutDown() = 0;
    // 设置日志级别
    virtual void SetLogLevel(LogLevel logLevel) = 0;
    // 获取日志界别
    virtual LogLevel GetLogLevel() const = 0;
    // 记录日志
   protected:
    virtual void log(LogLevel level, const char* file, int line, const char* func, const char* msg) = 0;

   public:
    template<typename... Args>
    void logWithFmt(LogLevel level, const char* file, int line, const char* func, const char* fmt, Args&&... args)
    {
      std::string formatStr;
      if constexpr (sizeof...(args) == 0)
      {
        formatStr = fmt;
      }
      else
      {
        formatStr = std::vformat(fmt, std::make_format_args(args...));
      }
      // 调用正确的 log 函数签名
      log(level, file, line, func, formatStr.c_str());
    }

   protected:
    LoggerConfig _config;
    bool _initialized;
  };

}  // namespace common

#endif  // COMMON_ILOGGER_HPP