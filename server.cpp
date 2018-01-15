//
// Created by alston on 12/13/17.
//

#include <cstdlib>
#include <cstring>
#include "server.h"
#include "utility.h"
#include "protocol.h"

namespace Skagway {

void Server::init(const Skagway::Json &config) {
    auto &sockets = config["sockets"].array_items();

    size_t num_sockets = sockets.size();
    this->sockets_len = num_sockets;
    this->sockets = new Socket*[num_sockets];
    size_t  idx = 0;

    timeval socket_timeout{0, socket_total_wait_time / static_cast<int>(sockets_len)};


    for (auto &socket : sockets) {
        std::string type = socket["type"].string_value();
        unsigned short port = static_cast<unsigned short>(socket["port"].int_value());
        if (type == "ip") {
            auto *sock = new IPSocket(port);
            sock->set_recv_timeout(socket_timeout);
            this->sockets[idx] = sock;
        }
        idx++;
    }

    this->stop = true;
    this->main_loop_thread = nullptr;
}

bool Server::create_conn(Conn &conn) {
    size_t idx = invalid_idx;
    for (size_t i = 0; i < max_client; i++) {
        if (!this->connections[i].valid()) {
            idx = i;
            break;
        }
    }

    if (idx == invalid_idx) return false;

    this->connections[idx] = conn;
    this->refresh_conn(idx);
    return true;

}

void Server::close_conn(size_t conn_idx) {
    if (!this->connections[conn_idx].valid()) {
        //TODO Handle error
        fprintf(stderr, "Warn: close_conn: invalid connection\n");
    }

    this->connections[conn_idx].set_invalid();
}

void Server::refresh_conn(size_t conn_idx) {
    this->connections[conn_idx].last_alive = current_time();
}

size_t Server::find_conn(Conn &conn) {
    size_t idx = invalid_idx;
    for (size_t i = 0; i < max_client; i++) {
        if (this->connections[i] == conn) {
            idx = i;
            break;
        }
    }

    return idx;
}

Server::~Server() {
    for (int i = 0; i < sockets_len; i++) {
        delete this->sockets[i];
    }
}

void Server::send_img_all(void *data, size_t size, uint32_t time, uint32_t seq) const{
    auto *data_head = (uint8_t*)data;
    if (size == 0) return;

    auto total_frag = static_cast<uint16_t>((size - 1) / frag_max_size + 1);
    size_t offset = 0;

    uint8_t buffer[frag_max_size + sizeof(DataSegment)];
    auto *packet = (DataSegment *) &buffer;
    packet->frame_seq = htonl(seq);
    packet->time = htonl(time);
    packet->total_frag = htons(total_frag);

    for (uint16_t cur_frag = 0; cur_frag < total_frag; cur_frag++) {
        size_t data_size = offset + frag_max_size <= size ? frag_max_size : size - offset;

        memcpy(packet->data, data_head + offset, data_size);
        offset += data_size;
        packet->cur_frag = htons(cur_frag);

        for (auto &conn : this->connections) {
            if (!conn.valid()) continue;

            conn.socket->send_to(packet, data_size + sizeof(DataSegment), conn);
        }
    }
}

void Server::main_loop() {
    ssize_t rv;
    bool success;
    uint8_t buffer[recv_buffer_len];
    KeepAliveS response{};
    Conn recv_conn{};

    while (!this->stop) {
        for (int i = 0; i < sockets_len; i++) {
            Socket *cur_socket = this->sockets[i];
            bool no_packet = false;

            rv = cur_socket->recv_from(&buffer, recv_buffer_len, recv_conn);
            if (rv < 0) continue;

            if (rv != sizeof(KeepAliveC)) {
                //TODO Handle error
                fprintf(stderr, "Warn: main_loop: unexpected packet\n");
                continue;
            }

            size_t idx = find_conn(recv_conn);
            if (idx == invalid_idx) {
                success = create_conn(recv_conn);
                if (!success) {
                    //TODO Handle error
                    fprintf(stderr, "Warn: main_loop: too many connection\n");
                    continue;
                }
            } else {
                refresh_conn(idx);
            }


            auto *message = (KeepAliveC*) buffer;

            response.client_time = message->client_time;
            response.server_time = htonl(current_time());

            recv_conn.socket->send_to(&response, sizeof(KeepAliveS), recv_conn);
        }

        uint32_t now = current_time();
        for (size_t idx = 0; idx < max_client; idx++) {
            if (this->connections[idx].valid() && now - this->connections[idx].last_alive > client_expire_time) {
                close_conn(idx);
            }
        }
    }
}

Server::Server(Skagway::Json &config) {
    this->init(config);
}

Server::Server(const std::string &config_path) {
    Skagway::Json config;
    bool success;

    success = parse_config_file(config_path, config);
    if (!success) {
        //TODO Handle error
        fprintf(stderr, "Error: Server: Can not find config file\n");
        exit(-1);
    }

    this->init(config);
}

void Server::start_loop() {
    if (this->main_loop_thread != nullptr) return;

    this->stop = false;
    this->main_loop_thread = new std::thread(Server::run_loop, this);
}

void Server::stop_loop() {
    if (this->main_loop_thread == nullptr) return;

    this->stop = true;
    this->main_loop_thread->join();
    delete this->main_loop_thread;

    this->main_loop_thread = nullptr;
}

void Server::run_loop(Server *server) {
    server->main_loop();
}

}
