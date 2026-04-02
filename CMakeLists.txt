cmake_minimum_required(VERSION 3.10)
project(TaskScheduler VERSION 1.0.0 LANGUAGES CXX)

# Требуем C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Флаги компиляции
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Включаем директорию с заголовками
include_directories(${PROJECT_SOURCE_DIR}/include)

# === Библиотека Task Scheduler ===
add_library(task_scheduler STATIC
    src/task.cpp
    src/dag.cpp
    src/thread_pool.cpp
    src/scheduler.cpp
)

target_include_directories(task_scheduler PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# Линковка с pthread на Linux/macOS
if(UNIX)
    target_link_libraries(task_scheduler pthread)
endif()

# === Примеры ===
add_executable(build_system_example
    examples/build_system.cpp
)

target_link_libraries(build_system_example
    task_scheduler
)

# === Опция для сборки тестов (если будут) ===
option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
    enable_testing()
    # Здесь можно добавить тесты
endif()

# === Информация о сборке ===
message(STATUS "Task Scheduler Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
