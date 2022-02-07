add_executable(afdb
  ${CMAKE_CURRENT_SOURCE_DIR}/afdb.cpp
)

target_link_libraries(afdb
  uiiitsupport
  ${GLOG}
  ${Boost_LIBRARIES}
)

