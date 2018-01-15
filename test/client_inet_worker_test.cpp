//
// Created by alston on 1/6/18.
//



#include <unistd.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "../server.h"
#include "../utility.h"

int main() {
    Skagway::Server server("client_inet_worker_test_config.json");
    server.start_loop();

    uint32_t frame_seq = 0;
    int image_seq = 1;
    auto *buffer = new char[250000];
    std::ifstream image;
    std::stringstream file_name;

    while (true) {
        file_name.str("");
        file_name << "images/" << std::setw(6) << std::setfill('0') << image_seq << ".jpeg";
        image.open(file_name.str(), std::ifstream::binary);
        if (!image.is_open()) {
            std::cerr << "Can not open file " << file_name.str() << std::endl;
            break;
        }
        image.read(buffer, 250000);
        if (!image.eofbit) {
            std::cerr << "Image size is too large" << std::endl;
        }
        long file_len = image.gcount();
        if (file_len <= 0) {
            std::cerr << "Can not read data from file" << std::endl;
        }
        server.send_img_all(buffer, static_cast<size_t>(file_len), Skagway::current_time(), frame_seq);

        frame_seq++;
        image_seq = (image_seq % 700) + 1;
        image.close();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 30));
    }
    return 0;
}
