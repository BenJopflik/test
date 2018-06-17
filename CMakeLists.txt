project(TEST)
cmake_minimum_required(VERSION 3.1)

set (CMAKE_CXX_STANDARD 17)

find_package(CapnProto CONFIG REQUIRED)



#capnp_generate_cpp(addressbookSources addressbookHeaders addressbook.capnp)
#add_executable(addressbook addressbook.c++ ${addressbookSources})
#target_link_libraries(addressbook PRIVATE CapnProto::capnp)
#target_include_directories(addressbook PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Don't build the rpc sample if find_package() found an installation of Cap'n Proto lite.
#if(TARGET CapnProto::capnp-rpc)
  capnp_generate_cpp(bankSources bankHeaders bank.capnp)
  add_executable(bank-client bank-client.cpp ${bankSources})
  add_executable(bank-server bank-server.cpp ${bankSources})
  target_link_libraries(bank-client PRIVATE CapnProto::capnp-rpc)
  target_link_libraries(bank-server PRIVATE CapnProto::capnp-rpc)
  target_include_directories(bank-client PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
  target_include_directories(bank-server PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
#endif()
