//
// Created by tang on 1/15/18.
//

#include "camera.h"

bool Skagway::Camera::init() {
    if (Camera::uvc_ctx == nullptr) {
        this->set_ctx();
    }

    uvc_error_t res;

    res= uvc_find_device(Camera::uvc_ctx, &this->dev, this->vid, this->pid, nullptr);
    if (res < 0) {
        //TODO Handle Error
        uvc_perror(res, "uvc_find_device");
        return false;
    }

    res = uvc_open(this->dev, &this->dev_handle);
    if (res < 0) {
        //TODO Handle Error
        uvc_perror(res, "uvc_open");
        return false;
    }

    return this->set_format(this->format, this->width, this->height, this->fps);

}

bool Skagway::Camera::set_ctx() {
    uvc_error_t res = uvc_init(&Camera::uvc_ctx, nullptr);
    if (res < 0) {
        //TODO Handle Error
        uvc_perror(res, "init_ctx");
        return false;
    }

    return true;
}


void Skagway::Camera::cb(uvc_frame_t *frame, void *ptr) {
    auto *server = (Server*)ptr;

    uint32_t frame_seq = frame->sequence;
    uint32_t time = time_val_to_time(frame->capture_time);

    server->send_img_all(frame->data, frame->data_bytes, time, frame_seq);
}

Skagway::Camera::Camera(const Server *server, int vid, int pid, uvc_frame_format format, int width, int height, int fps) {
    this->server = server;
    this->vid = vid;
    this->pid = pid;
    this->format = format;
    this->width = width;
    this->height = height;
    this->fps = fps;

    this->init();
}

Skagway::Camera::~Camera() {
    uvc_close(this->dev_handle);
    uvc_unref_device(this->dev);
}

bool Skagway::Camera::set_format(uvc_frame_format format, int width, int height, int fps) {
    uvc_error_t res;

    res = uvc_get_stream_ctrl_format_size(this->dev_handle, &ctrl, this->format, this->width, this->height, this->fps);
    if (res < 0) {
        //TODO Handle Error
        uvc_perror(res, "get_mode");
        return false;
    }

    return true;
}

bool Skagway::Camera::start_streaming() {
    uvc_start_streaming(this->dev_handle, &this->ctrl, Camera::cb, const_cast<void*>(this->server), 0);
}

bool Skagway::Camera::stop_streaming() {
    uvc_stop_streaming(this->dev_handle);
}

