//
// Created by alston on 12/16/17.
//

#ifndef SKAGWAY_FAKE_CLIENT_H
#define SKAGWAY_FAKE_CLIENT_H

#include "../socket.h"


namespace Skagway {
namespace Test {

class FakeClient {
public:
    const Socket *socket;
    Conn server_conn;
    
    FakeClient(const Socket *socket, Conn &server_conn);
    
    bool send_keep_alive();
    bool send_keep_alive(uint32_t value);

    bool expect_keep_alive_response();
    bool expect_keep_alive_response(void *buffer);
    
    ssize_t recv(void *buf, size_t len);
};

}
}

#endif //SKAGWAY_FAKE_CLIENT_H
