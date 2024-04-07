#ifndef CACHE_THREAD_H
#define CACHE_THREAD_H

#include <iostream>
#include <memory>
#include <atomic>
#include <thread>
#include "utils/SafeQueue.h"
#include "protocols/interfaces/RequestBase.h"

/**
 * @file CacheThread.h
 * @brief Class for managing a thread that handles a cache of received frames.
 */

/**
 * @class CacheThread
 * @brief Provides functionality to start and stop a thread that manages a cache of received frames.
 */
class CacheThread {
private:
    std::atomic<bool> runThread; /**< Atomic flag to control the execution of the thread. */
    std::thread workerThread; /**< Thread responsible for handling the cache. */
    SafeQueue<RequestBase*> requestsQueue;

public:
    /**
     * @brief Constructor for the CacheThread class.
     */
    CacheThread();

    /**
     * @brief Destructor for the CacheThread class.
     */
    ~CacheThread();

    /**
     * @brief Start the cache thread.
     * 
     * Initiates the thread that manages the cache.
     */
    void start();

    /**
     * @brief Stop the cache thread.
     * 
     * Stops the execution of the cache thread.
     */
    void stop();

     /**
     * @brief add request cache thread.
     *
     */
    void addRequest(RequestBase* req);

private:
    /**
     * @brief process function for handling the cache.
     * 
     * This function is called when the thread is started and is responsible for managing the received frame cache.
     */
    void process();
};

#endif // CACHE_THREAD_H

