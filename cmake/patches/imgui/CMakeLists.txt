cmake_minimum_required(VERSION 3.5.1)

project(imgui VERSION 1.6.0)
include(GNUInstallDirs)

option(IMGUI_BUILD_EXAMPLES "Set \"ON\" to build imgui examples" OFF)

# using unicode in VS
if(MSVC)
  ADD_DEFINITIONS(-DUNICODE)
  ADD_DEFINITIONS(-D_UNICODE)
endif(MSVC)

macro(imgui_build_example ...)
  set(name ${ARGV0})
  set(EXAMPLE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/examples/${name}_example)
  file(GLOB src_${name} ${EXAMPLE_ROOT}/*.cpp ${EXAMPLE_ROOT}/*.h)
  add_executable(${name}_example ${src_${name}})
  set(link_libs imgui)
  foreach(_lib ${ARGV})
    if (${ARGC} EQUAL 1)
      break()
    endif()
    if (${_lib} STREQUAL ${ARGV0})
      continue()
    endif()
    list(APPEND link_libs ${_lib})
  endforeach()
  target_link_libraries(${name}_example ${link_libs})
  install(TARGETS ${name}_example RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endmacro()

if(IMGUI_BUILD_EXAMPLES)
  find_package(Vulkan QUIET)
  find_package(GLFW QUIET)

  imgui_build_example(null)
  if(WIN32)
    imgui_build_example(directx9 d3d9)
    imgui_build_example(directx10 d3d10 dxgi d3dcompiler)
    imgui_build_example(directx11 d3d11 dxgi d3dcompiler)
    imgui_build_example(directx12 d3d12 dxgi d3dcompiler)
  endif(WIN32)
  if(Vulkan_FOUND AND GLFW_FOUND)
    imgui_build_example(vulkan ${Vulkan_LIBRARY} ${GLFW_LIBRARY})
    target_include_directories(vulkan_example PRIVATE
      ${Vulkan_INCLUDE_DIR} ${GLFW_INCLUDE_DIR})
  endif()
endif()

file(GLOB SOURCE ./*.cpp)
file(GLOB HEADER ./*.h)

add_library(imgui STATIC ${HEADER} ${SOURCE})
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(FILES ${HEADER} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/imgui)
install(TARGETS imgui ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
