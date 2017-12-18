//
// Created by alston on 12/13/17.
//

#ifndef SKAGWAY_CONN_H
#define SKAGWAY_CONN_H

#include <cstdint>
#include <cstdio>
#include <bits/sockaddr.h>


namespace Skagway {

class Socket;

/*

class Conn {
public:
    uint32_t last_alive;
    const Socket *socket;

    explicit Conn(const Socket *socket, uint32_t last_alive = 0);
    virtual ssize_t get_sockaddr(void *buf, size_t len) = 0;
    virtual bool operator==(Conn &r) const = 0;
};

class IPConn : public Conn {
public:
    uint32_t addr;
    unsigned short port;

    IPConn(const Socket *socket, uint32_t addr, unsigned short port, uint32_t last_alive = 0);
    ssize_t get_sockaddr(void *buf, size_t len) override;
    bool operator==(Conn &r) const override;
};

class FakeConn : public Conn {
public:
    FakeConn();
    ssize_t get_sockaddr(void *buf, size_t len) override;
    bool operator==(Conn &r) const override;
};

*/

union ConnCont {
    struct {
        uint32_t addr;
        unsigned short port;
    } IPCont;
};


struct Conn {
    const static sa_family_t invalid = 0xfe63;

    Conn();
    void construct(const Socket *socket, uint32_t last_alive);
    bool operator==(Conn &r) const;
    bool operator!=(Conn &r) const;
    ssize_t get_sockaddr(void *buf, size_t len) const;
    bool valid() const;
    bool set_invalid();


    ssize_t (*base_get_sockaddr)(const Conn &self, void *buf, size_t len);
    bool (*base_is_equal)(const Conn &self, const Conn &r);
    sa_family_t type;
    uint32_t last_alive;
    const Socket *socket;
    ConnCont cont;
};

/* IPConn */
void IPConn(Conn &self, const Socket *socket, uint32_t addr, unsigned short port);

}

#endif //SKAGWAY_CONN_H
