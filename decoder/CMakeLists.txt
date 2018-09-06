cmake_minimum_required(VERSION 3.5.1)

add_library(jpeg SHARED jpeg.cc)

add_library(png SHARED png.cc)
add_dependencies(png png_copy_headers_to_destination zlib_copy_headers_to_destination)
target_include_directories(png PUBLIC ${png_INCLUDE_DIR} ${zlib_INCLUDE_DIR})
target_link_libraries(png ${png_STATIC_LIBRARIES} ${zlib_STATIC_LIBRARIES})

add_library(flo SHARED flo.cc)

add_custom_command(TARGET png POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:png> $<TARGET_FILE_DIR:meteor>
    COMMENT "copy png decoder dll to build folder"
)

add_custom_command(TARGET jpeg POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:jpeg> $<TARGET_FILE_DIR:meteor>
    COMMENT "copy jpeg decoder dll to build folder"
)

add_custom_command(TARGET flo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:flo> $<TARGET_FILE_DIR:meteor>
    COMMENT "copy flo decoder dll to build folder"
)
