#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "task.hpp"
#include "dag.hpp"
#include "thread_pool.hpp"
#include <memory>
#include <vector>
#include <string>

/**
 * @brief Планировщик задач с автоматической параллелизацией
 */
class Scheduler {
public:
    /**
     * @brief Конструктор планировщика
     * @param num_threads Количество рабочих потоков (по умолчанию = число ядер)
     */
    explicit Scheduler(size_t num_threads = std::thread::hardware_concurrency());
    
    /**
     * @brief Добавить задачу без зависимостей
     * @param id Уникальный идентификатор задачи
     * @param func Функция для выполнения
     */
    void add_task(const std::string& id, std::function<void()> func);
    
    /**
     * @brief Добавить задачу с зависимостями
     * @param id Уникальный идентификатор задачи
     * @param dependencies Список ID задач, от которых зависит эта задача
     * @param func Функция для выполнения
     */
    void add_task(const std::string& id, 
                  const std::vector<std::string>& dependencies,
                  std::function<void()> func);
    
    /**
     * @brief Выполнить все задачи
     * @throws std::runtime_error если граф содержит циклы или невалиден
     */
    void execute();
    
    /**
     * @brief Получить статистику выполнения
     * @return Строка с детальной статистикой
     */
    std::string get_statistics() const;
    
    /**
     * @brief Очистить все задачи
     */
    void clear();
    
    /**
     * @brief Получить количество задач
     */
    size_t task_count() const { return dag_.size(); }

private:
    DAG dag_;                           // Граф зависимостей задач
    std::unique_ptr<ThreadPool> pool_;  // Пул потоков
    
    /**
     * @brief Выполнить одну задачу
     */
    void execute_task(std::shared_ptr<Task> task);
};

#endif // SCHEDULER_HPP
