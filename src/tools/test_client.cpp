#include <iostream>
#include <asio.hpp>
#include <spdlog/spdlog.h>

int main() {
    spdlog::info("Starting test client...");
    
    try {
        asio::io_context io_context;
        
        // 测试客户端逻辑
        spdlog::info("Connecting to test server...");
        
        // 这里添加您的测试客户端实现
        
        spdlog::info("Test client finished");
    }
    catch (std::exception& e) {
        spdlog::error("Test client error: {}", e.what());
        return 1;
    }
    
    return 0;
}