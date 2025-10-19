/**
 * @file uuid_usage_example.cpp
 * @brief UUID 工具类使用示例
 */

#include <iostream>
#include <vector>
#include <unordered_set>
#include "common/uuid.hpp"
#include "common/Log.hpp"

using namespace common;

int main() {
    // 初始化日志
    LoggerConfig config;
    loadLogConfig(config, "log.yaml");
    auto logger = create_logger();
    logger->Init(config);
    
    LOG_INFO("=== UUID Helper Usage Examples ===");
    
    // 1. 生成随机 UUID
    LOG_INFO("1. Random UUID Generation:");
    for (int i = 0; i < 3; ++i) {
        auto uuid_str = UuidHelper::generate_random_string();
        auto uuid_compact = UuidHelper::generate_compact_string();
        LOG_INFO("  Random UUID {}: {}", i + 1, uuid_str);
        LOG_INFO("  Compact UUID {}: {}", i + 1, uuid_compact);
    }
    
    // 2. 便捷函数
    LOG_INFO("2. Convenience Functions:");
    LOG_INFO("  new_uuid(): {}", new_uuid());
    LOG_INFO("  new_uuid_compact(): {}", new_uuid_compact());
    
    // 3. 基于时间的 UUID
    LOG_INFO("3. Time-based UUID (v1):");
    for (int i = 0; i < 2; ++i) {
        auto time_uuid = UuidHelper::generate_time_based_string();
        LOG_INFO("  Time UUID {}: {}", i + 1, time_uuid);
    }
    
    // 4. UUID 解析和验证
    LOG_INFO("4. UUID Parsing and Validation:");
    std::vector<std::string> test_strings = {
        "550e8400-e29b-41d4-a716-446655440000",  // 有效
        "invalid-uuid-string",                    // 无效
        "123e4567e89b12d3a456426614174000",      // 紧凑格式但无效
        new_uuid()                                // 新生成的有效UUID
    };
    
    for (const auto& str : test_strings) {
        bool valid = is_uuid(str);
        LOG_INFO("  '{}' is {}", str, valid ? "valid" : "invalid");
        
        if (valid) {
            auto uuid_opt = UuidHelper::from_string(str);
            if (uuid_opt) {
                int version = UuidHelper::get_version(*uuid_opt);
                LOG_INFO("    Version: {}", version);
            }
        }
    }
    
    // 5. nil UUID 操作
    LOG_INFO("5. Nil UUID Operations:");
    auto nil_uuid = UuidHelper::nil();
    LOG_INFO("  Nil UUID: {}", uuids::to_string(nil_uuid));
    LOG_INFO("  Is nil: {}", UuidHelper::is_nil(nil_uuid));
    
    auto random_uuid = UuidHelper::generate_random();
    LOG_INFO("  Random UUID is nil: {}", UuidHelper::is_nil(random_uuid));
    
    // 6. UUID 比较
    LOG_INFO("6. UUID Comparison:");
    auto uuid1 = UuidHelper::generate_random();
    auto uuid2 = UuidHelper::generate_random();
    auto uuid1_copy = uuid1;
    
    LOG_INFO("  UUID1: {}", uuids::to_string(uuid1));
    LOG_INFO("  UUID2: {}", uuids::to_string(uuid2));
    LOG_INFO("  UUID1 == UUID1_copy: {}", uuid1 == uuid1_copy);
    LOG_INFO("  UUID1 == UUID2: {}", uuid1 == uuid2);
    
    int cmp_result = UuidHelper::compare(uuid1, uuid2);
    std::string cmp_str = (cmp_result == 0) ? "equal" : 
                         (cmp_result < 0) ? "less than" : "greater than";
    LOG_INFO("  UUID1 {} UUID2", cmp_str);
    
    // 7. 唯一性测试
    LOG_INFO("7. Uniqueness Test:");
    constexpr int test_count = 10000;
    std::unordered_set<std::string> uuid_set;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < test_count; ++i) {
        uuid_set.insert(new_uuid());
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    LOG_INFO("  Generated {} UUIDs", test_count);
    LOG_INFO("  Unique UUIDs: {}", uuid_set.size());
    LOG_INFO("  Duplicates: {}", test_count - uuid_set.size());
    LOG_INFO("  Time taken: {} microseconds", duration.count());
    LOG_INFO("  Generation rate: {} UUIDs/second", 
             static_cast<double>(test_count) / duration.count() * 1000000);
    
    // 8. 用作容器键值
    LOG_INFO("8. Using UUID as Map Key:");
    std::map<uuid, std::string> uuid_map;
    
    for (int i = 0; i < 3; ++i) {
        auto key = UuidHelper::generate_random();
        uuid_map[key] = "Value " + std::to_string(i + 1);
        LOG_INFO("  Stored: {} -> {}", uuids::to_string(key), uuid_map[key]);
    }
    
    LOG_INFO("  Map size: {}", uuid_map.size());
    
    logger->ShutDown();
    return 0;
}