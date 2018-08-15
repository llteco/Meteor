cmake_minimum_required(VERSION 3.5.1)

add_library(jpeg SHARED jpeg.cc)
add_library(png SHARED png.cc)
add_dependencies(png png_copy_headers_to_destination zlib_copy_headers_to_destination)
target_include_directories(png PUBLIC ${png_INCLUDE_DIR} ${zlib_INCLUDE_DIR})
target_link_libraries(png ${png_STATIC_LIBRARIES} ${zlib_STATIC_LIBRARIES})
