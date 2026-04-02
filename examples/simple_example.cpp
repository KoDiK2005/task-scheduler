#include "scheduler.hpp"
#include <iostream>

/**
 * Простой пример использования Task Scheduler
 */

int main() {
    std::cout << "=== Simple Task Scheduler Example ===" << std::endl;
    
    // Создаём планировщик
    Scheduler scheduler;
    
    // Добавляем простые задачи
    scheduler.add_task("hello", []() {
        std::cout << "Hello from task!" << std::endl;
    });
    
    scheduler.add_task("world", {"hello"}, []() {
        std::cout << "World follows hello!" << std::endl;
    });
    
    // Выполняем
    scheduler.execute();
    
    return 0;
}
