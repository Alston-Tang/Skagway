//
// Created by alston on 12/18/17.
//

#ifndef SKAGWAY_MOCK_CAMERA_H
#define SKAGWAY_MOCK_CAMERA_H

#include "../server.h"

namespace Skagway {
namespace Test {

class MockCamera {
    const static size_t default_img_size = 1024 * 512;
    const Skagway::Server *server;
public:
    explicit MockCamera(const Skagway::Server *server);
    bool send_mock_img(char content, uint32_t seq);
    bool send_mock_img(void *data, size_t size, uint32_t time, uint32_t seq);
};

}
}

#endif //SKAGWAY_MOCK_CAMERA_H
