cmake_minimum_required(VERSION 3.10)

project(SCREENRECORD)

message(STATUS ${MULTITIMER_SOURCE_DIR})

find_package(OpenCV REQUIRED)
message(STATUS ${OpenCV_INCLUDE_DIRS})
message(STATUS ${OpenCV_LIBRARIES})

find_package(X11 REQUIRED)
message(STATUS ${X11_LIBRARIES})

include_directories(${MULTITIMER_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cpp screen_record.cpp ${MULTITIMER_SOURCE_DIR}/multitimer.cpp)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES} ${X11_LIBRARIES})
