include(ExternalProject)

set(tf_URL https://github.com/loseall/tensorflow-cmake.git)
set(tf_TAG master)
set(tf_INSTALL_DIRS ${CMAKE_BINARY_DIR}/tensorflow)
set(tf_INSTALL_FILE ${tensorflow_HOME}/tensorflow/contrib/cmake/build/cmake_install.cmake)
set(tf_CMAKE_HOME ${tensorflow_HOME}/tensorflow/contrib/cmake)

add_definitions(-DEIGEN_AVOID_STL_ARRAY)
if(WIN32)
  add_definitions(-DNOMINMAX -D_WIN32_WINNT=0x0A00 -DLANG_CXX11 -DCOMPILER_MSVC)
  add_definitions(-DWIN32 -DOS_WIN -D_MBCS -DWIN64 -DWIN32_LEAN_AND_MEAN -DPLATFORM_WINDOWS)
  add_definitions(-DTENSORFLOW_USE_EIGEN_THREADPOOL -DEIGEN_HAS_C99_MATH)
  add_definitions(-DTF_COMPILE_LIBRARY)
  # add_definitions(/bigobj /nologo /EHsc /GF /MP /Gm-)
  # Suppress warnings to reduce build log size.
  add_definitions(/wd4267 /wd4244 /wd4800 /wd4503 /wd4554 /wd4996 /wd4348 /wd4018)
  add_definitions(/wd4099 /wd4146 /wd4267 /wd4305 /wd4307)
  add_definitions(/wd4715 /wd4722 /wd4723 /wd4838 /wd4309 /wd4334)
  add_definitions(/wd4003 /wd4244 /wd4267 /wd4503 /wd4506 /wd4800 /wd4996)
  add_definitions(/wd4100 /wd4127 /wd4201 /wd4245 /wd4505)
endif()

ExternalProject_Add(tensorflow
  PREFIX ${CMAKE_BINARY_DIR}/tensorflow
  SOURCE_DIR ${CMAKE_BINARY_DIR}/tensorflow/src
  DOWNLOAD_COMMAND ""
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${tf_INSTALL_DIRS} -P ${tf_INSTALL_FILE}
)

include_directories(
  ${tf_INSTALL_DIRS}/include
  ${tf_INSTALL_DIRS}/include/external/nsync/public)

find_file(tf_binary NAMES "tensorflow.dll"
  HINTS
  ${tf_INSTALL_DIRS}/bin
  DOC "location of built shared tensorflow dll")
find_library(tf_library NAMES "tensorflow.lib"
  HINTS
  ${tf_INSTALL_DIRS}/lib
  DOC "location of built tensorflow lib")
add_library(libtf SHARED IMPORTED)
add_dependencies(libtf tensorflow)
set_target_properties(libtf PROPERTIES
  IMPORTED_IMPLIB ${tf_library}
  IMPORTED_LOCATION ${tf_binary})
