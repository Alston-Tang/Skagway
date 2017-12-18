//
// Created by alston on 12/18/17.
//

#include <cstring>
#include "mock_camera.h"
#include "../utility.h"

Skagway::Test::MockCamera::MockCamera(const Skagway::Server *server) {
    this->server = server;
}

bool Skagway::Test::MockCamera::send_mock_img(char content, uint32_t seq) {
    auto *data = new int8_t[default_img_size];
    memset(data, content, default_img_size);
    bool success = this->send_mock_img(data, default_img_size, current_time(), seq);

    delete[] data;
    return success;
}

bool Skagway::Test::MockCamera::send_mock_img(void *data, size_t size, uint32_t time, uint32_t seq) {
    this->server->send_img_all(data, size, time, seq);
    return true;
}
