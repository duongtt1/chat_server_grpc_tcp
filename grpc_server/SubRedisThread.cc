#include "SubRedisThread.h"

constexpr std::chrono::seconds SubRedisThread::retryDelay;

SubRedisThread::SubRedisThread(const std::string& ip, int port, std::unique_ptr<ChatStreamImpl> ptr, const std::string& channel)
: ipAddress(ip), port(port), chatStreamPtr(std::move(ptr)), channel(channel), redisClient(nullptr), isRunning(false)
{
    connectToRedis();
    redisCommand(redisClient, "SUBSCRIBE %s", channel.c_str());
    isRunning.store(true);
    subscriberThread = std::thread(&SubRedisThread::runSubscriber, this);
}

SubRedisThread::~SubRedisThread() {
    stop();
}

void SubRedisThread::stop() {
    if (isRunning.exchange(false)) {
        if (subscriberThread.joinable()) {
            subscriberThread.join();
        }
        if (redisClient) {
            redisFree(redisClient);
            redisClient = nullptr;
        }
    }
}

void SubRedisThread::connectToRedis() {
    std::lock_guard<std::mutex> lock(redisMutex);
    redisClient = redisConnect(ipAddress.c_str(), port);
    if (!redisClient || redisClient->err) {
        throw std::runtime_error("Failed to connect to Redis server: " + std::string(redisClient ? redisClient->errstr : "Can't allocate redis context"));
    }
}

void SubRedisThread::runSubscriber() {
    try {
        while (isRunning.load()) {
            redisReply* reply;
            {
                std::lock_guard<std::mutex> lock(redisMutex);
                if (redisGetReply(redisClient, reinterpret_cast<void**>(&reply)) != REDIS_OK) {
                    std::cerr << "Redis error: " << (redisClient ? redisClient->errstr : "Unknown error") << std::endl; 
                    std::this_thread::sleep_for(retryDelay);
                    continue;
                }
            }

            if (reply->type == REDIS_REPLY_ARRAY && reply->elements == 3) {
                ServerMessage message;
                message.ParseFromString(reply->element[2]->str);
                // Assuming chatStreamPtr is thread-safe
                chatStreamPtr->sendMsgToRoom(message);
                message.Clear();
            }
            freeReplyObject(reply);
        }
    } catch(const std::exception& e) {
        std::cerr << "Subscriber thread error: " << e.what() << std::endl;
    }
}