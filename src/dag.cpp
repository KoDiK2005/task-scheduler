#include "dag.hpp"
#include <algorithm>
#include <sstream>

void DAG::add_task(std::shared_ptr<Task> task) {
    if (!task) {
        throw std::invalid_argument("Cannot add null task to DAG");
    }
    
    const std::string& id = task->get_id();
    
    if (tasks_.find(id) != tasks_.end()) {
        throw std::runtime_error("Task with id '" + id + "' already exists in DAG");
    }
    
    tasks_[id] = task;
}

std::shared_ptr<Task> DAG::get_task(const std::string& id) const {
    auto it = tasks_.find(id);
    return (it != tasks_.end()) ? it->second : nullptr;
}

void DAG::validate() const {
    // Проверяем что все зависимости существуют
    for (const auto& [id, task] : tasks_) {
        for (const auto& dep_id : task->get_dependencies()) {
            if (tasks_.find(dep_id) == tasks_.end()) {
                throw std::runtime_error("Task '" + id + "' depends on non-existent task '" + dep_id + "'");
            }
        }
    }
    
    // Проверяем на циклы
    std::set<std::string> visited;
    std::set<std::string> rec_stack;
    
    for (const auto& [id, task] : tasks_) {
        if (visited.find(id) == visited.end()) {
            if (has_cycle_util(id, visited, rec_stack)) {
                throw std::runtime_error("Cyclic dependency detected in DAG");
            }
        }
    }
}

bool DAG::has_cycle_util(const std::string& task_id,
                         std::set<std::string>& visited,
                         std::set<std::string>& rec_stack) const {
    visited.insert(task_id);
    rec_stack.insert(task_id);
    
    auto task = get_task(task_id);
    if (!task) return false;
    
    // Проверяем все зависимости
    for (const auto& dep_id : task->get_dependencies()) {
        // Если зависимость не посещена - рекурсивно проверяем её
        if (visited.find(dep_id) == visited.end()) {
            if (has_cycle_util(dep_id, visited, rec_stack)) {
                return true;
            }
        }
        // Если зависимость в текущем стеке рекурсии - цикл обнаружен
        else if (rec_stack.find(dep_id) != rec_stack.end()) {
            return true;
        }
    }
    
    rec_stack.erase(task_id);
    return false;
}

std::vector<std::shared_ptr<Task>> DAG::topological_sort() const {
    validate();  // Сначала проверяем валидность
    
    std::set<std::string> visited;
    std::vector<std::shared_ptr<Task>> result;
    
    // DFS для всех непосещённых вершин
    for (const auto& [id, task] : tasks_) {
        if (visited.find(id) == visited.end()) {
            topological_sort_util(id, visited, result);
        }
    }
    
    // Результат в обратном порядке (т.к. DFS добавляет в конец)
    std::reverse(result.begin(), result.end());
    return result;
}

void DAG::topological_sort_util(const std::string& task_id,
                                std::set<std::string>& visited,
                                std::vector<std::shared_ptr<Task>>& stack) const {
    visited.insert(task_id);
    
    auto task = get_task(task_id);
    if (!task) return;
    
    // Рекурсивно обрабатываем все зависимости
    for (const auto& dep_id : task->get_dependencies()) {
        if (visited.find(dep_id) == visited.end()) {
            topological_sort_util(dep_id, visited, stack);
        }
    }
    
    // Добавляем текущую задачу в стек после обработки зависимостей
    stack.push_back(task);
}

std::vector<std::shared_ptr<Task>> DAG::get_ready_tasks() const {
    std::vector<std::shared_ptr<Task>> ready;
    
    for (const auto& [id, task] : tasks_) {
        // Пропускаем уже выполненные или выполняющиеся задачи
        if (task->get_status() != TaskStatus::PENDING) {
            continue;
        }
        
        // Проверяем все зависимости
        bool all_deps_completed = true;
        for (const auto& dep_id : task->get_dependencies()) {
            auto dep_task = get_task(dep_id);
            if (!dep_task || dep_task->get_status() != TaskStatus::COMPLETED) {
                all_deps_completed = false;
                break;
            }
        }
        
        if (all_deps_completed) {
            ready.push_back(task);
        }
    }
    
    return ready;
}
