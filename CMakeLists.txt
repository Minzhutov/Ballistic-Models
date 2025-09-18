cmake_minimum_required(VERSION 3.10)
project(AtmosphereLibrary)

# Создание библиотеки
add_library(atmosphere SHARED atmosphere.cpp)

# Создание исполняемого файла
add_executable(atmosphere_example main.cpp)

# Линковка библиотеки с исполняемым файлом
target_link_libraries(atmosphere_example atmosphere)