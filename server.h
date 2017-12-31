//
// Created by alston on 12/13/17.
//

#ifndef SKAGWAY_SERVER_H
#define SKAGWAY_SERVER_H


#include <cstdint>
#include <netinet/in.h>
#include <string>
#include <thread>
#include "conn.h"
#include "socket.h"

#define MAX_CLIENT 1

namespace Skagway {

class Server {
    static const unsigned char max_client = MAX_CLIENT;
    static const size_t invalid_idx = MAX_CLIENT;
    static const size_t frag_max_size = 1400;
    static const size_t recv_buffer_len = 1500;
    static const uint32_t ms_per_s = 1000;
    static const uint32_t us_per_ms = 1000;
    static const uint32_t client_expire_time = 10 * ms_per_s;
    static const uint32_t socket_total_wait_time = 200 * us_per_ms;

    static_assert(socket_total_wait_time < 1 * ms_per_s * us_per_ms, "socket_total_wait_time can not be longer than 1s");

    Conn connections[max_client];
    Socket **sockets;
    size_t sockets_len;
    bool stop;
    std::thread *main_loop_thread;

    bool create_conn(Conn &conn);
    void close_conn(size_t conn_idx);
    void refresh_conn(size_t conn_idx);
    size_t find_conn(Conn &conn);
    void main_loop();
    static void run_loop(Server *server);

public:
    explicit Server(const std::string &config_path);
    ~Server();
    void send_img_all(void *data, size_t size, uint32_t time, uint32_t seq) const;
    void start_loop();
    void stop_loop();

};

}

#endif //SKAGWAY_SERVER_H
