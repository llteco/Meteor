add_library(builtin STATIC flo.cc png.cc)
target_link_libraries(builtin PUBLIC
    ${png_STATIC_LIBRARIES} ${zlib_STATIC_LIBRARIES})
target_include_directories(builtin PUBLIC
    ${png_INCLUDE_DIR} ${zlib_INCLUDE_DIR})
add_dependencies(builtin
    png_copy_headers_to_destination zlib_copy_headers_to_destination)
