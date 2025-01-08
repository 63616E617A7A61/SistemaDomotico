cmake_minimum_required(VERSION 3.10)
project(SistemaDomotico)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_library(${PROJECT_NAME} SHARED
    src/Auto.cpp
    src/Clock.cpp
    src/Device.cpp
    src/House.cpp
    src/Id.cpp
    src/Manual.cpp)
	
add_executable(main
	src/main.cpp)
	
target_link_libraries(main ${PROJECT_NAME})