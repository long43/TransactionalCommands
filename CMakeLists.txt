cmake_minimum_required(VERSION 3.0.0)
project(TransactionalCommand VERSION 0.1.0)

ENABLE_TESTING()

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(FOLLY_ROOT_DIR "/usr/local")
set(GLOG_ROOT_DIR "/usr/local")
set(GFLAGS_ROOT_DIR "/usr")
#set(THRIFT_ROOT_DIR "/usr/local")

find_package(Folly)
find_package(GFlags)
find_package(Glog)
#find_package(Thrift)

message("folly include path is " ${FOLLY_INCLUDE_DIR})
message("folly lib path is " ${FOLLY_LIBRARIES})
message("glog include path is " ${GLOG_INCLUDE_DIRS})
message("glog lib path is " ${GLOG_LIBRARIES})
message("gflags include path is " ${GFLAGS_INCLUDE_DIRS})
message("gflags lib path is " ${GFLAGS_LIBRARIES})
#message("thrift include path is " ${THRIFT_INCLUDE_DIR})
#message("thrift lib path is " ${THRIFT_LIBRARIES})

include_directories(${FOLLY_INCLUDE_DIR})
#include_directories(${THRIFT_INCLUDE_DIR})
include_directories("/usr/local/include/")
include_directories(${source_dir})

set(SOURCE 
  src/tests/TestCar.h
  src/tests/TestCar.cpp
  src/tests/TestTruck.h
  src/transactional/TransactionalCommand.h
  src/transactional/TransactionalCommand.cpp
  src/transactional/TransactionExecutor.h
  src/transactional/TransactionExecutor.cpp
  src/Playground.cpp
)

set(TESTSOURCE
  src/tests/TestCar.h
  src/tests/TestCar.cpp
  src/tests/TestTruck.h
  src/transactional/TransactionalCommand.h
  src/transactional/TransactionalCommand.cpp
  src/transactional/TransactionExecutor.h
  src/transactional/TransactionExecutor.cpp
  src/tests/TransactionalCommandTest.cpp
)

add_executable(TransactionalCommand ${SOURCE})
add_executable(TransactionalCommandTest ${TESTSOURCE})

set(CMAKE_CXX_FLAGS "-std=c++17 -v -pthread")

target_link_libraries(TransactionalCommand ${GLOG_LIBRARIES})
target_link_libraries(TransactionalCommand ${GFLAGS_LIBRARIES})
target_link_libraries(TransactionalCommand ${FOLLY_LIBRARIES})
#target_link_libraries(TransactionalCommand ${THRIFT_LIBRARIES})

target_link_libraries(TransactionalCommandTest ${FOLLY_LIBRARIES})
#target_link_libraries(TransactionalCommandTest ${THRIFT_LIBRARIES})
target_link_libraries(TransactionalCommandTest ${GLOG_LIBRARIES})
target_link_libraries(TransactionalCommandTest ${GFLAGS_LIBRARIES})
target_link_libraries(TransactionalCommandTest "/usr/local/lib/libgtest.a")
target_link_libraries(TransactionalCommandTest "/usr/local/lib/libgtest_main.a")

add_test(NAME    TransactionalCommandTest 
         COMMAND TransactionalCommandTest)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
