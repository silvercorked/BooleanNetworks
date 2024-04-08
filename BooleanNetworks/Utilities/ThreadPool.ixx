module;

export module ThreadPool;

import PrimitiveTypes;

import <condition_variable>;
import <functional>;
import <mutex>;
import <thread>;
import <queue>;
import <vector>;

export class ThreadPool {
    const u32 numberOfThreads;
    bool shouldTerminate;
    std::mutex queueMutex;
    std::condition_variable mutexCondition;
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;

public:
    ThreadPool(const uint32_t);
    void queueTask(const std::function<void()>&);
    auto unassignedTasks();
    bool busy();
    ~ThreadPool();

private:
    void threadLoop();
};
