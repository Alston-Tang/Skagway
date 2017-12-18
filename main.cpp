#include <vector>
#include <iostream>
#include <fstream>
#include "server.h"
#include "utility.h"


int main() {
    Skagway::Json json;
    bool success = Skagway::parse_config_file("config.json", json);
    auto sockets = json["socketd"].array_items();

    std::cout << sockets.size();

    for (auto &item : sockets) {
        std::cout << item.type();
    }

    return 0;
}