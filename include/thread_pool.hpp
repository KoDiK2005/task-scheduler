#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

/**
 * @brief Пул потоков для параллельного выполнения задач
 */
class ThreadPool {
public:
    using Task = std::function<void()>;
    
    /**
     * @brief Конструктор пула потоков
     * @param num_threads Количество рабочих потоков (по умолчанию = число ядер CPU)
     */
    explicit ThreadPool(size_t num_threads = std::thread::hardware_concurrency());
    
    /**
     * @brief Деструктор - останавливает все потоки
     */
    ~ThreadPool();
    
    /**
     * @brief Добавить задачу в очередь
     * @param task Функция для выполнения
     */
    void enqueue(Task task);
    
    /**
     * @brief Дождаться завершения всех задач
     */
    void wait_all();
    
    /**
     * @brief Получить количество активных потоков
     */
    size_t get_thread_count() const { return workers_.size(); }
    
    // Запрет копирования и присваивания
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

private:
    /**
     * @brief Рабочая функция потока
     */
    void worker_thread();
    
    std::vector<std::thread> workers_;          // Рабочие потоки
    std::queue<Task> tasks_;                    // Очередь задач
    
    std::mutex queue_mutex_;                    // Мьютекс для защиты очереди
    std::condition_variable condition_;         // Условная переменная для ожидания задач
    std::condition_variable wait_condition_;    // Условная переменная для wait_all
    
    std::atomic<bool> stop_;                    // Флаг остановки
    std::atomic<size_t> active_tasks_;          // Количество активных задач
};

#endif // THREAD_POOL_HPP
