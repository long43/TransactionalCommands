# https://github.com/bro/cmake/blob/master/FindJeMalloc.cmake
# - Try to find google test headers and libraries.
#
# Usage of this module as follows:
#
#     find_package(Thrift)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  Thrift_ROOT_DIR Set this variable to the root installation of
#                    Thrift if the module has problems finding
#                    the proper installation path.
#
# Variables defined by this module:
#
#  THRIFT_FOUND             System has Thrift libs/headers
#  THRIFT_LIBRARIES         The Thrift library/libraries
#  THRIFT_INCLUDE_DIR       The location of Thrift headers

find_path(THRIFT_ROOT_DIR
    NAMES include/Thrift/Thrift-config.h
)

find_library(THRIFT_LIBRARIES
    NAMES Thrift
    HINTS ${THRIFT_ROOT_DIR}/lib
)

find_library(THRIFT_BENCHMARK_LIBRARIES
    NAMES THRIFTbenchmark
    HINTS ${THRIFT_ROOT_DIR}/lib
)

find_path(THRIFT_INCLUDE_DIR
    NAMES Thrift/Thrift-config.h
    HINTS ${THRIFT_ROOT_DIR}/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Thrift DEFAULT_MSG
    THRIFT_LIBRARIES
    THRIFT_INCLUDE_DIR
)

mark_as_advanced(
    THRIFT_ROOT_DIR
    THRIFT_LIBRARIES
    THRIFT_BENCHMARK_LIBRARIES
    THRIFT_INCLUDE_DIR
)