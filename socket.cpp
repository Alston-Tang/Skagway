//
// Created by alston on 12/13/17.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstdlib>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#include "socket.h"

Skagway::Socket::Socket(unsigned short port) {
    this->port = port;
}


Skagway::Socket::Socket(const Skagway::Socket &cp) {
    this->port = cp.port;
    this->fd = dup(cp.fd);
}

Skagway::Socket::~Socket() {
    int rv = close(this->fd);
    if (rv != 0) {
        //TODO Handle error
        fprintf(stderr, "Error: close: %d\n", errno);
        throw errno;
    }
}

int Skagway::Socket::set_recv_timeout(timeval &timeout) {
    int rv = setsockopt(this->fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeval));
    if (rv != 0) {
        //TODO Handle error
        fprintf(stderr, "Error: setsockopt: %d\n", errno);
        throw errno;
    }
}

int Skagway::Socket::get_fd() {
    return this->fd;
}

Skagway::IPSocket::IPSocket(unsigned short port) : Socket(port) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        //TODO Handle error
        fprintf(stderr, "Error: socket: %d\n", errno);
        throw errno;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    ssize_t rv = bind(fd, (sockaddr*)&addr, sizeof(addr));
    if (rv != 0) {
        //TODO Handle error
        fprintf(stderr, "Error: bind: %d\n", errno);
        throw errno;
    }

    //TODO Set timeout

    this->fd = fd;
}

ssize_t Skagway::IPSocket::send_to(void *data, size_t len, const Skagway::Conn &conn) const {
    sockaddr_in addr{};

    ssize_t rv = conn.get_sockaddr(&addr, sizeof(addr));
    if (rv < 0) {
        //TODO Handle error
        fprintf(stderr, "Error: get_sockaddr: %d\n", errno);
        throw errno;
    }

    auto addr_len = static_cast<socklen_t >(rv);
    rv = sendto(this->fd, data, len, MSG_DONTWAIT, (sockaddr*)&addr, addr_len);
    if (rv < 0) {
        //TODO Handle error
        fprintf(stderr, "Error: sendto: %d\n", errno);
        throw errno;
    }

    return rv;
}

ssize_t Skagway::IPSocket::recv_from(void *data, size_t len, Skagway::Conn &conn) const {
    sockaddr_in addr{};
    socklen_t addr_len = sizeof(addr);

    ssize_t rv = recvfrom(this->fd, data, len, 0, (sockaddr*)&addr, &addr_len);
    if (rv < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return rv;
        //TODO Handle error
        fprintf(stderr, "Error: recvfrom: %d\n", errno);
        throw errno;
    }
    if (addr_len != sizeof(addr)) {
        //TODO Handle error
        fprintf(stderr, "Error: recvfrom: %d\n", errno);
        throw errno;
    }

    IPConn(conn, this, ntohl(addr.sin_addr.s_addr), ntohs(addr.sin_port));

    return rv;
}
