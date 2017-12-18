//
// Created by alston on 12/13/17.
//

#ifndef SKAGWAY_PROTOCOL_H
#define SKAGWAY_PROTOCOL_H

namespace Skagway {

#pragma pack(1)

struct KeepAliveC {
    uint32_t client_time;
};

struct KeepAliveS {
    uint32_t client_time;
    uint32_t server_time;
};

struct DataSegment {
    uint32_t time;
    uint32_t frame_seq;
    uint16_t total_frag;
    uint16_t cur_frag;
    unsigned char data[];
};

#pragma pack()
}

#endif //SKAGWAY_PROTOCOL_H
