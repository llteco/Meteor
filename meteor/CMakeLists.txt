cmake_minimum_required(VERSION 3.5.1)

file(GLOB_RECURSE CORE ${PROJECT_ROOT_DIR}/meteor/core/*.cc ${PROJECT_ROOT_DIR}/meteor/core/*.h)
file(GLOB_RECURSE UI ${PROJECT_ROOT_DIR}/meteor/ui/*.cc ${PROJECT_ROOT_DIR}/meteor/ui/*.h)

add_executable(meteor ${CORE} ${UI})
target_link_libraries(meteor imgui ixr_engine)
