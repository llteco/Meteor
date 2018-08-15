include (ExternalProject)

set(imgui_URL https://github.com/ocornut/imgui.git)
set(imgui_TAG v1.62)
set(imgui_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/imgui/include)

if(WIN32)
  set(imgui_STATIC_LIBS
    ${CMAKE_CURRENT_BINARY_DIR}/imgui/lib/imgui.lib)
  set(imgui_STATIC_LIBS_DEBUG
    ${CMAKE_CURRENT_BINARY_DIR}/imgui/lib/imguid.lib)
else()
  set(imgui_STATIC_LIBS
    ${CMAKE_CURRENT_BINARY_DIR}/imgui/lib/libimgui.a)
  set(imgui_STATIC_LIBS_DEBUG
    ${CMAKE_CURRENT_BINARY_DIR}/imgui/lib/libimguid.a)
endif()

ExternalProject_Add(imgui-src
  PREFIX imgui
  GIT_REPOSITORY ${imgui_URL}
  GIT_TAG ${imgui_TAG}
  PATCH_COMMAND ${CMAKE_COMMAND} -E copy_if_different ...
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/patches/imgui/CMakeLists.txt <SOURCE_DIR>
  CMAKE_CACHE_ARGS
    -DIMGUI_BUILD_EXAMPLES:BOOL=OFF
    -DCMAKE_DEBUG_POSTFIX:STRING=d
    -DCMAKE_CXX_FLAGS_DEBUG:STRING=${CMAKE_CXX_FLAGS_DEBUG}
    -DCMAKE_CXX_FLAGS_RELEASE:STRING=${CMAKE_CXX_FLAGS_RELEASE}
    -DCMAKE_INSTALL_PREFIX:STRING=<INSTALL_DIR>)

include_directories(${imgui_INCLUDE_DIRS})
add_library(imgui STATIC IMPORTED)
add_dependencies(imgui imgui-src)
set_target_properties(imgui PROPERTIES
  IMPORTED_LOCATION ${imgui_STATIC_LIBS}
  IMPORTED_LOCATION_DEBUG ${imgui_STATIC_LIBS_DEBUG})
