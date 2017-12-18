//
// Created by alston on 12/15/17.
//

#include <cstdint>
#include <ctime>
#include <fstream>
#include <sstream>
#include "utility.h"


namespace Skagway {

const static uint16_t ms_per_s = 1000;
const static uint32_t ns_per_ms = 1000000;

uint32_t current_time() {
    uint32_t rv;
    timespec time{};

    clock_gettime(CLOCK_REALTIME_COARSE, &time);

    // Truncate if value can't fit into uint32_t
    rv = (uint32_t)time.tv_sec * ms_per_s + (uint32_t)(time.tv_nsec / ns_per_ms);
    return rv;
}

bool parse_config_file(const std::string &file_path, Json &json) {
    Json rv;
    std::ifstream file(file_path);

    if (file.bad()) {
        return false;
    }
    std::stringstream s_stream;
    s_stream << file.rdbuf();

    std::string err;
    json = json.parse(s_stream.str(), err);

    if (!err.empty()) {
        //TODO Handle Error
        fprintf(stderr, "Error: json.parse: %s\n", err);
        return false;
    }

    // At least 1 socket
    auto &sockets = json["sockets"].array_items();
    if (sockets.empty()) {
        //TODO Handle Error
        fprintf(stderr, "Error: json.parse: no socket\n");
        return false;
    }
    
    // Each socket should at least has port and type set
    for (auto &socket : sockets) {
        if (socket["port"].int_value() == 0 || !socket["type"].is_string()) {
            //TODO Handle Error
            fprintf(stderr, "Error: json.parse: invalid socket\n");
            return false;
        }
    }

    return true;
}

}
