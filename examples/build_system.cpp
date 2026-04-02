#include "scheduler.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

/**
 * Пример использования Task Scheduler:
 * Симуляция системы сборки проекта с зависимостями между модулями
 */

// Вспомогательная функция для симуляции работы
void simulate_work(const std::string& task_name, int min_ms, int max_ms) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min_ms, max_ms);
    
    int duration = dis(gen);
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

int main() {
    std::cout << "=== Task Scheduler Example: Build System ===" << std::endl;
    std::cout << "Simulating parallel compilation of a project\n" << std::endl;
    
    // Создаём планировщик с 4 потоками
    Scheduler scheduler(4);
    
    // === Этап 1: Компиляция исходных файлов (независимы друг от друга) ===
    
    scheduler.add_task("compile_utils", []() {
        simulate_work("compile_utils", 100, 300);
    });
    
    scheduler.add_task("compile_network", []() {
        simulate_work("compile_network", 150, 350);
    });
    
    scheduler.add_task("compile_database", []() {
        simulate_work("compile_database", 200, 400);
    });
    
    scheduler.add_task("compile_ui", []() {
        simulate_work("compile_ui", 180, 320);
    });
    
    // === Этап 2: Компиляция модулей, зависящих от базовых ===
    
    scheduler.add_task("compile_auth", {"compile_utils", "compile_database"}, []() {
        simulate_work("compile_auth", 100, 250);
    });
    
    scheduler.add_task("compile_api", {"compile_network", "compile_database"}, []() {
        simulate_work("compile_api", 150, 300);
    });
    
    scheduler.add_task("compile_gui", {"compile_ui", "compile_utils"}, []() {
        simulate_work("compile_gui", 200, 350);
    });
    
    // === Этап 3: Линковка компонентов ===
    
    scheduler.add_task("link_backend", {"compile_auth", "compile_api"}, []() {
        simulate_work("link_backend", 100, 200);
    });
    
    scheduler.add_task("link_frontend", {"compile_gui", "compile_ui"}, []() {
        simulate_work("link_frontend", 100, 200);
    });
    
    // === Этап 4: Финальная сборка ===
    
    scheduler.add_task("package", {"link_backend", "link_frontend"}, []() {
        simulate_work("package", 150, 250);
    });
    
    scheduler.add_task("create_installer", {"package"}, []() {
        simulate_work("create_installer", 100, 200);
    });
    
    std::cout << "Added " << scheduler.task_count() << " tasks to scheduler\n" << std::endl;
    
    // Выполняем все задачи
    try {
        scheduler.execute();
        
        // Выводим статистику
        std::cout << scheduler.get_statistics() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
