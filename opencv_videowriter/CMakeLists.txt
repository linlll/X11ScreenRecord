cmake_minimum_required(VERSION 3.10)

project(opencv_videowriter)

find_package(OpenCV REQUIRED)
message(STATUS ${OpenCV_INCLUDE_DIRS})
message(STATUS ${OpenCV_LIBRARIES})

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} opencv_videowriter.cpp)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES})
