//
// Created by alston on 12/13/17.
//

#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include "conn.h"

namespace Skagway {


void Conn::construct(const Socket *socket, uint32_t last_alive) {
    this->socket = socket;
    this->last_alive = last_alive;
}

bool Conn::operator==(Conn &r) const{
    if (r.type != this->type) return false;
    return this->base_is_equal(*this, r);
}

bool Conn::operator!=(Conn &r) const{
    return !(*this == r);
}

ssize_t Conn::get_sockaddr(void *buf, size_t len) const{
    return this->base_get_sockaddr(*this, buf, len);
}

bool Conn::valid() const{
    return this->type != invalid;
}

bool Conn::set_invalid() {
    this->type = invalid;
}

Conn::Conn() {
    this->type = invalid;
}


/* IPConn */
ssize_t ip_get_sockaddr(const Conn &self, void *buf, size_t len) {
    if (len < sizeof(sockaddr_in)) return -1;

    auto *rv = (sockaddr_in*)buf;

    memset(rv, 0, sizeof(sockaddr_in));
    rv->sin_family = self.type;
    rv->sin_addr.s_addr = htonl(self.cont.IPCont.addr);
    rv->sin_port = htons(self.cont.IPCont.port);

    return sizeof(sockaddr_in);
}
bool ip_is_equal(const Conn &self, const Conn &r) {
    return self.type == r.type
            && self.socket == r.socket
            && self.cont.IPCont.port == r.cont.IPCont.port
            && self.cont.IPCont.addr == r.cont.IPCont.addr;
}
void IPConn(Conn &self, const Socket *socket, uint32_t addr, unsigned short port) {
    self.construct(socket, 0);

    static_assert(AF_INET != Conn::invalid);
    self.type = AF_INET;
    self.cont.IPCont.addr = addr;
    self.cont.IPCont.port = port;

    self.base_is_equal = ip_is_equal;
    self.base_get_sockaddr = ip_get_sockaddr;
}


}
