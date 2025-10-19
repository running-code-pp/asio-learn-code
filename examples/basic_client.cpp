#include <iostream>
#include <asio.hpp>
#include <spdlog/spdlog.h>

using asio::ip::tcp;

int main() {
    spdlog::info("Basic TCP Client Example");
    
    try {
        asio::io_context io_context;
        
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve("localhost", "8080");
        
        tcp::socket socket(io_context);
        asio::connect(socket, endpoints);
        
        std::string message = "Hello from basic client!";
        asio::write(socket, asio::buffer(message));
        
        char reply[1024];
        size_t reply_length = asio::read(socket, asio::buffer(reply, message.length()));
        
        spdlog::info("Server reply: {}", std::string(reply, reply_length));
    }
    catch (std::exception& e) {
        spdlog::error("Client error: {}", e.what());
        return 1;
    }
    
    return 0;
}