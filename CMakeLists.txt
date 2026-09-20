cmake_minimum_required(VERSION 3.10)
project(asd_lab_1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

foreach(number RANGE 1 10)
    if(number LESS 10)
        set(folder "task_0${number}")
    else()
        set(folder "task_${number}")
    endif()
    add_executable(${folder} ${folder}/main.cpp)
    target_include_directories(${folder} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endforeach()

