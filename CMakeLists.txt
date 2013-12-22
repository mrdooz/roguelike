cmake_minimum_required (VERSION 2.6)

project (roguelike)

set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
set(COMMON_FLAGS "-Wno-switch-enum")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED system window graphics network audio)
find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${SFML_INCLUDE_DIRS})

file(GLOB SRC "*.cpp")

add_executable(roguelike ${SRC})
target_link_libraries(roguelike ${SFML_LIBRARIES})