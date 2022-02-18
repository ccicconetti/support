add_library(uiiitdataset SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/afdb-utils.cpp
)

target_link_libraries(uiiitdataset
  ${GLOG}
)

add_executable(afdb
  ${CMAKE_CURRENT_SOURCE_DIR}/afdb.cpp
)

target_link_libraries(afdb
  uiiitdataset
  uiiitsupport
  ${GLOG}
  ${Boost_LIBRARIES}
)

