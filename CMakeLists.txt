project(TEST)
cmake_minimum_required(VERSION 3.1)

add_subdirectory(googletest)

set (CMAKE_CXX_STANDARD 17)

find_package(CapnProto CONFIG REQUIRED)

set (gtest_include_path "${CMAKE_SOURCE_DIR}/googletest/googletest/include")
set (gmock_include_path "${CMAKE_SOURCE_DIR}/googletest/googlemock/include")

set (gmock_library_path "${CMAKE_BINARY_DIR}/googletest/googlemock/")
set (gtest_library_path "${CMAKE_BINARY_DIR}/googletest/googlemock/gtest/")


set (SERVER "server")
set (CLIENT "client")


capnp_generate_cpp(bankSources bankHeaders bank.capnp)

add_executable(${CLIENT} bank-client.cpp ${bankSources})
target_link_libraries(${CLIENT} PRIVATE CapnProto::capnp-rpc)
target_include_directories(${CLIENT} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${SERVER} server.cpp ${bankSources})
target_link_libraries(${SERVER} PRIVATE CapnProto::capnp-rpc)
target_include_directories(${SERVER} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})


link_directories(${gmock_library_path} ${gtest_library_path})

set (TESTS tests)

set (TEST_SOURCES ${CMAKE_SOURCE_DIR}/tests/bank_test.cpp
                  ${CMAKE_SOURCE_DIR}/tests/client_test.cpp
                  ${CMAKE_SOURCE_DIR}/tests/transfer_processor_test.cpp

                  )

add_executable (${TESTS} ${TEST_SOURCES})
target_include_directories (${TESTS} PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories (${TESTS} PRIVATE ${gtest_include_path})
target_include_directories (${TESTS} PRIVATE ${gmock_include_path})
target_link_libraries (${TESTS} pthread gmock_main gmock)
target_link_libraries(${CLIENT} PRIVATE CapnProto::capnp-rpc)
target_include_directories(${SERVER} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})


