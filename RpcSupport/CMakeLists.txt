add_library(uiiitrpcsupport SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/simpleserver.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
)

target_link_libraries(uiiitrpcsupport
  ${GLOG}
  gRPC::grpc++_reflection
  protobuf::libprotobuf
)
