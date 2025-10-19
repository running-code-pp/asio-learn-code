/** 
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-11 21:17:32
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-12 15:08:54
 * @FilePath: \asio-learn-code\include\common\Log.hpp
 * @Description: 日志宏
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */
#ifndef COMMON_LOG_HPP
#define COMMON_LOG_HPP

#include <yaml-cpp/yaml.h>

#include <cassert>
#include <filesystem>
#include <mutex>
#include <string_view>

#include "LoggerConfig.hpp"
#include "internal/LoggerImpl.hpp"

namespace common
{

  // 从yaml配置文件加载日志配置
  void loadLogConfig(LoggerConfig& config, const std::string& yamlPath)
  {
    // 获取当前执行文件的目录
    std::filesystem::path execPath = std::filesystem::current_path();
    std::filesystem::path fullPath = execPath / yamlPath;
    
    assert(std::filesystem::exists(fullPath));
    // 读取配置
    auto yaml = YAML::LoadFile(fullPath.string());
    if (!yaml["log"])
    {
      throw std::runtime_error("Invalid log config file: missing 'log' section");
    }
    config.log_file = yaml["log"]["log_file"].as<std::string>("log/log.txt");
    config.log_level = strToLogLevel(yaml["log"]["log_level"].as<std::string>("info"));
    config.max_file_size = yaml["log"]["max_file_size"].as<std::size_t>(1048576);
    config.max_files = yaml["log"]["max_files"].as<std::size_t>(3);
    config.log_level = strToLogLevel(yaml["log"]["log_level"].as<std::string>("info"));
    config.enable_console = yaml["log"]["enable_console"].as<bool>(true);
    config.enable_file = yaml["log"]["enable_file"].as<bool>(true);
    config.enable_color = yaml["log"]["enable_color"].as<bool>(true);
    config.enable_async = yaml["log"]["enable_async"].as<bool>(false);
  }

  // 创建日志接口,DCLP保证线程安全，全局唯一实例
  std::shared_ptr<ILogger> create_logger()
  {
    static std::shared_ptr<ILogger> logger = nullptr;
    static std::mutex mtx;
    if (!logger)
    {
      std::lock_guard<std::mutex> lock(mtx);
      if (!logger)
      {
        logger = std::make_shared<details::LoggerImpl>();
      }
    }
    return logger;
  }

#define LOG_TRACE(msg,...) common::create_logger()->logWithFmt(common::LogLevel::TRACE,__FILE__,__LINE__,__func__,msg,##__VA_ARGS__)
#define LOG_DEBUG(msg,...) common::create_logger()->logWithFmt(common::LogLevel::DEBUG,__FILE__,__LINE__,__func__,msg,##__VA_ARGS__)
#define LOG_INFO(msg,...) common::create_logger()->logWithFmt(common::LogLevel::INFO,__FILE__,__LINE__,__func__,msg,##__VA_ARGS__)
#define LOG_WARN(msg,...) common::create_logger()->logWithFmt(common::LogLevel::WARN,__FILE__,__LINE__,__func__,msg,##__VA_ARGS__)
#define LOG_ERR(msg,...) common::create_logger()->logWithFmt(common::LogLevel::ERR,__FILE__,__LINE__,__func__,msg,##__VA_ARGS__)
#define LOG_FATAL(msg,...) common::create_logger()->logWithFmt(common::LogLevel::FATAL,__FILE__,__LINE__,__func__,msg,##__VA_ARGS__)


  /**
   example in main:
   auto logger = common::create_logger();
   auto config = common::LoggerConfig();
   logger->Init(config);
   LOG_INFO("this is a {} log message","info");
   logger->shutDown();
  */

}  // namespace common

#endif  // COMMON_LOG_HPP