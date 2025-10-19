/**
 * @Author: running-code-pp 3320996652@qq.com
 * @Date: 2025-10-18 16:34:22
 * @LastEditors: running-code-pp 3320996652@qq.com
 * @LastEditTime: 2025-10-18 17:47:25
 * @FilePath: \asio-learn-code\src\coroutine\coroutine_01.cpp
 * @Description: C++20协程入门示例01
 * @Copyright: Copyright (c) 2025 by ${git_name}, All Rights Reserved.
 */

// 定义协程返回值类型
#include <stdio.h>

#include <coroutine>
#include <iostream>
#include <thread>

template<typename T>
struct coro_ret
{
  struct promise_type;
  using handle_type = std::coroutine_handle<promise_type>;
  // 协程句柄
  handle_type _coro_handle;

  coro_ret(handle_type h) : _coro_handle(h)
  {
  }
  coro_ret(coro_ret&& other) : _coro_handle(other._coro_handle)
  {
    other._coro_handle = nullptr;
    return *this;
  }
  coro_ret(const coro_ret&) = delete;
  ~coro_ret()
  {
    // 销毁协程
    if (_coro_handle)
    {
      _coro_handle.destroy();
    }
  }

  // 恢复协程，返回是否结束
  bool move_next()
  {
    _coro_handle.resume();
    return _coro_handle.done();
  }

  // 获取返回值
  T get()
  {
    return _coro_handle.promise()._return_value;
  }

  // 定义承诺对象
  struct promise_type
  {
    // 返回值
    T _return_value;
    // 几个必须定义的接口
    // 生成协程函数的返回对象
    auto get_return_object()
    {
      printf("get_return_object was called\n");
      return coro_ret{ handle_type::from_promise(*this) };
    }

    // 确定协程初始化之后的行为，返回等待体
    auto initial_suspend()
    {
      printf("initial_suspend was called\n");
      return std::suspend_always{};  // 立即挂起
    }

    // co_return之后调用的行为
    void return_value(T value)
    {
      printf("return_value was called\n");
      _return_value = value;
      return;
    }

    // co_yield之后调用的行为
    auto yield_value(T value)
    {
      printf("yield_value was called\n");
      _return_value = value;
      return std::suspend_always{};  // 立即挂起
    }

    // 协程退出时调用的行为
    auto final_suspend() noexcept
    {
      printf("final_suspend was called\n");
      return std::suspend_always{};  // 协程结束时挂起
    }

    void unhandled_exception()
    {
      std::exit(1);
    }

  };
};

// 定义协程函数
coro_ret<int> simple_coroutine()
{
  printf("coroutine started\n");
  co_await std::suspend_never{};  // 不会挂起
  printf("1st suspend coroutine\n");
  co_await std::suspend_always{};  // 挂起协程
  printf("2nd suspend coroutine\n");
  co_yield 42;                    // 挂起协程并返回值42
  printf("3rd suspend coroutine\n");
  co_yield 100;                  // 返回值100并结束协程
  co_return 200;                  // 结束协程
}

int main(){
    bool done = false;
    auto coro_ret = simple_coroutine();
    printf("coroutine %s ret =%d\n", done ? "done" : "not done", coro_ret.get());
    done = coro_ret.move_next();
    printf("coroutine %s ret =%d\n", done ? "done" : "not done", coro_ret.get());
    done = coro_ret.move_next();
    printf("coroutine %s ret =%d\n", done ? "done" : "not done", coro_ret.get());
    done = coro_ret.move_next();
    printf("coroutine %s ret =%d\n", done ? "done" : "not done", coro_ret.get());
    done = coro_ret.move_next();
    printf("coroutine %s ret =%d\n", done ? "done" : "not done", coro_ret.get());

    return 0;
}
/*
运行结果：
get_return_object was called
initial_suspend was called
coroutine not done ret =0
coroutine started
1st suspend coroutine
coroutine not done ret =0
2nd suspend coroutine
yield_value was called
coroutine not done ret =42
3rd suspend coroutine
yield_value was called
coroutine not done ret =100
return_value was called
final_suspend was called
coroutine done ret =200
*/