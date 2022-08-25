if(NOT skip_google_test)

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

endif()

include_directories(${CMAKE_BINARY_DIR}/googletest-src/googletest/include)

include(GoogleTest)

set(LIBS
  uiiitsupport
  gtest
  ${Boost_LIBRARIES}
)

if(WITH_TEST_RPC)
  protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SRC_DIR} testservice.proto)
  grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_SRC_DIR} testservice.proto)

  add_library(testservice STATIC
    ${PROTO_SRCS}
    ${GRPC_SRCS}
  )

  add_executable(testrpc testmain.cpp testrpc.cpp)
  target_link_libraries(testrpc
    ${LIBS}
    testservice
    uiiitrpcsupport
    gRPC::grpc++_reflection
    protobuf::libprotobuf
  )
  gtest_discover_tests(testrpc)
endif()

add_executable(testconf testmain.cpp testconf.cpp)
target_link_libraries(testconf ${LIBS})
gtest_discover_tests(testconf)

add_executable(testexperimentdata testmain.cpp testexperimentdata.cpp)
target_link_libraries(testexperimentdata ${LIBS})
gtest_discover_tests(testexperimentdata)

add_executable(testfit testmain.cpp testfit.cpp)
target_link_libraries(testfit ${LIBS})
gtest_discover_tests(testfit)

add_executable(testhistogram testmain.cpp testhistogram.cpp)
target_link_libraries(testhistogram ${LIBS})
gtest_discover_tests(testhistogram)

add_executable(testjain testmain.cpp testjain.cpp)
target_link_libraries(testjain ${LIBS})
gtest_discover_tests(testjain)

add_executable(testmath testmain.cpp testmath.cpp)
target_link_libraries(testmath ${LIBS})
gtest_discover_tests(testmath)

add_executable(testmmtable testmain.cpp testmmtable.cpp)
target_link_libraries(testmmtable ${LIBS})
gtest_discover_tests(testmmtable)

add_executable(testmovingavg testmain.cpp testmovingavg.cpp)
target_link_libraries(testmovingavg ${LIBS})
gtest_discover_tests(testmovingavg)

add_executable(testmovingvariance testmain.cpp testmovingvariance.cpp)
target_link_libraries(testmovingvariance ${LIBS})
gtest_discover_tests(testmovingvariance)

add_executable(testmovingwnd testmain.cpp testmovingwnd.cpp)
target_link_libraries(testmovingwnd ${LIBS})
gtest_discover_tests(testmovingwnd)

add_executable(testparallelbatch testmain.cpp testparallelbatch.cpp)
target_link_libraries(testparallelbatch ${LIBS})
gtest_discover_tests(testparallelbatch)

add_executable(testperiodictask testmain.cpp testperiodictask.cpp)
target_link_libraries(testperiodictask ${LIBS})
gtest_discover_tests(testperiodictask)

add_executable(testprocess testmain.cpp testprocess.cpp)
target_link_libraries(testprocess ${LIBS})
gtest_discover_tests(testprocess)

add_executable(testqueue testmain.cpp testqueue.cpp)
target_link_libraries(testqueue ${LIBS})
gtest_discover_tests(testqueue)

add_executable(testrandom testmain.cpp testrandom.cpp)
target_link_libraries(testrandom ${LIBS})
gtest_discover_tests(testrandom)

add_executable(testsplit testmain.cpp testsplit.cpp)
target_link_libraries(testsplit ${LIBS})
gtest_discover_tests(testsplit)

add_executable(testsystem testmain.cpp testsystem.cpp)
target_link_libraries(testsystem ${LIBS})
gtest_discover_tests(testsystem)

add_executable(teststat testmain.cpp teststat.cpp)
target_link_libraries(teststat ${LIBS})
gtest_discover_tests(teststat)

add_executable(testthreadpool testmain.cpp testthreadpool.cpp)
target_link_libraries(testthreadpool ${LIBS})
gtest_discover_tests(testthreadpool)

add_executable(testuuid testmain.cpp testuuid.cpp)
target_link_libraries(testuuid ${LIBS})
gtest_discover_tests(testuuid)

add_executable(testwait testmain.cpp testwait.cpp)
target_link_libraries(testwait ${LIBS})
gtest_discover_tests(testwait)

