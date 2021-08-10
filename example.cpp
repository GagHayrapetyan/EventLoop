#include <iostream>
#include "event_loop.h"

int main() {
    el::EventLoop e;

    auto now = std::chrono::high_resolution_clock::now();

    auto event_1 = e.setTimeout(20000, [now]() {
        std::cout << "Timeout 1: " << std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - now).count() << std::endl;
    });

    auto event_2 = e.setInterval(6000, [now]() {
        std::cout << "Interval 1: " << std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - now).count() << std::endl;
    });

    auto event_3 = e.setInterval(3000, [now]() {
        std::cout << "Interval 2: " << std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - now).count() << std::endl;
    });

    e.setTimeout(10000, [&]() {
        e.clearInterval(event_3);
    });

    e.setTimeout(10, [&]() {
        e.clearTimeout(event_1);
    });


    e.start();

//    _callback_map[i]


}