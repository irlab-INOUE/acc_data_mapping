cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
set (CMAKE_CXX_FLAGS "-O3 -g3 -std=c++17")
project(imgMapSamples)

include_directories(${PROJECT_SOURCE_DIR})
link_directories(${PROJECT_SOURCE_DIR})

find_package (OpenCV REQUIRED)

add_executable (main main.cpp)
add_executable (conf conf.cpp)
add_executable (sta_test sta_test.cpp)
target_link_libraries(main ${OpenCV_LIBRARIES})
target_link_libraries(conf ${OpenCV_LIBRARIES})
