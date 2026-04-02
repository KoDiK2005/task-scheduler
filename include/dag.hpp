#ifndef DAG_HPP
#define DAG_HPP

#include "task.hpp"
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <stdexcept>

/**
 * @brief Класс для представления направленного ациклического графа (DAG) задач
 */
class DAG {
public:
    /**
     * @brief Добавить задачу в граф
     * @param task Указатель на задачу
     */
    void add_task(std::shared_ptr<Task> task);
    
    /**
     * @brief Получить задачу по ID
     * @param id Идентификатор задачи
     * @return Указатель на задачу или nullptr если не найдена
     */
    std::shared_ptr<Task> get_task(const std::string& id) const;
    
    /**
     * @brief Проверить граф на наличие циклических зависимостей
     * @throws std::runtime_error если обнаружен цикл
     */
    void validate() const;
    
    /**
     * @brief Получить топологическую сортировку задач
     * @return Вектор задач в порядке выполнения
     * @throws std::runtime_error если граф содержит циклы
     */
    std::vector<std::shared_ptr<Task>> topological_sort() const;
    
    /**
     * @brief Получить все задачи, готовые к выполнению (без незавершённых зависимостей)
     * @return Вектор задач, готовых к запуску
     */
    std::vector<std::shared_ptr<Task>> get_ready_tasks() const;
    
    /**
     * @brief Получить общее количество задач
     */
    size_t size() const { return tasks_.size(); }
    
    /**
     * @brief Очистить граф
     */
    void clear() { tasks_.clear(); }

private:
    /**
     * @brief Рекурсивная DFS для обнаружения циклов
     */
    bool has_cycle_util(const std::string& task_id, 
                       std::set<std::string>& visited,
                       std::set<std::string>& rec_stack) const;
    
    /**
     * @brief Рекурсивная DFS для топологической сортировки
     */
    void topological_sort_util(const std::string& task_id,
                               std::set<std::string>& visited,
                               std::vector<std::shared_ptr<Task>>& stack) const;
    
    std::map<std::string, std::shared_ptr<Task>> tasks_;  // Хранилище задач
};

#endif // DAG_HPP
