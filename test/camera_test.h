#include <cxxtest/TestSuite.h>
#include <arpa/inet.h>
#include "../conn.h"
#include "../socket.h"
#include "../server.h"
#include "../camera.h"

class ConnTest : public CxxTest::TestSuite
{
private:
    const static uint16_t vid = 0x058f;
    const static uint16_t pid = 0x0362;
public:
    void testCamera()
    {
        Skagway::Server server("test_config_1.json");
        Skagway::Camera camera(&server, vid, pid, UVC_FRAME_FORMAT_MJPEG, 1280, 720, 30);

        server.start_loop();
        camera.start_streaming();
    }
};