cmake_minimum_required(VERSION 3.10)
project(AtmosphereLibrary)

# Настройка библиотеки
add_library(atmosphere SHARED atmosphere.cpp atmosphere.h)

# Установка флага экспорта для Windows
if(WIN32)
    target_compile_definitions(atmosphere PRIVATE ATMOSPHERE_EXPORTS)
endif()

# Пример программы
add_executable(atmosphere_example main.cpp)
target_link_libraries(atmosphere_example atmosphere)