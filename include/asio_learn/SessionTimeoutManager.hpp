/** 
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-13 15:39:47
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-13 15:40:30
 * @FilePath: \asio-learn-code\include\asio_learn\SessionTimeoutManager.hpp
 * @Description: 带有超时机制的会话管理器，自动断开长时间不活跃的连接，节省系统资源
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */
#ifndef ASIO_LEARN_SESSION_TIMEOUT_MANAGER_HPP
#define ASIO_LEARN_SESSION_TIMEOUT_MANAGER_HPP


#include <array>
#include <memory>
#include <vector>
#include <chrono>
#include "asio_learn/public.hpp"
#include "common/Log.hpp"
#include "common/uuid.hpp"
/**
 * 合理设置超时间: 太短会导致频繁断开连接，影响用户体验；太长则可能浪费资源。
 * 区分不同类型的会话: 可以区分优先级，设置不同的超时策略
 * 优雅关闭连接： 断开连接之前，通知一下客户端
 * 资源清理： 确保在断开连接后，释放相关资源，防止内存泄漏
 */

namespace asio_learn{

    class SessionTimeoutManager{
        public:
        using Duration = std::chrono::steady_clock::duration; //超时间隔类型
        using Clock = std::chrono::steady_clock; //时钟类型
        using SessionUid  = common::uuid;
        using TimeoutHandler = std::function<void(const SessionUid& session_uuid)>; //超时处理方法
        SessionTimeoutManager(){}
        ~SessionTimeoutManager(){}

        private:
        io_context _ioc;
        Duration _duration; // 超时间隔
        tcp_acceptor _acceptor{_ioc};
        std::unordered_map<SessionUid,std::shared_ptr<tcp_socket
    }

    
} // namespace asio_learn


#endif // ASIO_LEARN_SESSION_TIMEOUT_MANAGER_HPP