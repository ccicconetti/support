cmake_policy(SET CMP0048 NEW)

project(uiiitsupport VERSION 1.0.0)
cmake_minimum_required(VERSION 3.2)

set (CMAKE_CXX_STANDARD 17)

# Cmake find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

if(APPLE)
  find_package(glog REQUIRED)
  set(GLOG glog::glog)
else()
  set(GLOG glog)
endif()
find_package(Protobuf REQUIRED)
find_package(GRPC REQUIRED)
find_package(Boost
  REQUIRED
  COMPONENTS program_options filesystem system chrono thread
)

set (DISABLE_WARNINGS "-Wno-missing-field-initializers -Wno-unused-parameter -Wnon-virtual-dtor")
set (COMPILER_COMMON_FLAGS "-Wall -Wextra -Werror -DGTEST_CREATE_SHARED_LIBRARY=1 -DGTEST_LINKED_AS_SHARED_LIBRARY=1 -fPIC ${DISABLE_WARNINGS}")
set (CMAKE_CXX_FLAGS_DEBUG "${COMPILER_COMMON_FLAGS} -g -O0")
set (CMAKE_CXX_FLAGS_RELEASE "${COMPILER_COMMON_FLAGS} -O2 -DNDEBUG")

MESSAGE("============CONFIGURATION SUMMARY================")
MESSAGE("")
MESSAGE("CMAKE_SOURCE_DIR:         ${CMAKE_CURRENT_SOURCE_DIR}")
MESSAGE("COMPILER:                 ${CMAKE_CXX_COMPILER}")
MESSAGE("COMPILER FLAGS DEBUG:     ${CMAKE_CXX_FLAGS_DEBUG}")
MESSAGE("COMPILER FLAGS RELEASE:   ${CMAKE_CXX_FLAGS_RELEASE}")
MESSAGE("CMAKE_BUILD_TYPE:         ${CMAKE_BUILD_TYPE}")
MESSAGE("Protobuf Library:         ${PROTOBUF_LIBRARY}")
MESSAGE("Protobuf Includes Path:   ${PROTOBUF_INCLUDE_DIR}")

set(PROTO_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto-src)
file(MAKE_DIRECTORY ${PROTO_SRC_DIR})
include_directories(${PROTO_SRC_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(Dataset)
add_subdirectory(Support)
add_subdirectory(RpcSupport)

string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_LOWER)
if (${CMAKE_BUILD_TYPE_LOWER} STREQUAL "debug")
  enable_testing()
  add_subdirectory(Test)
endif()
