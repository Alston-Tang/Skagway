//
// Created by alston on 12/16/17.
//

#include <netinet/in.h>
#include <cstring>
#include "fake_client.h"
#include "../protocol.h"
#include "../utility.h"

Skagway::Test::FakeClient::FakeClient(const Skagway::Socket *socket, Skagway::Conn &server_conn) {
    this->socket = socket;
    this->server_conn = server_conn;
}

bool Skagway::Test::FakeClient::send_keep_alive() {
    return this->send_keep_alive(current_time());
}

bool Skagway::Test::FakeClient::send_keep_alive(uint32_t value) {
    Skagway::KeepAliveC packet{};
    ssize_t rv;

    packet.client_time = htonl(value);
    rv = this->socket->send_to(&packet, sizeof(packet), this->server_conn);

    return rv == sizeof(packet);
}

ssize_t Skagway::Test::FakeClient::recv(void *buf, size_t len) {
    Conn recv_conn{};
    ssize_t rv;

    rv = this->socket->recv_from(buf, len, recv_conn);

    if (rv < 0) return rv;

    if (recv_conn != this->server_conn) {
        fprintf(stderr, "Warn: recv: Unknown source\n");
        return -1;
    }

    return rv;
}

bool Skagway::Test::FakeClient::expect_keep_alive_response() {
    uint8_t local_buffer[1500];
    ssize_t rv;

    do {
        rv = this->recv(&local_buffer, 1500);
        if (rv < 0) return false;
        if (rv == sizeof(Skagway::KeepAliveS)) return true;
    } while (true);

}

bool Skagway::Test::FakeClient::expect_keep_alive_response(void *buffer) {
    uint8_t local_buffer[1500];
    ssize_t rv;

    do {
        rv = this->recv(&local_buffer, 1500);
        if (rv < 0) return false;
        if (rv == sizeof(Skagway::KeepAliveS)) {
            memcpy(buffer, &local_buffer, sizeof(Skagway::KeepAliveS));
            return true;
        }
    } while (true);
}
