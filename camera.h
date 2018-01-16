//
// Created by tang on 1/15/18.
//

#ifndef SKAGWAY_CAMERA_H
#define SKAGWAY_CAMERA_H

#include <libuvc/libuvc.h>
#include "server.h"

namespace Skagway {
    class Camera {
        static uvc_context_t *uvc_ctx;

        const Skagway::Server *server;

        uvc_device_t *dev;
        uvc_device_handle_t *dev_handle;
        uvc_stream_ctrl_t ctrl;

        int vid;
        int pid;
        uvc_frame_format format;
        int width;
        int height;
        int fps;

        bool init();
        static bool set_ctx();

        static void cb(uvc_frame_t *frame, void *ptr);
    public:
        Camera(const Server *server, int vid, int pid, uvc_frame_format format, int width, int height, int fps);
        ~Camera();
        bool set_format(uvc_frame_format format, int width, int height, int fps);
        bool start_streaming();
        bool stop_streaming();
    };
}

#endif //SKAGWAY_CAMERA_H
