# Get the git version
execute_process(
  COMMAND git describe --always
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_DESCRIBE
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

MESSAGE("GIT_DESCRIBE: ${GIT_DESCRIBE}")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
  ${CMAKE_CURRENT_SOURCE_DIR}/version.h
)

add_library(uiiitsupport SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/chrono.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/clioptions.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/conf.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/fileutils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/glograii.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/histogram.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/linearestimator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/mmtable.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/movingvariance.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/process.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/queue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/random.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/signalhandlerflag.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/signalhandlerwait.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/stat.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/system.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/thrower.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/uuid.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/versionutils.cpp
)

target_link_libraries(uiiitsupport
  ${GLOG}
  ${Boost_LIBRARIES}
)

