cmake_minimum_required(VERSION 3.10)
project(Atmosphere)  

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(atmosphere_app main.cpp atmosphere.cpp)


if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(atmosphere_app PRIVATE -Wall -Wextra -pedantic)
endif()


if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(atmosphere_app PRIVATE -std=c++11)
endif()