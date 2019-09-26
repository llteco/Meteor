cmake_minimum_required(VERSION 3.5.1)
unset(CMAKE_DEBUG_POSTFIX)
add_library(jpeg SHARED jpeg.cc)

add_custom_command(TARGET jpeg POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:jpeg> $<TARGET_FILE_DIR:meteor>
    COMMENT "copy jpeg decoder dll to build folder")
