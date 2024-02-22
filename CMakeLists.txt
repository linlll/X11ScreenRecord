cmake_minimum_required(VERSION 3.10)

project(MAIN)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/opencv_videowriter)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/x11_screenshot)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/multitimer)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/x11_screenrecord)
