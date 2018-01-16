#include <unistd.h>
#include "server.h"
#include "camera.h"

int main() {
    const static uint16_t vid = 0x058f;
    const static uint16_t pid = 0x0362;

    Skagway::Server server("test_config_1.json");
    Skagway::Camera camera(&server, vid, pid, UVC_FRAME_FORMAT_MJPEG, 1280, 720, 30);

    server.start_loop();
    camera.start_streaming();

    sleep(6000);
}