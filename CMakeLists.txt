

cmake_minimum_required(VERSION 3.11)

project(bmp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
# Установка переменной со списком исходников
#set( SOURCE_LIB 
#    src/main.cpp
#)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/headers)

add_executable( bmp ${SOURCE_LIB} src/main.cpp)

#target_link_libraries(math_parese PRIVATE MathParseLib)