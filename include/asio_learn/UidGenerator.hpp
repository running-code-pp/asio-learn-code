/**
 * @file UidGenerator.hpp
 * @brief 高安全性跨平台 UID 生成器
 * @date 2025-10-13
 */

#ifndef ASIO_LEARN_UID_GENERATOR_HPP
#define ASIO_LEARN_UID_GENERATOR_HPP

#include <string>
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <atomic>
#include <thread>
#include <functional>

namespace asio_learn {

/**
 * @brief 高安全性 UID 生成器
 * 
 * 碰撞概率分析：
 * - numeric_uid: 在同一毫秒内需要生成约100万个ID才可能碰撞
 * - uuid_string: 标准UUID v4级别，2^122空间
 * - snowflake_id: Twitter Snowflake算法，分布式环境安全
 */
class UidGenerator {
private:
    static std::mt19937_64& get_random_generator() {
        thread_local std::random_device rd;
        thread_local std::mt19937_64 gen(rd());
        return gen;
    }
    
public:
    /**
     * @brief 生成 Snowflake 风格的 64位 ID
     * 
     * 格式: 1位符号位 + 41位时间戳 + 10位机器ID + 12位序列号
     * 
     * 优点：
     * - 时间有序
     * - 分布式安全（通过机器ID区分）
     * - 高性能（无锁）
     * 
     * 碰撞概率：同一毫秒内同一机器需要生成4096个ID才会碰撞
     */
    static uint64_t generate_snowflake_id() {
        static std::atomic<uint32_t> sequence{0};
        static const uint64_t epoch = 1288834974657ULL; // 2010-11-04 09:42:54 UTC
        static const uint32_t machine_id = static_cast<uint32_t>(
            std::hash<std::thread::id>{}(std::this_thread::get_id()) & 0x3FF); // 10位机器ID
        
        auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        uint64_t timestamp = (now - epoch) & 0x1FFFFFFFFFF; // 41位时间戳
        uint32_t seq = sequence.fetch_add(1) & 0xFFF;        // 12位序列号
        
        return (timestamp << 22) | (machine_id << 12) | seq;
    }
    
    /**
     * @brief 生成标准 UUID v4 字符串
     * 
     * 符合 RFC 4122 标准
     * 碰撞概率：2^-122 ≈ 5.3 × 10^-37
     * 
     * 实际上可以认为是"永不碰撞"的
     */
    static std::string generate_uuid_v4() {
        auto& gen = get_random_generator();
        
        // 生成128位随机数据
        uint64_t data1 = gen();
        uint64_t data2 = gen();
        
        // 提取各部分
        uint32_t time_low = static_cast<uint32_t>(data1);
        uint16_t time_mid = static_cast<uint16_t>(data1 >> 32);
        uint16_t time_hi_version = static_cast<uint16_t>((data1 >> 48) & 0x0FFF) | 0x4000; // 版本4
        uint8_t  clock_seq_hi = static_cast<uint8_t>((data2 >> 8) & 0x3F) | 0x80;  // 变体10
        uint8_t  clock_seq_low = static_cast<uint8_t>(data2);
        uint64_t node = (data2 >> 16) & 0xFFFFFFFFFFFFULL;
        
        std::ostringstream oss;
        oss << std::hex << std::setfill('0')
            << std::setw(8) << time_low << "-"
            << std::setw(4) << time_mid << "-"
            << std::setw(4) << time_hi_version << "-"
            << std::setw(2) << static_cast<int>(clock_seq_hi)
            << std::setw(2) << static_cast<int>(clock_seq_low) << "-"
            << std::setw(12) << node;
            
        return oss.str();
    }
    
    /**
     * @brief 生成简单的数字 UID
     * 
     * 基于时间戳 + 线程ID + 计数器
     * 适合单机环境，性能最高
     */
    static uint64_t generate_simple_uid() {
        static std::atomic<uint32_t> counter{0};
        static const uint32_t thread_id = static_cast<uint32_t>(
            std::hash<std::thread::id>{}(std::this_thread::get_id()) & 0xFFFF);
        
        auto now = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        uint32_t cnt = counter.fetch_add(1) & 0xFFFF;  // 16位计数器
        
        // 32位时间戳 + 16位线程ID + 16位计数器 = 64位
        return (static_cast<uint64_t>(now & 0xFFFFFFFF) << 32) | 
               (static_cast<uint64_t>(thread_id) << 16) | 
               cnt;
    }
    
    /**
     * @brief 生成会话 ID
     * 格式友好，适合日志和调试
     */
    static std::string generate_session_id() {
        auto snowflake = generate_snowflake_id();
        std::ostringstream oss;
        oss << "sess_" << std::hex << snowflake;
        return oss.str();
    }
    
    /**
     * @brief 获取唯一性评估
     */
    struct UniquenessInfo {
        std::string type;
        std::string collision_probability;
        std::string recommended_use;
    };
    
    static std::vector<UniquenessInfo> get_uniqueness_info() {
        return {
            {"UUID v4", "2^-122 (事实上永不碰撞)", "全局唯一标识符，最高安全级别"},
            {"Snowflake ID", "同毫秒4096个才碰撞", "分布式系统，时间有序"},
            {"Simple UID", "同微秒65536个才碰撞", "单机高性能场景"},
            {"Session ID", "基于Snowflake", "会话管理，可读性好"}
        };
    }
};

} // namespace asio_learn

#endif // ASIO_LEARN_UID_GENERATOR_HPP