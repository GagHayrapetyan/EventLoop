#include "event_loop.h"

#include <utility>


namespace el {

    EventLoop::EventLoop() : _running(false),
                             _callback_map(),
                             _callback_id_counter(0),
                             _time_groups(),
                             _min_timer(INT32_MAX) {

    }

    EventLoop::time_type EventLoop::_now() {
        return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());
    }

    EventLoop::TimeGroupsEnum EventLoop::_time_group_type(time_type &delay) {
        if (delay < 60'000) {
            return TimeGroupsEnum::SEC;
        } else if (delay < 3'600'000) {
            return TimeGroupsEnum::MIN;
        } else if (delay < 86'400'000) {
            return TimeGroupsEnum::HOURS;
        } else {
            return TimeGroupsEnum::DAY;
        }
    }

    void EventLoop::_erase_from_time_group(const EventLoop::TimeGroupsEnum &time_group_type,
                                           const id_type &id) {
        switch (time_group_type) {
            case TimeGroupsEnum::SEC:
                _time_groups.sec.second.remove(id);
                break;
            case TimeGroupsEnum::MIN:
                _time_groups.min.second.remove(id);
                break;
            case TimeGroupsEnum::HOURS:
                _time_groups.hours.second.remove(id);
                break;
            case TimeGroupsEnum::DAY:
                _time_groups.day.second.remove(id);
                break;
            default:
                break;
        }
    }

    void EventLoop::_insert_to_time_group(const EventLoop::TimeGroupsEnum &time_group_type,
                                          const EventLoop::id_type &id) {
        auto list = _time_groups.sec.second;
        auto it = std::begin(_time_groups.sec.second);

        switch (time_group_type) {
            case TimeGroupsEnum::SEC:
                for (auto &i: list) {
                    auto time = _callback_map[id].time;

                    if (time < _callback_map[i].time) {
                        _time_groups.sec.second.insert(it, id);
                        _min_timer = time;
                        return;
                    }

                    ++it;
                }

                _time_groups.sec.second.push_back(id);
                break;
            case TimeGroupsEnum::MIN:
                _time_groups.min.second.push_back(id);
                break;
            case TimeGroupsEnum::HOURS:
                _time_groups.hours.second.push_back(id);
                break;
            case TimeGroupsEnum::DAY:
                _time_groups.day.second.push_back(id);
                break;
            default:
                break;
        }
    }

    EventLoop::id_type EventLoop::_callback_id() {
        auto id = _callback_id_counter;
        _callback_id_counter++;

        return id;
    }

    void EventLoop::_erase(id_type &id) {
        _callback_map.erase(id);
        _erase_from_time_group(TimeGroupsEnum::SEC, id);
        _erase_from_time_group(TimeGroupsEnum::MIN, id);
        _erase_from_time_group(TimeGroupsEnum::HOURS, id);
        _erase_from_time_group(TimeGroupsEnum::DAY, id);

        if (_callback_map.empty()) {
            _min_timer = INT32_MAX;
        }
    }

    void EventLoop::_insert(id_type &id, time_type &delay, std::function<void()> cb) {
        _min_timer = 1;
        _callback_map.insert({id, {std::move(cb), _now() + delay}});
        _insert_to_time_group(_time_group_type(delay), id);
    }

    EventLoop::id_type EventLoop::setTimeout(time_type delay, std::function<void()> cb) {
        auto id = _callback_id();
        auto func = [cb, id, this]() {
            cb();

            clearTimeout(id);
        };

        _insert(id, delay, func);

        return id;
    }

    EventLoop::id_type EventLoop::setInterval(time_type delay, std::function<void()> cb) {
        auto id = _callback_id();
        auto func = [cb, id, delay, this]() {
            cb();
            _callback_map[id].time = _now() + delay;
        };

        _insert(id, delay, func);

        return 0;
    }

    void EventLoop::clearTimeout(id_type id) {
        _erase(id);
    }

    void EventLoop::clearInterval(id_type id) {
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

    void EventLoop::_loop() {
        while (!_running) {
//            for ()
        }
    }


}