cmake_minimum_required(VERSION 3.5.1)

file(GLOB_RECURSE CORE ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cc ${CMAKE_CURRENT_SOURCE_DIR}/core/*.h)
file(GLOB_RECURSE UI ${CMAKE_CURRENT_SOURCE_DIR}/ui/*.cc ${CMAKE_CURRENT_SOURCE_DIR}/ui/*.h)
file(GLOB_RECURSE LODE ${CMAKE_CURRENT_SOURCE_DIR}/lodepng/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/lodepng/*.h)

add_executable(meteor ${CORE} ${UI} ${LODE})
target_link_libraries(meteor imgui ixr_engine)
