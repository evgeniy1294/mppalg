cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

set(TARGET test)

set($(TARGET)_VERSION "0.0.1")

project(${TARGET} VERSION ${${TARGET}_VERSION} LANGUAGES CXX C)

# ------------ [ sources ] ------------
set(SOURCE_LIST
   )

add_executable(${TARGET} main.cpp)
target_sources(${TARGET} PRIVATE ${SOURCE_LIST})
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../include)
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_compile_features(${TARGET} PRIVATE cxx_std_20)
