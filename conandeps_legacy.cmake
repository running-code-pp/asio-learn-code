message(STATUS "Conan: Using CMakeDeps conandeps_legacy.cmake aggregator via include()")
message(STATUS "Conan: It is recommended to use explicit find_package() per dependency instead")

find_package(nlohmann_json)
find_package(spdlog)
find_package(GTest)
find_package(asio)
find_package(yaml-cpp)
find_package(OpenSSL)

set(CONANDEPS_LEGACY  nlohmann_json::nlohmann_json  spdlog::spdlog  gtest::gtest  asio::asio  yaml-cpp::yaml-cpp  openssl::openssl )