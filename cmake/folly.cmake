include (ExternalProject)

set(folly_URL https://github.com/facebook/folly.git)
set(folly_TAG v2018.07.30.00)
set(folly_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/folly/include)

if(WIN32)
  set(folly_STATIC_LIBS
    ${CMAKE_CURRENT_BINARY_DIR}/folly/lib/folly.lib)
  set(folly_STATIC_LIBS_DEBUG
    ${CMAKE_CURRENT_BINARY_DIR}/folly/lib/follyd.lib)
else()
  set(folly_STATIC_LIBS
    ${CMAKE_CURRENT_BINARY_DIR}/folly/lib/libfolly.a)
  set(folly_STATIC_LIBS_DEBUG
    ${CMAKE_CURRENT_BINARY_DIR}/folly/lib/libfollyd.a)
endif()

ExternalProject_Add(folly-src
  PREFIX folly
  GIT_REPOSITORY ${folly_URL}
  GIT_TAG ${folly_TAG}
  CMAKE_CACHE_ARGS
    -DCMAKE_DEBUG_POSTFIX:STRING=d
    -DCMAKE_CXX_FLAGS_DEBUG:STRING=${CMAKE_CXX_FLAGS_DEBUG}
    -DCMAKE_CXX_FLAGS_RELEASE:STRING=${CMAKE_CXX_FLAGS_RELEASE}
    -DCMAKE_INSTALL_PREFIX:STRING=<INSTALL_DIR>)

include_directories(${folly_INCLUDE_DIRS})
add_library(folly STATIC IMPORTED)
add_dependencies(folly folly-src)
set_target_properties(folly PROPERTIES
  IMPORTED_LOCATION ${folly_STATIC_LIBS}
  IMPORTED_LOCATION_DEBUG ${folly_STATIC_LIBS_DEBUG})
