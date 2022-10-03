add_library(uiiitrpcsupport STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/simpleserver.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
)

target_link_libraries(uiiitrpcsupport
  uiiitsupport
  ${GLOG}
  gRPC::grpc++_reflection
  protobuf::libprotobuf
  absl_synchronization
)
