# Download and unpack googletest at configure time
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Test/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Test/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

include_directories(${CMAKE_BINARY_DIR}/googletest-src/googletest/include)

add_library(testlib SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/testconf.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/testfit.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/testhistogram.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/testmath.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/testmmtable.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/testmovingavg.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/testmovingvariance.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/testmovingwnd.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/testperiodictask.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/testprocess.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/testqueue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/testrandom.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/testsplit.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/testsystem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/teststat.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/testthreadpool.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/testuuid.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/testwait.cpp
)

target_link_libraries(testlib
  uiiitsupport
  gtest
  ${Boost_LIBRARIES}
)

add_executable(testmain
  ${CMAKE_CURRENT_SOURCE_DIR}/testmain.cpp
)

target_link_libraries(testmain
  testlib
)
