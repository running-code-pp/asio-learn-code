/** 
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-13 15:55:52
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-13 19:40:10
 * @FilePath: \asio-learn-code\include\common\uuid.hpp
 * @Description: 对于stduuid的简单封装，提供更易用的接口
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */
#ifndef COMMON_UUID_HPP
#define COMMON_UUID_HPP

#include <string>
#include <optional>
#define UUID_SYSTEM_GENERATOR // 启用系统级UUID生成器 要写在stduuid.hpp前面
#include "internal/stduuid.hpp"
namespace common {

using uuid = uuids::uuid;

/**
 * @brief UUID 工具类，提供常用的 UUID 操作接口
 */
class UuidHelper {
public:
    /**
     * @brief 生成系统级 UUID (最高安全性) ⭐推荐⭐
     * 使用操作系统提供的原生UUID生成器，重复概率最小
     * 
     * 平台支持：
     * - Windows: CoCreateGuid (COM GUID API)  
     * - Linux: uuid_generate (libuuid 库)
     * - macOS: CFUUIDCreate (Core Foundation)
     * 
     * 注意：需要在编译时定义 UUID_SYSTEM_GENERATOR 宏才能启用
     * 编译示例：g++ -DUUID_SYSTEM_GENERATOR your_code.cpp
     * 
     * @return uuid 对象
     */
    static uuid generate_system() {
#ifdef UUID_SYSTEM_GENERATOR
        static thread_local uuids::uuid_system_generator gen;
        return gen();
#else
        // 系统生成器不可用时，回退到安全随机生成器
        // 要启用系统生成器，请在编译时添加：-DUUID_SYSTEM_GENERATOR
        return generate_secure_random();
#endif
    }
    
    /**
     * @brief 生成系统级 UUID 字符串 ⭐推荐⭐
     * @return UUID 字符串
     */
    static std::string generate_system_string() {
        return uuids::to_string(generate_system());
    }
    
    /**
     * @brief 生成安全随机 UUID (v4) - 高安全性
     * 使用 std::random_device 作为种子，不依赖伪随机算法
     * @return uuid 对象
     */
    static uuid generate_secure_random() {
        static thread_local std::random_device rd;
        static thread_local uuids::basic_uuid_random_generator<std::random_device> gen{rd};
        return gen();
    }
    
    /**
     * @brief 生成随机 UUID (v4) - 标准实现
     * 使用 Mersenne Twister 引擎，性能高但安全性略低
     * @return uuid 对象
     */
    static uuid generate_random() {
        static thread_local std::random_device rd;
        static thread_local std::mt19937 gen_engine{rd()};
        static thread_local uuids::uuid_random_generator gen{gen_engine};
        return gen();
    }
    
    /**
     * @brief 生成随机 UUID 并转为字符串 (v4)
     * @return UUID 字符串，格式: xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
     */
    static std::string generate_random_string() {
        return uuids::to_string(generate_random());
    }
    
    /**
     * @brief 生成基于名称的 UUID (v5) - 确定性生成
     * 相同的命名空间和名称总是生成相同的UUID，重复概率为0（但可预测）
     * @param namespace_uuid 命名空间UUID
     * @param name 名称字符串
     * @return uuid 对象
     */
    static uuid generate_name_based(const uuid& namespace_uuid, const std::string& name) {
        uuids::uuid_name_generator gen(namespace_uuid);
        return gen(name);
    }
    
    /**
     * @brief 生成基于时间的 UUID (v1) - 实验性功能
     * 注意：这是实验性功能，不建议在生产环境使用
     * @return uuid 对象
     */
    static uuid generate_time_based() {
#ifdef UUID_TIME_GENERATOR
        static thread_local uuids::uuid_time_generator gen;
        return gen();
#else
        // 时间生成器不可用时，使用系统生成器
        return generate_system();
#endif
    }
    
    /**
     * @brief 生成紧凑的 UUID 字符串（无连字符）
     * @return 32位十六进制字符串
     */
    static std::string generate_compact_string() {
        auto uuid = generate_random();
        auto str = uuids::to_string(uuid);
        str.erase(std::remove(str.begin(), str.end(), '-'), str.end());
        return str;
    }
    

    /**
     * @brief 从字符串解析 UUID
     * @param str UUID 字符串
     * @return 解析成功返回 uuid 对象，失败返回 nullopt
     */
    static std::optional<uuid> from_string(const std::string& str) {
        auto result = uuids::uuid::from_string(str);
        if (result.has_value()) {
            return result.value();
        }
        return std::nullopt;
    }
    
    /**
     * @brief 验证字符串是否为有效的 UUID 格式
     * @param str 要验证的字符串
     * @return true 如果是有效的 UUID 格式
     */
    static bool is_valid(const std::string& str) {
        return from_string(str).has_value();
    }
    
    /**
     * @brief 生成 nil UUID（全零 UUID）
     * @return nil uuid 对象
     */
    static uuid nil() {
        return uuids::uuid{};
    }
    
    /**
     * @brief 判断是否为 nil UUID
     * @param id 要检查的 UUID
     * @return true 如果是 nil UUID
     */
    static bool is_nil(const uuid& id) {
        return id.is_nil();
    }
    
    /**
     * @brief 比较两个 UUID
     * @param lhs 左操作数
     * @param rhs 右操作数
     * @return 0 相等，<0 lhs < rhs，>0 lhs > rhs
     */
    static int compare(const uuid& lhs, const uuid& rhs) {
        if (lhs < rhs) return -1;
        else if(lhs == rhs) return 0;
        else return 1;
    }
    
    /**
     * @brief 获取 UUID 的版本号
     * @param id UUID 对象
     * @return 版本号 (1-5)
     */
    static int get_version(const uuid& id) {
        return static_cast<int>(id.version());
    }
    
    /**
     * @brief 获取 UUID 的变体
     * @param id UUID 对象
     * @return 变体类型
     */
    static uuids::uuid_variant get_variant(const uuid& id) {
        return id.variant();
    }
    
    // 预定义的命名空间 UUID（RFC 4122 标准）
    static constexpr const char* NS_DNS = "6ba7b810-9dad-11d1-80b4-00c04fd430c8";
    static constexpr const char* NS_URL = "6ba7b811-9dad-11d1-80b4-00c04fd430c8"; 
    static constexpr const char* NS_OID = "6ba7b812-9dad-11d1-80b4-00c04fd430c8";
    static constexpr const char* NS_X500 = "6ba7b814-9dad-11d1-80b4-00c04fd430c8";
    
    /**
     * @brief 基于域名生成 UUID
     * @param domain 域名，如 "example.com"
     * @return uuid 对象
     */
    static uuid generate_from_domain(const std::string& domain) {
        auto ns = from_string(NS_DNS);
        if (ns) {
            return generate_name_based(ns.value(), domain);
        }
        return generate_system(); // 备用方案
    }
    
    /**
     * @brief 基于URL生成 UUID  
     * @param url URL地址，如 "https://example.com/path"
     * @return uuid 对象
     */
    static uuid generate_from_url(const std::string& url) {
        auto ns = from_string(NS_URL);
        if (ns) {
            return generate_name_based(ns.value(), url);
        }
        return generate_system(); // 备用方案
    }
};

/**
 * @brief 便捷函数：生成随机 UUID 字符串
 */
inline std::string new_uuid() {
    return UuidHelper::generate_random_string();
}

/**
 * @brief 便捷函数：生成紧凑 UUID 字符串
 */
inline std::string new_uuid_compact() {
    return UuidHelper::generate_compact_string();
}

/**
 * @brief 便捷函数：验证 UUID 字符串
 */
inline bool is_uuid(const std::string& str) {
    return UuidHelper::is_valid(str);
}

/*
 * ========================================================================
 * UUID 生成器安全性分析和重复概率比较
 * ========================================================================
 * 
 * 🔐 安全性等级排序（重复概率从小到大）：
 * 
 * 1. 🥇 generate_system() - 系统级生成器 【推荐】
 *    - 重复概率：理论上最低（~5.3×10^-37）
 *    - 使用操作系统级的加密安全随机数生成器
 *    - Windows: CryptGenRandom, Linux: /dev/urandom
 *    - 抗攻击性：最强，无法预测
 *    - 性能：中等（系统调用开销）
 * 
 * 2. 🥈 generate_secure_random() - 安全随机生成器
 *    - 重复概率：极低（~5.3×10^-37）
 *    - 使用硬件随机数种子的 std::random_device
 *    - 抗攻击性：强，难以预测
 *    - 性能：较好（用户态实现）
 * 
 * 3. 🥉 generate_random() - 标准随机生成器
 *    - 重复概率：低（~5.3×10^-37）
 *    - 使用 Mersenne Twister 伪随机算法
 *    - 抗攻击性：中等，算法可预测但种子随机
 *    - 性能：最好（纯算法实现）
 * 
 * 4. ⚠️  generate_name_based() - 确定性生成器
 *    - 重复概率：0%（相同输入必定相同输出）
 *    - 使用 SHA-1 哈希算法
 *    - 抗攻击性：弱，完全可预测
 *    - 用途：需要可重现的UUID时使用
 * 
 * 5. ⚠️  generate_time_based() - 时间戳生成器（实验性）
 *    - 重复概率：中等（依赖时钟精度和MAC地址）
 *    - 包含时间戳和MAC地址信息
 *    - 抗攻击性：弱，可能泄露主机信息
 *    - 用途：需要时间排序的UUID时考虑使用
 * 
 * 📊 理论重复概率计算：
 * - UUID v4 (随机): 2^122 个可能值 ≈ 5.3×10^36
 * - 生日悖论：50% 概率重复需要 2^61 ≈ 2.3×10^18 个UUID
 * - 实际应用中，重复概率可忽略不计
 * 
 * 💡 选择建议：
 * - 高安全性应用：使用 generate_system()
 * - 一般应用：使用 generate_secure_random()
 * - 高性能场景：使用 generate_random()
 * - 需要确定性：使用 generate_name_based()
 * - 需要时间信息：谨慎使用 generate_time_based()
 * 
 * ⚡ 性能测试建议：
 * benchmark::DoNotOptimize(UuidHelper::generate_system());
 * benchmark::DoNotOptimize(UuidHelper::generate_secure_random());
 * benchmark::DoNotOptimize(UuidHelper::generate_random());
 * 
 */

} // namespace common

#endif // COMMON_UUID_HPP
