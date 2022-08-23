add_library(uiiitdataset STATIC
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

add_executable(afdb-convert
  ${CMAKE_CURRENT_SOURCE_DIR}/afdb-convert.cpp
)

target_link_libraries(afdb-convert
  uiiitdataset
  uiiitsupport
  ${GLOG}
  ${Boost_LIBRARIES}
)

add_executable(afdb-cost
  ${CMAKE_CURRENT_SOURCE_DIR}/afdb-cost.cpp
)

target_link_libraries(afdb-cost
  uiiitdataset
  uiiitsupport
  ${GLOG}
  ${Boost_LIBRARIES}
)
