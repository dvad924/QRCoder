cmake_minimum_required(VERSION 2.8.11)
project(qr-reader)

find_package(OpenCV REQUIRED)

add_executable(reader src/main.cpp
                      src/qr.cpp)

target_link_libraries(reader ${OpenCV_LIBS})
set_property(TARGET reader PROPERTY CXX_STANDARD 11)
set_property(TARGET reader PROPERTY CXX_STANDARD_REQUIRED ON )
