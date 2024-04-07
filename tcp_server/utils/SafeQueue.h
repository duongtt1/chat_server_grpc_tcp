#ifndef SAFEQUEUE_H
#define SAFEQUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <stdexcept>

template <typename T> class SafeQueue {
public:
    explicit SafeQueue(size_t maxSize) : maxSize(maxSize) {}

    void push(T value) {
        std::unique_lock<std::mutex> lock(queueMutex);
        if (dataQueue.size() >= maxSize) {
            dataQueue.pop();
        }
        dataQueue.push(std::move(value));
        lock.unlock();
        queueCondVar.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(queueMutex);
        queueCondVar.wait(lock, [this] { return !dataQueue.empty(); });

        T value = std::move(dataQueue.front());
        dataQueue.pop();

        return value;
    }

    bool empty() const {
        std::unique_lock<std::mutex> lock(queueMutex);
        return dataQueue.empty();
    }

    bool isEmpty() const {
        std::unique_lock<std::mutex> lock(queueMutex);
        return (dataQueue.size() == 0);
    }

    bool isFull() const {
        std::unique_lock<std::mutex> lock(queueMutex);
        return ( dataQueue.size() == maxSize );
    }

    size_t size() const {
        std::unique_lock<std::mutex> lock(queueMutex);
        return dataQueue.size();
    }

private:
    std::queue<T>           dataQueue;
    mutable std::mutex      queueMutex;
    std::condition_variable queueCondVar;
    size_t                  maxSize;
};

#endif // SAFEQUEUE_H