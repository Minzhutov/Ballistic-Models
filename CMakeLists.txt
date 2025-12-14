cmake_minimum_required(VERSION 3.10)
project(TrajectoryProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Указываем, что include директория содержит заголовочные файлы
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Добавляем исполняемый файл
add_executable(trajectory_calc
    main.cpp
    src/Atmosphere.cpp
    src/trajectory.cpp
)

# Настройки компилятора
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(trajectory_calc PRIVATE -Wall -Wextra)
endif()

# Для Windows
if(WIN32)
    target_compile_definitions(trajectory_calc PRIVATE _USE_MATH_DEFINES)
endif()