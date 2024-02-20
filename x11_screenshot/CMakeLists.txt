cmake_minimum_required(VERSION 3.10)

project(SCREENSHOT)

find_package(X11 REQUIRED)
message(STATUS ${X11_LIBRARIES})

find_package(OpenCV REQUIRED)
message(STATUS ${OpenCV_INCLUDE_DIRS})
message(STATUS ${OpenCV_LIBRARIES})

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} screenshot.cpp)
target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES} ${OpenCV_LIBRARIES})
