/** 
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-11 20:21:07
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-12 15:09:05
 * @FilePath: \asio-learn-code\include\common\LoggerConfig.hpp
 * @Description: 日志配置
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */
#ifndef COMMON_LOGGERCONFIG_HPP
#define COMMON_LOGGERCONFIG_HPP

#include<string>
#include<cstdint>

namespace common
{
enum class LogLevel{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERR,
    FATAL
};

struct LoggerConfig {
    std::string log_file = "app.log"; // 默认日志文件名
    LogLevel log_level = LogLevel::INFO;    // 默认日志级别
    std::size_t max_file_size = 1048576; // 1 MB
    std::size_t max_files = 3;          // 保留的最大日志文件数
    bool enable_console = true; // 是否启用控制台日志
    bool enable_file = true;    // 是否启用文件日志
    bool enable_color = true; // 是否启用彩色日志
    bool enable_async = false; // 是否启用异步日志
};

LogLevel strToLogLevel(const std::string& level) {
    if (level == "trace") return LogLevel::TRACE;
    if (level == "debug") return LogLevel::DEBUG;
    if (level == "info") return LogLevel::INFO;
    if (level == "warn") return LogLevel::WARN;
    if (level == "error") return LogLevel::ERR;
    if (level == "fatal") return LogLevel::FATAL;
    return LogLevel::INFO; // 默认级别
}

std::string logLevelToStr(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default:              return "INFO";
    }
}


std::string FormatLoggerConfig(const LoggerConfig& config) {
    return "LoggerConfig { log_file: " + config.log_file +
           ", log_level: " + common::logLevelToStr(config.log_level) +
           ", max_file_size: " + std::to_string(config.max_file_size) +
           ", max_files: " + std::to_string(config.max_files) +
           ", enable_console: " + (config.enable_console ? "true" : "false") +
           ", enable_file: " + (config.enable_file ? "true" : "false") +
           ", enable_color: " + (config.enable_color ? "true" : "false") +
           ", enableAsync: " + (config.enable_async ? "true" : "false") +
           " }";
}
}  // namespace common
#endif  // COMMON_LOGGERCONFIG_HPP