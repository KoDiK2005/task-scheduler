#include "thread_pool.hpp"

ThreadPool::ThreadPool(size_t num_threads)
    : stop_(false)
    , active_tasks_(0)
{
    // Создаём рабочие потоки
    for (size_t i = 0; i < num_threads; ++i) {
        workers_.emplace_back(&ThreadPool::worker_thread, this);
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }
    
    // Будим все потоки
    condition_.notify_all();
    
    // Ждём завершения всех потоков
    for (std::thread& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::enqueue(Task task) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        tasks_.push(std::move(task));
    }
    condition_.notify_one();  // Будим один поток для выполнения задачи
}

void ThreadPool::wait_all() {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    wait_condition_.wait(lock, [this]() {
        return tasks_.empty() && active_tasks_ == 0;
    });
}

void ThreadPool::worker_thread() {
    while (true) {
        Task task;
        
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            
            // Ждём пока появится задача или придёт сигнал остановки
            condition_.wait(lock, [this]() {
                return stop_ || !tasks_.empty();
            });
            
            // Если остановка и нет задач - выходим
            if (stop_ && tasks_.empty()) {
                return;
            }
            
            // Берём задачу из очереди
            if (!tasks_.empty()) {
                task = std::move(tasks_.front());
                tasks_.pop();
                ++active_tasks_;
            }
        }
        
        // Выполняем задачу (вне блокировки!)
        if (task) {
            task();
            --active_tasks_;
            wait_condition_.notify_all();  // Оповещаем wait_all()
        }
    }
}
