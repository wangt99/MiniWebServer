add_executable(echo_client echo_client.cpp)
target_link_libraries(echo_client PRIVATE pine_core)

add_executable(echo_server echo_server.cpp)
target_link_libraries(echo_server PRIVATE pine_core)