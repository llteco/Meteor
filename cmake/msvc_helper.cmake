## A bundle of cmake helper functions used in MSVC
## Colleted from internet
## Contact Email: wenyi.tang@intel.com
## Date: 2017.11.16

set(ALL_CMAKE_CXX_FLAGS
  CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
  CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
set(ALL_CMAKE_C_FLAGS
  CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
  CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
set(DEFAULT_WARNING_LEVEL "/W3")

## Using static vc runtime
macro(enable_compile_flag_mt)
  if(MSVC)
    # For MSVC, CMake sets certain flags to defaults we want to override.
    # This replacement code is taken from sample in the CMake Wiki at
    # http://www.cmake.org/Wiki/CMake_FAQ#Dynamic_Replace.
    foreach (flag_var ${ALL_CMAKE_CXX_FLAGS} ${ALL_CMAKE_C_FLAGS})
      string(REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    endforeach()
  endif(MSVC)
endmacro(enable_compile_flag_mt)

## Using unicode charactors, by default it uses multibytes
macro(enable_unicode_charactor)
  # Use unicode
  add_definitions(-DUNICODE)
  add_definitions(-D_UNICODE)
  # Disable max & min macros
  add_definitions(-DNOMINMAX)
endmacro(enable_unicode_charactor)

## Change warning levels
macro(set_msvc_warning_level level)
  if(MSVC)
    foreach (flag_var ${ALL_CMAKE_CXX_FLAGS} ${ALL_CMAKE_C_FLAGS})
      string(REPLACE ${DEFAULT_WARNING_LEVEL} "/W${level}" ${flag_var} "${${flag_var}}")
    endforeach()
    set(DEFAULT_WARNING_LEVEL "/W${level}")
  endif()
endmacro(set_msvc_warning_level)

## Ignore specific warnings
macro(ignore_warning ...)
  foreach(warn ${ARGV})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd${warn}")
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -wd${warn}")
  endforeach()
endmacro(ignore_warning)

## Auto update version string in .rc resource file of VS project
function(gen_version_rc ...)
  # Func: gen_version_rc(ver resource output icon)
  # Param [in] ver: version numbers (i.e. x.x.x.x)
  # Param [in] resource: path to resource file without suffix
  # Param [in] output: path to generated file without suffix
  # Param [optional] icon: path to .ico file
  set(resource ${ARGV1})
  set(output ${ARGV2})
  set(icon ${ARGV3})
  file(STRINGS ${resource}.rc rcfile)
  set(rcoverwrite)
  string(REPLACE "." "," prodver "${ARGV0}")
  foreach(line ${rcfile})
    string(REGEX MATCH "@VERSION@" vermatch "${line}")
    string(REGEX MATCH "@ICON@" iconmatch "${line}")
    if(vermatch)
      string(REGEX REPLACE "@VERSION@" "${prodver}" _line "${line}")
      string(APPEND rcoverwrite "${_line}\n")
    elseif(iconmatch)
    string(REGEX REPLACE "@ICON@" "${icon}" _line "${line}")
    string(APPEND rcoverwrite "${_line}\n")
    else()
      string(APPEND rcoverwrite "${line}\n")
    endif()
  endforeach()
  file(WRITE ${output}.rc "${rcoverwrite}")
  file(COPY ${resource}.h DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endfunction(gen_version_rc)

function(ixr_require_cxx_standard std)
  if(${std} EQUAL 11 OR ${std} EQUAL 14 OR ${std} EQUAL 17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++${std}" PARENT_SCOPE)
  else()
    message(FATAL_ERROR "wrong c++ standard! (std:c++11/14/17)")
  endif()
endfunction(ixr_require_cxx_standard)
