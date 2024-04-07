#include <iostream>
#include <thread>
#include <hiredis/hiredis.h>
#include <string.h>

void publishThread() {
    redisContext* context = redisConnect("127.0.0.1", 6379);
    if (context == nullptr || context->err) {
        if (context) {
            std::cerr << "Connection error: " << context->errstr << std::endl;
            redisFree(context);
        } else {
            std::cerr << "Connection error: Can't allocate redis context" << std::endl;
        }
        return;
    }

    const char* channel = "your_channel_name";
    
    int i = 0;
    while (true) {
        std::string message = "Message " + std::to_string(i++);
        
        redisReply* reply = static_cast<redisReply*>(redisCommand(context, "PUBLISH %s %s", channel, message.c_str()));
        if (reply == nullptr || context->err) {
            std::cerr << "Publish error: " << context->errstr << std::endl;
            if (reply) {
                freeReplyObject(reply);
            }
            redisFree(context);
            return;
        }
        
        freeReplyObject(reply);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    redisFree(context);
}

void subscribeThread() {
    redisContext* context = redisConnect("127.0.0.1", 6379);
    if (context == nullptr || context->err) {
        if (context) {
            std::cerr << "Connection error: " << context->errstr << std::endl;
            redisFree(context);
        } else {
            std::cerr << "Connection error: Can't allocate redis context" << std::endl;
        }
        return;
    }

    const char* channel = "your_channel_name";

    redisReply* reply = static_cast<redisReply*>(redisCommand(context, "SUBSCRIBE %s", channel));
    if (reply == nullptr || context->err) {
        std::cerr << "Subscription error: " << context->errstr << std::endl;
        if (reply) {
            freeReplyObject(reply);
        }
        redisFree(context);
        return;
    }

    freeReplyObject(reply);

    while (true) {
        redisReply* message;
        if (redisGetReply(context, reinterpret_cast<void**>(&message)) == REDIS_OK) {
            if (message != nullptr && message->type == REDIS_REPLY_ARRAY && message->elements == 3) {
                if (strcmp(message->element[0]->str, "message") == 0) {
                    std::cout << "Received message: " << message->element[2]->str << std::endl;
                }
            }
            freeReplyObject(message);
        }
    }

    redisFree(context);
}

int main() {
    std::thread publisher(publishThread);
    std::thread subscriber(subscribeThread);

    publisher.join();
    subscriber.join();

    return 0;
}
