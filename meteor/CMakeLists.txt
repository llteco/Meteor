# Copyright (c) 2019 Wenyi Tang
# Author: Wenyi Tang
# E-mail: wenyitang@outlook.com

option(MT_NO_CONSOLE "Use subsystem:windows" OFF)

file(GLOB_RECURSE CORE ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cc ${CMAKE_CURRENT_SOURCE_DIR}/core/*.h)
file(GLOB_RECURSE UI ${CMAKE_CURRENT_SOURCE_DIR}/ui/*.cc ${CMAKE_CURRENT_SOURCE_DIR}/ui/*.h)
file(GLOB_RECURSE LODE ${CMAKE_CURRENT_SOURCE_DIR}/lodepng/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/lodepng/*.h)

add_subdirectory(builtin)
add_executable(meteor ${CORE} ${UI} ${LODE})
target_link_libraries(meteor builtin imgui ixr_engine ixr_codec)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/mt_config.in
    ${CMAKE_CURRENT_BINARY_DIR}/mt_config.h)
target_include_directories(meteor PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(MT_NO_CONSOLE)
    set_target_properties(meteor PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif()
