cmake_minimum_required (VERSION 2.6)

project (roguelike)

set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED system window graphics network audio)
find_package(Boost REQUIRED COMPONENTS coroutine context)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(roguelike game.cpp game_state.cpp level.cpp level_factory.cpp monster.cpp party.cpp player.cpp player_factory.cpp precompiled.cpp renderer.cpp roguelike.cpp utils.cpp)
target_link_libraries(roguelike ${SFML_LIBRARIES} ${Boost_COROUTINE_LIBRARY} ${Boost_CONTEXT_LIBRARY})