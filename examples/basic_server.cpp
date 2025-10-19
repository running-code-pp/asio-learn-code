#include <iostream>
#include <asio.hpp>
#include <spdlog/spdlog.h>

using asio::ip::tcp;

class BasicServer {
public:
    BasicServer(asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        start_accept();
    }

private:
    void start_accept() {
        auto new_session = std::make_shared<tcp::socket>(acceptor_.get_executor());
        
        acceptor_.async_accept(*new_session,
            [this, new_session](std::error_code ec) {
                if (!ec) {
                    handle_client(new_session);
                }
                start_accept();
            });
    }
    
    void handle_client(std::shared_ptr<tcp::socket> socket) {
        auto buffer = std::make_shared<std::array<char, 1024>>();
        
        socket->async_read_some(asio::buffer(*buffer),
            [socket, buffer](std::error_code ec, std::size_t length) {
                if (!ec) {
                    std::string message(buffer->data(), length);
                    spdlog::info("Received: {}", message);
                    
                    // Echo back
                    asio::async_write(*socket, asio::buffer(*buffer, length),
                        [socket](std::error_code /*ec*/, std::size_t /*length*/) {
                            socket->close();
                        });
                }
            });
    }
    
    tcp::acceptor acceptor_;
};

int main() {
    spdlog::info("Basic TCP Server Example");
    
    try {
        asio::io_context io_context;
        BasicServer server(io_context, 8080);
        
        spdlog::info("Server listening on port 8080");
        io_context.run();
    }
    catch (std::exception& e) {
        spdlog::error("Server error: {}", e.what());
        return 1;
    }
    
    return 0;
}