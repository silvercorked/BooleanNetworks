
module ThreadPool;

ThreadPool::ThreadPool(const u32 numbOfthreads = std::thread::hardware_concurrency())
    : numberOfThreads(numbOfthreads != 0 ? numbOfthreads : 4) // default 4 if std...hardware_concurrency gives a zero for some reason
    , shouldTerminate(false)
    , threads(std::vector<std::thread>())
    , tasks(std::queue<std::function<void()>>())
{
    threads.reserve(this->numberOfThreads);
    for (uint32_t i = 0; i < this->numberOfThreads; i++) {
        threads.push_back(
            std::thread(
                [this]() { this->threadLoop(); }
            )
        );
    }
}
ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(this->queueMutex);
        this->shouldTerminate = true;
    }
    this->mutexCondition.notify_all();
    for (std::thread& activeThread : this->threads) {
        activeThread.join();
    }
}

void ThreadPool::queueTask(const std::function<void()>& task) {
    {
        std::unique_lock<std::mutex> lock(this->queueMutex);
        this->tasks.push(task);
    }
    this->mutexCondition.notify_one();
}
auto ThreadPool::unassignedTasks() {
    std::queue<std::function<void()>>::size_type len;
    {
        std::unique_lock<std::mutex> lock(this->queueMutex);
        len = this->tasks.size();
    }
    return len;

}
bool ThreadPool::busy() {
    bool poolBusy;
    {
        std::unique_lock<std::mutex> lock(this->queueMutex);
        poolBusy = !this->tasks.empty();
    }
    return poolBusy;
}

void ThreadPool::threadLoop() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(this->queueMutex);
            this->mutexCondition.wait(lock, [this] {
                return !this->tasks.empty() || this->shouldTerminate;
            });
            if (this->shouldTerminate) return;
            task = this->tasks.front();
            this->tasks.pop();
        }
        task();
    }
}
