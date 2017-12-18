//
// Created by alston on 12/15/17.
//

#include <cxxtest/TestSuite.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../utility.h"
#include "../server.h"
#include "../protocol.h"
#include "fake_client.h"
#include "mock_camera.h"

timeval client_timeout;
in_addr localhost;

class ServerTest : public CxxTest::TestSuite
{
    static void testSendImgAllCameraThread(Skagway::Test::MockCamera *camera) {
        for (int i = 0; i < 10; i++) {
            camera->send_mock_img(static_cast<char>('a' + i), static_cast<uint32_t>(i));
            sleep(1);
        }
    }
    static void testSendImgAllMaxRes(Skagway::Test::MockCamera *camera) {
        std::ifstream img_file("maxresdefault.jpg", std::ifstream::binary);
        long img_size;
        auto *img = new char[512*1024];

        img_file.read(img, 512 * 1024);
        img_size = img_file.gcount();

        camera->send_mock_img(img, static_cast<size_t>(img_size), Skagway::current_time(), 142345);

        delete[] img;
    }
public:
    Skagway::Server *server = nullptr;

    void setUp() override {
        inet_aton("127.0.0.1", &localhost);
        this->server = new Skagway::Server("test_config_1.json");
        //2s timeout
        client_timeout.tv_sec = 2;
        client_timeout.tv_usec = 0;
    }

    void tearDown() override {
        delete this->server;
        this->server = nullptr;
    }

    void testServerStartStop() {
        Skagway::IPSocket client_socket(25832);
        client_socket.set_recv_timeout(client_timeout);
        Skagway::Conn conn_server1{};
        Skagway::IPConn(conn_server1, &client_socket, htonl(localhost.s_addr), 12345);
        Skagway::Test::FakeClient client(&client_socket, conn_server1);

        this->server->start_loop();
        TS_ASSERT_EQUALS(client.send_keep_alive(), true);
        TS_ASSERT_EQUALS(client.expect_keep_alive_response(), true);
        this->server->stop_loop();

        TS_ASSERT_EQUALS(client.send_keep_alive(), true);
        TS_ASSERT_EQUALS(client.expect_keep_alive_response(), false);
    }

    void testKeepAliveResponse() {
        Skagway::IPSocket client_socket(25832);
        client_socket.set_recv_timeout(client_timeout);
        Skagway::Conn conn_server1{};
        Skagway::IPConn(conn_server1, &client_socket, htonl(localhost.s_addr), 12345);
        Skagway::Test::FakeClient client(&client_socket, conn_server1);

        uint32_t send_time = Skagway::current_time();
        Skagway::KeepAliveS response{};

        this->server->start_loop();
        TS_ASSERT_EQUALS(client.send_keep_alive(send_time), true);
        TS_ASSERT_EQUALS(client.expect_keep_alive_response(&response), true);
        TS_ASSERT_EQUALS(ntohl(response.client_time), send_time);
        this->server->stop_loop();
    }

    void testMaxConnection() {
        unsigned short base_port = 25833;
        std::vector<Skagway::IPSocket> sockets;
        std::vector<Skagway::Test::FakeClient> clients;
        for (int i = 0; i < MAX_CLIENT; i++) {
            sockets.push_back(Skagway::IPSocket(static_cast<unsigned short>(base_port + i)));
            Skagway::Conn conn;
            Skagway::IPConn(conn, &sockets[0], htonl(localhost.s_addr), 12345);
            clients.push_back(Skagway::Test::FakeClient(&sockets[i], conn));
        }

        Skagway::IPSocket client_socket(25832);
        client_socket.set_recv_timeout(client_timeout);
        Skagway::Conn conn_server1{};
        Skagway::IPConn(conn_server1, &client_socket, htonl(localhost.s_addr), 12345);
        Skagway::Test::FakeClient client(&client_socket, conn_server1);


        this->server->start_loop();
        for (int i = 0; i < MAX_CLIENT; i++) {
            clients[i].send_keep_alive();
            TS_ASSERT_EQUALS(clients[i].expect_keep_alive_response(), true);
        }
        client.send_keep_alive();
        TS_ASSERT_EQUALS(client.expect_keep_alive_response(), false);
        this->server->stop_loop();
    }

    void testSendImgAll() {
        Skagway::Test::MockCamera camera(this->server);

        std::thread t(testSendImgAllCameraThread, &camera);

        Skagway::IPSocket client_socket(25832);
        client_socket.set_recv_timeout(client_timeout);
        Skagway::Conn conn_server1{};
        Skagway::IPConn(conn_server1, &client_socket, htonl(localhost.s_addr), 12345);
        Skagway::Test::FakeClient client(&client_socket, conn_server1);

        uint8_t buffer[1500];
        auto *packet = (Skagway::DataSegment*)&buffer;
        ssize_t rv;

        server->start_loop();

        client.send_keep_alive();

        int frame_count = -1;


        while (true) {
            rv = client.recv(buffer, 1500);
            if (rv < 0) break;
            else if (rv <= sizeof(Skagway::KeepAliveS)) continue;
            auto cur_frame_seq = static_cast<int>(ntohl(packet->frame_seq));
            if (cur_frame_seq > frame_count) {
                frame_count = ntohl(packet->frame_seq);
            }
        }
        t.join();
        server->stop_loop();

        TS_ASSERT_LESS_THAN_EQUALS(9, frame_count);




    }
};
