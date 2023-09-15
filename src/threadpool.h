#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <functional>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool(){}

    void start();
    void queue_job(const std::function<void(float)> &job, float delta_time);
    void stop();
    bool is_busy();

private:
    void thread_loop();

    bool _should_terminate; // Tells threads to stop looking for jobs
    std::mutex _queue_mutex; // Prevents data races to the job queue
    std::condition_variable _mutex_condition; // Allows threads to wait on new jobs or termination 
    std::vector<std::thread> _threads;
    std::queue<std::pair<std::function<void(float)>, float>> _jobs;
};

#endif // THREADPOOL_H