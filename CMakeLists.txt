cmake_minimum_required(VERSION 3.5.1)
project(meteor VERSION 0.0.0.1)

# enable folder scope in ide
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(msvc_helper)

option(M_ENABLE_EXPERIMENAL_FUNC "Private" OFF)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ABI x64)
else()
  set(ABI x32)
endif()
message(STATUS "Compiling on ${ABI}")

enable_compile_flag_mt()
enable_unicode_charactor()
if(MSVC)
  set_msvc_warning_level(4)
endif(MSVC)

set(PROJECT_ROOT_DIR ${CMAKE_SOURCE_DIR})
include_directories(${PROJECT_ROOT_DIR})

include(imgui)
include(zlib)
include(png)
if(M_ENABLE_EXPERIMENAL_FUNC)
  include(protobuf)
  include(grpc)
  include(tensorflow)
endif()
add_subdirectory(graphic)
add_subdirectory(decoder)
add_subdirectory(meteor)
