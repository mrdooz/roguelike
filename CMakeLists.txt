cmake_minimum_required (VERSION 2.6)

project (roguelike)

set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
set(COMMON_FLAGS "-Wno-switch-enum")

include(FindProtobuf)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED system window graphics network audio)
find_package(Boost REQUIRED)
find_package(Protobuf REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${SFML_INCLUDE_DIRS})
include_directories(${PROTOBUF_INCLUDE_DIR})

file(GLOB SRC "*.cpp" "protocol/*.pb.cc")

add_executable(roguelike ${SRC})

if (APPLE)
	find_library(APP_SERVICES ApplicationServices)
    set_target_properties(
        ${PROJECT_NAME} 
        PROPERTIES
        XCODE_ATTRIBUTE_GCC_PREFIX_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/precompiled.hpp"
        XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER "YES"
        LINK_FLAGS "-F/Library/Frameworks"
    )
endif()

target_link_libraries(roguelike ${SFML_LIBRARIES} ${Boost_DATETIME_LIBRARY} ${PROTOBUF_LIBRARY} ${APP_SERVICES} )
