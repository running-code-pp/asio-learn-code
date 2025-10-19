#include <iostream>
#include <asio.hpp>
#include <spdlog/spdlog.h>

int main() {
    spdlog::info("Starting network performance benchmark...");
    
    try {
        // 这里添加您的基准测试逻辑
        spdlog::info("Running TCP throughput test...");
        spdlog::info("Running UDP latency test...");
        spdlog::info("Benchmark completed successfully");
    }
    catch (std::exception& e) {
        spdlog::error("Benchmark error: {}", e.what());
        return 1;
    }
    
    return 0;
}