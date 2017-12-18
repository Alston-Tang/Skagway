//
// Created by alston on 12/14/17.
//

#include <cxxtest/TestSuite.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include "../socket.h"
#include "../utility.h"
#include "../server.h"

class SocketTest : public CxxTest::TestSuite
{
public:
    in_addr localhost{};

    void setUp() override{
        inet_aton("127.0.0.1", &localhost);
        localhost.s_addr = ntohl(localhost.s_addr);
    }

    void testIPSocket()
    {
        TS_TRACE("Test send_to and recv_from\n");

        unsigned short s1_port = 13642, s2_port = 13643;

        Skagway::IPSocket s1(s1_port);
        Skagway::IPSocket s2(s2_port);

        Skagway::Conn con_s1{}, con_s2{};
        Skagway::IPConn(con_s1, &s2, localhost.s_addr, s1_port);
        Skagway::IPConn(con_s2, &s1, localhost.s_addr, s2_port);

        Skagway::Conn recv_conn{};

        char data[] = "Hello World";
        char recv_data[20];
        ssize_t rv;

        rv = s1.send_to(data, sizeof(data), con_s2);
        TS_ASSERT_EQUALS(rv, sizeof(data));
        rv = s2.recv_from(recv_data, sizeof(recv_data), recv_conn);
        TS_ASSERT_EQUALS(rv, sizeof(data));
        TS_ASSERT_EQUALS(strcmp(data, recv_data), 0);
        TS_ASSERT_EQUALS(recv_conn.cont.IPCont.port, s1_port);
        TS_ASSERT_EQUALS(recv_conn.socket, &s2);
        TS_ASSERT_EQUALS(recv_conn.cont.IPCont.addr, localhost.s_addr);

        rv = s2.send_to(data, sizeof(data), con_s1);
        TS_ASSERT_EQUALS(rv, sizeof(data));
        rv = s1.recv_from(recv_data, sizeof(recv_data), recv_conn);
        TS_ASSERT_EQUALS(rv, sizeof(data));
        TS_ASSERT_EQUALS(strcmp(data, recv_data), 0);
        TS_ASSERT_EQUALS(recv_conn.cont.IPCont.port, s2_port);
        TS_ASSERT_EQUALS(recv_conn.socket, &s1);
        TS_ASSERT_EQUALS(recv_conn.cont.IPCont.addr, localhost.s_addr);



        TS_TRACE("Test socket copy\n");

        Skagway::IPSocket cp = s1;
        TS_ASSERT_DIFFERS(cp.get_fd(), s1.get_fd());
        rv = cp.send_to(data, sizeof(data), con_s2);
        TS_ASSERT_EQUALS(rv, sizeof(data));
        rv = s2.recv_from(recv_data, sizeof(recv_data), recv_conn);
        TS_ASSERT_EQUALS(rv, sizeof(data));

    }

    void testSetRecvTimeout() {
        char recv_data[20];
        Skagway::IPSocket s(58231);
        ssize_t rv;
        Skagway::Conn recv_conn;

        TS_TRACE("Test set_recv_timeout\n");
        timeval timeout{0, 200000};
        s.set_recv_timeout(timeout);

        timeval start_time{};
        timeval end_time{};

        uint32_t start = Skagway::current_time();
        rv = s.recv_from(recv_data, sizeof(recv_data), recv_conn);
        uint32_t end = Skagway::current_time();

        TS_ASSERT_LESS_THAN(rv, 0);
        TS_ASSERT_LESS_THAN(end - start, 300);
        TS_ASSERT_LESS_THAN(150, end - start);
    }

    void testCopy() {
        ssize_t rv;
        char data[] = "Hello World";
        char recv_data[20];
        Skagway::Conn recv_conn;
        Skagway::IPSocket receiver(18322);
        std::vector<Skagway::IPSocket> sockets;

        sockets.push_back(Skagway::IPSocket(14321));
        Skagway::Conn conn_receiver;
        Skagway::IPConn(conn_receiver, &sockets[0], localhost.s_addr, 18322);


        rv = sockets[0].send_to(data, sizeof(data), conn_receiver);
        TS_ASSERT_EQUALS(rv, sizeof(data));
        rv = receiver.recv_from(recv_data, 20, recv_conn);
        TS_ASSERT_EQUALS(rv, sizeof(data));
        TS_ASSERT_EQUALS(strcmp(data, recv_data), 0);


    }
};
