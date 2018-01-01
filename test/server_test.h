//
// Created by alston on 12/15/17.
//

#include <cxxtest/TestSuite.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
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
        auto img_buffer = new char[512 * 1024];
        for (unsigned int i = 0; i < 300; i++) {
            std::stringstream img_path;
            img_path << "images/" << std::setw(6) << std::setfill('0') << i + 1 << ".jpeg";
            std::ifstream img(img_path.str(), std::ifstream::binary);
            if (!img.is_open()) {
                fprintf(stderr, "Can not open image\n");
                fprintf(stderr, "%s\n", img_path.str().c_str());
                exit(-1);
            }
            img.read(img_buffer, 512 * 1024);
            if (!img.eofbit) {
                fprintf(stderr, "Image too large\n");
                exit(-1);
            }
            img.close();
            auto img_size = static_cast<size_t>(img.gcount());
            camera->send_mock_img(img_buffer, img_size, Skagway::current_time(), i);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 30));
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

        char buffer[sizeof(Skagway::KeepAliveS)];
        std::ofstream out("capture/KeepAliveS.cap", std::ofstream::binary);

        this->server->start_loop();
        TS_ASSERT_EQUALS(client.send_keep_alive(), true);
        TS_ASSERT_EQUALS(client.expect_keep_alive_response(buffer), true);

        if (out.is_open()) {
            out.write(buffer, sizeof(buffer));
        }
        this->server->stop_loop();

        out.close();

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
            if (cur_frame_seq == 123) {
                uint16_t frag_seq = ntohs(packet->cur_frag);
                std::stringstream cap_path;
                cap_path << "capture/" << frag_seq << ".cap";
                std::ofstream out(cap_path.str(), std::ofstream::binary);

                if (out.is_open()) {
                    out.write((char *) buffer, rv);
                }

                out.close();
            }
            if (cur_frame_seq > frame_count) {
                frame_count = ntohl(packet->frame_seq);
            }
        }
        t.join();
        server->stop_loop();

        TS_ASSERT_LESS_THAN_EQUALS(270, frame_count);




    }
};
