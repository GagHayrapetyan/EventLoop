#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <functional>
#include <chrono>

#ifndef EVENTLOOP_EVENT_LOOP_H
#define EVENTLOOP_EVENT_LOOP_H

namespace el {

    struct CallBack {
        std::function<void()> func;
        unsigned int time;
    };

    class EventLoop {
    public:
        bool _running;
        unsigned int _callback_id_counter;
        std::map<unsigned int, CallBack> _callback_map;

        static unsigned int _now();

        unsigned int _callback_id();

        void _erase(unsigned int &id);

        void _loop();

    public:
        EventLoop();

        unsigned int setTimeout(unsigned int delay, std::function<void()> cb);

        unsigned int setInterval(unsigned int delay, std::function<void()> cb);

        void clearTimeout(unsigned int id);

        void clearInterval(unsigned int id);

        void start();

        void stop();
    };

}

#endif //EVENTLOOP_EVENT_LOOP_H
