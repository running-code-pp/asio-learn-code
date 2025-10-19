#include <iostream>
#include <chrono>
#include <unordered_set>
#include "common/uuid.hpp"

using namespace common;

/**
 * @brief 测试不同 UUID 生成器的性能和唯一性
 */
void test_uuid_generators() {
    const int test_count = 10000;
    
    std::cout << "=== UUID 生成器测试 ===" << std::endl;
    std::cout << "测试次数: " << test_count << std::endl << std::endl;
    
    // 1. 系统生成器测试
    {
        auto start = std::chrono::high_resolution_clock::now();
        std::unordered_set<std::string> unique_uuids;
        
        for (int i = 0; i < test_count; ++i) {
            auto uuid = UuidHelper::generate_system_string();
            unique_uuids.insert(uuid);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "🥇 系统生成器 (推荐):" << std::endl;
        std::cout << "   生成数量: " << test_count << std::endl;
        std::cout << "   唯一数量: " << unique_uuids.size() << std::endl;
        std::cout << "   重复数量: " << (test_count - unique_uuids.size()) << std::endl;
        std::cout << "   耗时: " << duration.count() << " 微秒" << std::endl;
        std::cout << "   示例: " << *unique_uuids.begin() << std::endl << std::endl;
    }
    
    // 2. 安全随机生成器测试
    {
        auto start = std::chrono::high_resolution_clock::now();
        std::unordered_set<std::string> unique_uuids;
        
        for (int i = 0; i < test_count; ++i) {
            auto uuid = uuids::to_string(UuidHelper::generate_secure_random());
            unique_uuids.insert(uuid);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "🥈 安全随机生成器:" << std::endl;
        std::cout << "   生成数量: " << test_count << std::endl;
        std::cout << "   唯一数量: " << unique_uuids.size() << std::endl;
        std::cout << "   重复数量: " << (test_count - unique_uuids.size()) << std::endl;
        std::cout << "   耗时: " << duration.count() << " 微秒" << std::endl;
        std::cout << "   示例: " << *unique_uuids.begin() << std::endl << std::endl;
    }
    
    // 3. 标准随机生成器测试
    {
        auto start = std::chrono::high_resolution_clock::now();
        std::unordered_set<std::string> unique_uuids;
        
        for (int i = 0; i < test_count; ++i) {
            auto uuid = UuidHelper::generate_random_string();
            unique_uuids.insert(uuid);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "🥉 标准随机生成器:" << std::endl;
        std::cout << "   生成数量: " << test_count << std::endl;
        std::cout << "   唯一数量: " << unique_uuids.size() << std::endl;
        std::cout << "   重复数量: " << (test_count - unique_uuids.size()) << std::endl;
        std::cout << "   耗时: " << duration.count() << " 微秒" << std::endl;
        std::cout << "   示例: " << *unique_uuids.begin() << std::endl << std::endl;
    }
    
    // 4. 名称生成器测试（确定性）
    {
        auto start = std::chrono::high_resolution_clock::now();
        std::unordered_set<std::string> unique_uuids;
        
        for (int i = 0; i < test_count; ++i) {
            std::string name = "test_name_" + std::to_string(i);
            auto uuid = uuids::to_string(UuidHelper::generate_from_domain(name));
            unique_uuids.insert(uuid);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "⚠️  名称生成器 (确定性):" << std::endl;
        std::cout << "   生成数量: " << test_count << std::endl;
        std::cout << "   唯一数量: " << unique_uuids.size() << std::endl;
        std::cout << "   重复数量: " << (test_count - unique_uuids.size()) << std::endl;
        std::cout << "   耗时: " << duration.count() << " 微秒" << std::endl;
        std::cout << "   示例: " << *unique_uuids.begin() << std::endl << std::endl;
        
        // 验证确定性：相同输入应该产生相同结果
        auto uuid1 = UuidHelper::generate_from_domain("example.com");
        auto uuid2 = UuidHelper::generate_from_domain("example.com");
        std::cout << "   确定性验证: " << (uuid1 == uuid2 ? "✅ 通过" : "❌ 失败") << std::endl << std::endl;
    }
}

/**
 * @brief 演示 UUID 实用功能
 */
void demo_uuid_utilities() {
    std::cout << "=== UUID 实用功能演示 ===" << std::endl;
    
    // 生成不同格式的UUID
    auto uuid = UuidHelper::generate_system();
    std::cout << "标准格式: " << uuids::to_string(uuid) << std::endl;
    std::cout << "紧凑格式: " << UuidHelper::generate_compact_string() << std::endl;
    std::cout << "版本信息: v" << UuidHelper::get_version(uuid) << std::endl;
    
    // 验证功能
    std::string valid_uuid = "123e4567-e89b-12d3-a456-426614174000";
    std::string invalid_uuid = "not-a-uuid";
    std::cout << "UUID验证: " << valid_uuid << " -> " << (UuidHelper::is_valid(valid_uuid) ? "✅ 有效" : "❌ 无效") << std::endl;
    std::cout << "UUID验证: " << invalid_uuid << " -> " << (UuidHelper::is_valid(invalid_uuid) ? "✅ 有效" : "❌ 无效") << std::endl;
    
    // nil UUID
    auto nil_uuid = UuidHelper::nil();
    std::cout << "Nil UUID: " << uuids::to_string(nil_uuid) << " (is_nil: " << UuidHelper::is_nil(nil_uuid) << ")" << std::endl;
    
    // 基于域名和URL的UUID
    auto domain_uuid = UuidHelper::generate_from_domain("github.com");
    auto url_uuid = UuidHelper::generate_from_url("https://github.com/user/repo");
    std::cout << "域名UUID: " << uuids::to_string(domain_uuid) << std::endl;
    std::cout << "URL UUID: " << uuids::to_string(url_uuid) << std::endl;
}

int main() {
    try {
        test_uuid_generators();
        demo_uuid_utilities();
        
        std::cout << "\n💡 总结建议:" << std::endl;
        std::cout << "• 高安全性应用 → 使用 generate_system()" << std::endl;
        std::cout << "• 一般应用场景 → 使用 generate_secure_random()" << std::endl;
        std::cout << "• 高性能需求 → 使用 generate_random()" << std::endl;
        std::cout << "• 需要确定性 → 使用 generate_name_based()" << std::endl;
        std::cout << "• 便捷使用 → 使用 new_uuid() 全局函数" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}