//
// Created by alston on 12/15/17.
//

#ifndef SKAGWAY_UTILITY_H
#define SKAGWAY_UTILITY_H

#include <fstream>
#include "json11.hpp"

namespace Skagway {

typedef json11::Json Json;

uint32_t current_time();

uint32_t time_val_to_time(timeval time);

bool parse_config_file(const std::string &file_path, json11::Json &json);

}
#endif //SKAGWAY_UTILITY_H
