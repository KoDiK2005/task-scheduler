#include "scheduler.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>

Scheduler::Scheduler(size_t num_threads)
    : pool_(std::make_unique<ThreadPool>(num_threads))
{
}

void Scheduler::add_task(const std::string& id, std::function<void()> func) {
    add_task(id, {}, std::move(func));
}

void Scheduler::add_task(const std::string& id,
                        const std::vector<std::string>& dependencies,
                        std::function<void()> func) {
    auto task = std::make_shared<Task>(id, std::move(func));
    
    // Добавляем зависимости
    for (const auto& dep_id : dependencies) {
        task->add_dependency(dep_id);
    }
    
    dag_.add_task(task);
}

void Scheduler::execute() {
    // Валидация графа (проверка циклов и существования зависимостей)
    dag_.validate();
    
    std::cout << "Starting execution of " << dag_.size() << " tasks..." << std::endl;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Продолжаем пока есть незавершённые задачи
    size_t completed_count = 0;
    size_t failed_count = 0;
    
    while (completed_count + failed_count < dag_.size()) {
        // Получаем задачи, готовые к выполнению
        auto ready_tasks = dag_.get_ready_tasks();
        
        if (ready_tasks.empty()) {
            // Если нет готовых задач, но есть незавершённые - ждём
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
        // Запускаем готовые задачи в пуле потоков
        for (auto& task : ready_tasks) {
            task->set_status(TaskStatus::RUNNING);
            
            // Захватываем shared_ptr в лямбде
            pool_->enqueue([task]() {
                std::cout << "  [START] Task: " << task->get_id() << std::endl;
                task->execute();
                
                if (task->get_status() == TaskStatus::COMPLETED) {
                    std::cout << "  [DONE]  Task: " << task->get_id() 
                             << " (" << task->get_execution_time_ms() << "ms)" << std::endl;
                } else {
                    std::cout << "  [FAIL]  Task: " << task->get_id() 
                             << " - " << task->get_error_message() << std::endl;
                }
            });
        }
        
        // Ждём завершения текущей волны задач
        pool_->wait_all();
        
        // Подсчитываем завершённые и проваленные задачи
        completed_count = 0;
        failed_count = 0;
        
        auto sorted_tasks = dag_.topological_sort();
        for (const auto& task : sorted_tasks) {
            if (task->get_status() == TaskStatus::COMPLETED) {
                ++completed_count;
            } else if (task->get_status() == TaskStatus::FAILED) {
                ++failed_count;
            }
        }
        
        // Если есть проваленные задачи - прерываем выполнение
        if (failed_count > 0) {
            std::cerr << "\nExecution stopped due to task failures!" << std::endl;
            break;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    std::cout << "\nExecution completed in " << total_time << "ms" << std::endl;
    std::cout << "Tasks completed: " << completed_count << "/" << dag_.size() << std::endl;
    
    if (failed_count > 0) {
        std::cout << "Tasks failed: " << failed_count << std::endl;
    }
}

std::string Scheduler::get_statistics() const {
    std::ostringstream oss;
    
    auto sorted_tasks = dag_.topological_sort();
    
    oss << "\n=== Task Execution Statistics ===\n\n";
    oss << std::left << std::setw(20) << "Task ID" 
        << std::setw(12) << "Status"
        << std::setw(15) << "Time (ms)"
        << "Dependencies\n";
    oss << std::string(70, '-') << "\n";
    
    long long total_time = 0;
    size_t completed = 0;
    size_t failed = 0;
    
    for (const auto& task : sorted_tasks) {
        oss << std::left << std::setw(20) << task->get_id();
        
        // Статус
        std::string status_str;
        switch (task->get_status()) {
            case TaskStatus::PENDING:   status_str = "PENDING"; break;
            case TaskStatus::RUNNING:   status_str = "RUNNING"; break;
            case TaskStatus::COMPLETED: status_str = "COMPLETED"; ++completed; break;
            case TaskStatus::FAILED:    status_str = "FAILED"; ++failed; break;
        }
        oss << std::setw(12) << status_str;
        
        // Время выполнения
        oss << std::setw(15) << task->get_execution_time_ms();
        
        // Зависимости
        const auto& deps = task->get_dependencies();
        if (deps.empty()) {
            oss << "-";
        } else {
            for (size_t i = 0; i < deps.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << deps[i];
            }
        }
        
        oss << "\n";
        
        if (task->get_status() == TaskStatus::COMPLETED) {
            total_time += task->get_execution_time_ms();
        }
        
        // Если задача провалилась - показываем ошибку
        if (task->get_status() == TaskStatus::FAILED) {
            oss << "    Error: " << task->get_error_message() << "\n";
        }
    }
    
    oss << std::string(70, '-') << "\n";
    oss << "\nSummary:\n";
    oss << "  Total tasks: " << sorted_tasks.size() << "\n";
    oss << "  Completed: " << completed << "\n";
    oss << "  Failed: " << failed << "\n";
    oss << "  Total execution time: " << total_time << "ms\n";
    
    return oss.str();
}

void Scheduler::clear() {
    dag_.clear();
}
