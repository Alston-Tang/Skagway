cmake_minimum_required(VERSION 3.5)
project(Skagway)

set(CMAKE_CXX_STANDARD 11)


set(ServerSource server.cpp server.h)
set(ConnSource conn.cpp conn.h)
set(SocketSource socket.cpp socket.h)
set(UtilitySource utility.cpp utility.h json11.cpp json11.hpp)
set(CameraSource camera.cpp camera.h)

add_library(Server ${ServerSource})
target_link_libraries(Server Socket Conn Utility pthread)

add_library(Socket ${SocketSource})
target_link_libraries(Socket Conn)

add_library(Conn ${ConnSource})

add_library(Utility ${UtilitySource})

add_library(Camera ${CameraSource})
target_link_libraries(Camera uvc)



add_executable(main main.cpp)
target_link_libraries(main uvc)
add_custom_command(
        TARGET main PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/config.json
        ${CMAKE_CURRENT_BINARY_DIR}/config.json)



find_package(CxxTest)
if(CXXTEST_FOUND)
    include_directories(${CXXTEST_INCLUDE_DIR})
    enable_testing()

    CXXTEST_ADD_TEST(conn_test conn_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/test/conn_test.h)
    target_link_libraries(conn_test Conn)

    CXXTEST_ADD_TEST(proto_test proto_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/test/proto_test.h)

    CXXTEST_ADD_TEST(socket_test socket_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/test/socket_test.h)
    target_link_libraries(socket_test Socket Utility)

    CXXTEST_ADD_TEST(utility_test utility_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/test/utility_test.h)
    target_link_libraries(utility_test Utility)

    CXXTEST_ADD_TEST(server_test server_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/test/server_test.h)
    target_link_libraries(server_test Server FakeClient MockCamera)
    add_custom_command(
            TARGET server_test PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/test/test_config_1.json
            ${CMAKE_CURRENT_BINARY_DIR}/test_config_1.json)
    add_custom_command(
            TARGET server_test PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/test/maxresdefault.jpg
            ${CMAKE_CURRENT_BINARY_DIR}/maxresdefault.jpg)
    add_custom_command(
            TARGET server_test PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/test/images/
            ${CMAKE_CURRENT_BINARY_DIR}/images/)
    add_custom_command(
            TARGET server_test PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
            ${CMAKE_CURRENT_BINARY_DIR}/capture)

    add_library(FakeClient test/fake_client.cpp test/fake_client.h)
    target_link_libraries(FakeClient Socket Utility)
    add_library(MockCamera test/mock_camera.cpp test/mock_camera.h)


    CXXTEST_ADD_TEST(camera_test camera_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/test/camera_test.h)
    target_link_libraries(camera_test Camera Server)

    add_executable(client_inet_worker_test test/client_inet_worker_test.cpp)
    target_link_libraries(client_inet_worker_test Server)
    add_custom_command(
            TARGET client_inet_worker_test PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/test/client_inet_worker_test_config.json
            ${CMAKE_CURRENT_BINARY_DIR}/client_inet_worker_test_config.json)



endif()
