#include "CacheThread.h"

// Constructor definition
CacheThread::CacheThread() : runThread(false), requestsQueue(100) {}

// Destructor definition
CacheThread::~CacheThread() {
    stop();
}

// Start the thread
void CacheThread::start() {
    if (!runThread.load()) {
        runThread.store(true);
        workerThread = std::thread(&CacheThread::process, this);
    }
}

// Stop the thread
void CacheThread::stop() {
    if (runThread.load()) {
        runThread.store(false);
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }
}

void  CacheThread::addRequest(RequestBase *req)
{
    if (req != nullptr && !requestsQueue.isFull() )
        requestsQueue.push(req);
}



// Thread function
void CacheThread::process() {
    while (runThread.load() || !requestsQueue.empty()) {
        auto req = requestsQueue.pop();
        if ( req != nullptr ) {
            req->callback(req->socket);
            delete req;
        } else {
            std::this_thread::yield();
        }
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}