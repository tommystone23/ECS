#include "threadpool.h"

ThreadPool::ThreadPool()
    : _should_terminate(false)
{
}

void ThreadPool::start()
{
    // Add threads to vector of threads
    const uint32_t num_threads = std::thread::hardware_concurrency(); // Max # of threads the system supports
    for (uint32_t i = 0; i < num_threads; ++i) {
        _threads.emplace_back(std::thread(&ThreadPool::thread_loop, this));
    }
}

void ThreadPool::queue_job(const std::function<void(float)> &job, float delta_time)
{
    {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        _jobs.push(std::pair(job, delta_time));
    }

    _mutex_condition.notify_one();
}

void ThreadPool::stop()
{
    {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        _should_terminate = true;
    }
    _mutex_condition.notify_all();

    for (std::thread& active_thread : _threads) {
        active_thread.join();
    }

    _threads.clear();
}

bool ThreadPool::is_busy()
{
    bool pool_busy;
    {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        pool_busy = !_jobs.empty();
    }

    return pool_busy;
}

void ThreadPool::thread_loop()
{
    while(true) {
        std::pair<std::function<void(float)>, float> job;
        {
            std::unique_lock<std::mutex> lock(_queue_mutex);
            _mutex_condition.wait(lock, [this] {
                return !_jobs.empty() || _should_terminate;
            });

            if (_should_terminate) {
                return;
            }

            job = _jobs.front();
            _jobs.pop();
        }

        float delta_time = job.second;
        std::function<void(float)> job_function = job.first;
        job_function(delta_time);
    }
}
