#############################
# Test
#############################

# Fetch GoogleTest
Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.3.2 # or a later release
)

FetchContent_MakeAvailable(Catch2)

set(PINE_TEST_DIR ${PROJECT_SOURCE_DIR}/test)
set(PINE_TEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/test/include)

add_executable(net_address_test ${PINE_TEST_DIR}/core/net_address_test.cpp)
target_link_libraries(net_address_test PRIVATE Catch2::Catch2WithMain pine_core)

add_executable(socket_test ${PINE_TEST_DIR}/core/socket_test.cpp)
target_link_libraries(socket_test PRIVATE Catch2::Catch2WithMain pine_core)

add_executable(buffer_test ${PINE_TEST_DIR}/core/buffer_test.cpp)
target_link_libraries(buffer_test PRIVATE Catch2::Catch2WithMain pine_core)

add_executable(connection_test ${PINE_TEST_DIR}/core/connection_test.cpp)
target_link_libraries(connection_test PRIVATE Catch2::Catch2WithMain pine_core)

add_executable(poller_test ${PINE_TEST_DIR}/core/poller_test.cpp)
target_link_libraries(poller_test PRIVATE Catch2::Catch2WithMain pine_core)

add_executable(looper_test ${PINE_TEST_DIR}/core/looper_test.cpp)
target_link_libraries(looper_test PRIVATE Catch2::Catch2WithMain pine_core)

add_executable(acceptor_test ${PINE_TEST_DIR}/core/acceptor_test.cpp)
target_link_libraries(acceptor_test PRIVATE Catch2::Catch2WithMain pine_core)

add_executable(thread_pool_test ${PINE_TEST_DIR}/core/thread_pool_test.cpp)
target_link_libraries(thread_pool_test PRIVATE Catch2::Catch2WithMain pine_core)

add_executable(log_test ${PINE_TEST_DIR}/log_test.cpp)
target_link_libraries(log_test PRIVATE Catch2::Catch2WithMain pine_log)


# file(GLOB_RECURSE PINE_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*/*test.cpp")

# foreach(pine_test_source ${PINE_TEST_SOURCES})
#   get_filename_component(pine_test_filename ${pine_test_source} NAME)
#   string(REPLACE ".cpp" "" pine_test_name ${pine_test_filename})

#   # add the test target separatly
#   add_executable(${pine_test_name} )
  
# endforeach()
