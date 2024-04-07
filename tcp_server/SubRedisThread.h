#ifndef SUB_REDISTHREAD_H
#define SUB_REDISTHREAD_H

#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <hiredis/hiredis.h>
#include <mutex>
#include <atomic>
#include <stdexcept>
#include <algorithm>
#include <iostream> // for std::cerr

#include "ChatStreamImpl.h"

class SubRedisThread {
private:
    std::atomic<bool> isRunning;
    std::string ipAddress;
    int port;
    std::unique_ptr<ChatStreamImpl> chatStreamPtr;
    std::string channel;
    std::thread subscriberThread;
    redisContext* redisClient;
    static constexpr std::chrono::seconds retryDelay{1};
    std::mutex redisMutex; // Mutex for thread safety

public:
    SubRedisThread(const std::string& ip, int port, std::unique_ptr<ChatStreamImpl> ptr, const std::string& channel);
    ~SubRedisThread();
    void stop();

private:
    void connectToRedis();
    void runSubscriber();
};

#endif // SUB_REDISTHREAD_H