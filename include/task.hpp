#ifndef TASK_HPP
#define TASK_HPP

#include <string>
#include <functional>
#include <vector>
#include <memory>
#include <chrono>

/**
 * @brief Статус выполнения задачи
 */
enum class TaskStatus {
    PENDING,    // Задача ожидает выполнения
    RUNNING,    // Задача выполняется
    COMPLETED,  // Задача завершена успешно
    FAILED      // Задача завершена с ошибкой
};

/**
 * @brief Класс представляющий одну задачу в графе зависимостей
 */
class Task {
public:
    using TaskFunction = std::function<void()>;
    
    /**
     * @brief Конструктор задачи
     * @param id Уникальный идентификатор задачи
     * @param func Функция для выполнения
     */
    Task(const std::string& id, TaskFunction func);
    
    /**
     * @brief Получить ID задачи
     */
    const std::string& get_id() const { return id_; }
    
    /**
     * @brief Получить текущий статус задачи
     */
    TaskStatus get_status() const { return status_; }
    
    /**
     * @brief Установить статус задачи
     */
    void set_status(TaskStatus status) { status_ = status; }
    
    /**
     * @brief Получить список зависимостей (ID задач, от которых зависит эта)
     */
    const std::vector<std::string>& get_dependencies() const { return dependencies_; }
    
    /**
     * @brief Добавить зависимость
     */
    void add_dependency(const std::string& dep_id);
    
    /**
     * @brief Выполнить задачу
     */
    void execute();
    
    /**
     * @brief Получить время выполнения задачи в миллисекундах
     */
    long long get_execution_time_ms() const { return execution_time_ms_; }
    
    /**
     * @brief Получить сообщение об ошибке (если задача завершилась с FAILED)
     */
    const std::string& get_error_message() const { return error_message_; }

private:
    std::string id_;                           // Уникальный идентификатор
    TaskFunction func_;                        // Функция для выполнения
    TaskStatus status_;                        // Текущий статус
    std::vector<std::string> dependencies_;    // Список зависимостей
    long long execution_time_ms_;              // Время выполнения
    std::string error_message_;                // Сообщение об ошибке
};

#endif // TASK_HPP
