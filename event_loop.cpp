#include "event_loop.h"


namespace el {

    EventLoop::EventLoop() : _running(false),
                             _callback_map(),
                             _callback_id_counter(0) {

    }

    unsigned int EventLoop::_now() {
        return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());
    }

    unsigned int EventLoop::_callback_id() {
        auto id = _callback_id_counter;
        _callback_id_counter++;

        return id;
    }

    void EventLoop::_erase(unsigned int &id) {
        _callback_map.erase(id);
    }

    unsigned int EventLoop::setTimeout(unsigned int delay, std::function<void()> cb) {
        auto id = _callback_id();
        auto func = [cb, id, this]() {
            cb();

            clearTimeout(id);
        };

        _callback_map.insert({id, {func, _now() + delay}});

        return id;
    }

    unsigned int EventLoop::setInterval(unsigned int delay, std::function<void()> cb) {
        auto id = _callback_id();
        auto func = [cb, id, delay, this]() {
            cb();
            _callback_map[id].time = _now() + delay;
        };

        _callback_map.insert({id, {func, _now() + delay}});

        return 0;
    }

    void EventLoop::clearTimeout(unsigned int id) {
        _erase(id);
    }

    void EventLoop::clearInterval(unsigned int id) {
        _erase(id);
    }

    void EventLoop::start() {
        if (_running) {
            return;
        }

        _running = true;
        _loop();
    }

    void EventLoop::stop() {
        _running = false;
    }



}