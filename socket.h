//
// Created by alston on 12/13/17.
//

#ifndef SKAGWAY_SOCKET_H
#define SKAGWAY_SOCKET_H

#include <monetary.h>
#include "conn.h"

namespace Skagway {

class Socket {
protected:
    int fd;
    unsigned short port;

public:
    explicit Socket(unsigned short port);
    Socket(const Socket &cp);
    ~Socket();
    int set_recv_timeout(timeval &timeout);
    int get_fd();
    virtual ssize_t send_to(void *data, size_t len, const Conn &conn) const = 0;
    virtual ssize_t recv_from(void *data, size_t len, Conn &conn) const = 0;
};

class IPSocket : public Socket {
public:
    explicit IPSocket(unsigned short port);
    ssize_t send_to(void *data, size_t len, const Conn &conn) const override;
    ssize_t recv_from(void *data, size_t len, Conn &conn) const override;
};

}

#endif //SKAGWAY_SOCKET_H
