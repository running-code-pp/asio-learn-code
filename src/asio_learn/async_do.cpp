/** 
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-12 01:10:05
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-12 15:08:00
 * @FilePath: \asio-learn-code\src\asio_learn\async_do.cpp
 * @Description: 
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */


#include <asio.hpp>
#include <iostream>
#include <string>

#include "common/Log.hpp"

template<asio::completion_token_for<void(std::string)> CompletionToken>
auto async_query_value(asio::thread_pool& pool, asio::io_context& ioc, std::string key, CompletionToken&& token)
{
    return asio::async_initiate<CompletionToken, void(std::string)>(
        [&](auto completion_handler, std::string const key) {
            auto io_eq = asio::prefer(ioc.get_executor(), asio::execution::outstanding_work.tracked);
            asio::post(pool, [key, io_eq = std::move(io_eq), completion_handler = std::move(completion_handler)]() mutable {
                std::string value = std::string("value for ") + key;
                asio::post(io_eq, [value = std::move(value), completion_handler = std::move(completion_handler)]() mutable {
                    completion_handler(value);
                });
            });
        },
        token, key
    );
}

asio::awaitable<void> test_coroutine(asio::thread_pool&pool,asio::io_context&ioc){
    std::string key="01";
    std::string val = co_await async_query_value(pool,ioc,key,asio::use_awaitable);
    LOG_INFO("got value for key{}:{}",key,val);
}

int main()
{
    asio::thread_pool tp;
    asio::io_context ioc;
    asio::co_spawn(ioc,test_coroutine(tp,ioc),asio::detached);
    ioc.run();
    tp.join();
  return 0;
}