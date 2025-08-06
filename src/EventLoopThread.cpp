#include <condition_variable>
#include <mutex>

#include "EventLoopThread.h"
#include "EventLoop.h"

EventLoopThread::EventLoopThread(const ThreadInitCallback &cb, const std::string &name)
    : loop_(nullptr)
    , exiting_(false)
    , thread_(std::bind(&EventLoopThread::threadFunc, this), name)
    , mutex_()
    , cond_()
    , callback_(cb)
{    
}


EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    if (loop_ != nullptr) {
        loop_->quit();
        thread_.join();     // 防止主线程提前退出，丢失子进程
    }
}


EventLoop *EventLoopThread::startLoop()
{
    thread_.start();        // threadFunc()

    EventLoop *loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (loop_==nullptr) {
            cond_.wait(lock);
        }
        loop = loop_;
    }
    return loop;
}


void EventLoopThread::threadFunc()
{
    EventLoop loop;

    if (callback_) {
        callback_(&loop);
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }
    loop.loop();        // subloop
    std::unique_lock<std::mutex> lock(mutex_);      // quit
    loop_ = nullptr;
}
