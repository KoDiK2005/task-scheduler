#include "task.hpp"
#include <stdexcept>
#include <iostream>

Task::Task(const std::string& id, TaskFunction func)
    : id_(id)
    , func_(std::move(func))
    , status_(TaskStatus::PENDING)
    , execution_time_ms_(0)
{
}

void Task::add_dependency(const std::string& dep_id) {
    dependencies_.push_back(dep_id);
}

void Task::execute() {
    status_ = TaskStatus::RUNNING;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        func_();  // Выполняем функцию задачи
        status_ = TaskStatus::COMPLETED;
    } catch (const std::exception& e) {
        status_ = TaskStatus::FAILED;
        error_message_ = e.what();
        std::cerr << "Task " << id_ << " failed: " << e.what() << std::endl;
    } catch (...) {
        status_ = TaskStatus::FAILED;
        error_message_ = "Unknown error";
        std::cerr << "Task " << id_ << " failed with unknown error" << std::endl;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    execution_time_ms_ = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}
