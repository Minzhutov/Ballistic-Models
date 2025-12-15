cmake_minimum_required(VERSION 3.10)

project(TrajectoryProject LANGUAGES CXX)

# Требуемый стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Директория с заголовками
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Если хотите пользоваться целевыми свойствами, объявим их явно
add_executable(trajectory_calc
    main.cpp
    src/atmosphere.cpp
    src/trajectory.cpp
)

# Опции компилятора для GNU/Clang
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(trajectory_calc PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Windows‑специфичное определение
if (WIN32)
    target_compile_definitions(trajectory_calc PRIVATE _USE_MATH_DEFINES)
endif()