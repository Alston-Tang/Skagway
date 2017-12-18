//
// Created by alston on 12/13/17.
//

#include <cxxtest/TestSuite.h>
#include <arpa/inet.h>
#include "../conn.h"
#include "../socket.h"

class ConnTest : public CxxTest::TestSuite
{
public:
    void testIPConn()
    {
        Skagway::Conn t1{}, t2{};
        Skagway::Socket *s = nullptr;
        uint32_t addr;
        inet_pton(AF_INET, "192.168.100.45", &addr);

        Skagway::IPConn(t1, s, addr, 80);

        TS_TRACE("Test default last_alive value");
        TS_ASSERT_EQUALS(t1.last_alive, 0);

        TS_TRACE("Test constructed sockaddr");
        sockaddr_in sock_addr{1,2,3,4,5};
        ssize_t rv = t1.get_sockaddr(&sock_addr, sizeof(sock_addr));
        TS_ASSERT_EQUALS(rv, sizeof(sockaddr_in));
        TS_ASSERT_EQUALS(sock_addr.sin_addr.s_addr, htonl(addr));
        TS_ASSERT_EQUALS(sock_addr.sin_port, htons(80));
        TS_ASSERT_EQUALS(sock_addr.sin_family, AF_INET);
        TS_ASSERT_EQUALS(*((uint64_t*)sock_addr.sin_zero), 0);

        TS_TRACE("Test equal operator");
        Skagway::IPConn(t2, s, addr, 80);
        TS_ASSERT_EQUALS(t1 == t2, true);
        TS_ASSERT_EQUALS(t1 != t2, false);
        Skagway::Conn &conn = t2;
        TS_ASSERT_EQUALS(t1 == conn, true);
        TS_ASSERT_EQUALS(t1 != conn, false)
        Skagway::Conn t3{};
        Skagway::IPConn(t3, s, addr, 81);
        TS_ASSERT_EQUALS(t1 == t3, false);
        TS_ASSERT_EQUALS(t1 != t3, true);

        TS_TRACE("Test valid");
        TS_ASSERT_EQUALS(t1.valid(), true);
        t1.set_invalid();
        TS_ASSERT_EQUALS(t1.valid(), false);

        TS_TRACE("Test copy");
        Skagway::IPConn(t1, s, addr, 80);
        t3 = t1;
        TS_ASSERT_EQUALS(t1 == t3, true);
    }
};