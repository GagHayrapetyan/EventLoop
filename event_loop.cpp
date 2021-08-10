#include "event_loop.h"

#include <utility>


namespace el {

    EventLoop::EventLoop() : _running(false),
                             _callback_map(),
                             _callback_id_counter(0),
                             _time_groups({{TimeGroupsEnum::SEC,   {0, {}}},
                                           {TimeGroupsEnum::MIN,   {0, {}}},
                                           {TimeGroupsEnum::HOURS, {0, {}}},
                                           {TimeGroupsEnum::DAY,   {0, {}}}}) {
    }

    EventLoop::time_type EventLoop::_now() {
        return static_cast<time_type>(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());
    }

    EventLoop::TimeGroupsEnum EventLoop::_time_group_type(time_type delay) {
        if (delay < MIN_TO_MS) {
            return TimeGroupsEnum::SEC;
        } else if (delay < HOURS_TO_MS) {
            return TimeGroupsEnum::MIN;
        } else if (delay < DAY_TO_MS) {
            return TimeGroupsEnum::HOURS;
        } else {
            return TimeGroupsEnum::DAY;
        }
    }

    void EventLoop::_erase_from_time_group(const EventLoop::TimeGroupsEnum &time_group_type,
                                           const id_type &id) {
        _time_groups[time_group_type].list.remove(id);
    }

    void EventLoop::_insert_to_time_group(const EventLoop::TimeGroupsEnum &time_group_type,
                                          const EventLoop::id_type &id) {
        auto list = _time_groups[time_group_type].list;
        auto it = std::begin(_time_groups[time_group_type].list);

        for (auto &i: list) {
            auto time = _callback_map[id].time;

            if (time < _callback_map[i].time) {
                _time_groups[time_group_type].list.insert(it, id);
                return;
            }

            ++it;
        }

        _time_groups[time_group_type].list.push_back(id);
    }

    void EventLoop::_update_time_group(const EventLoop::TimeGroupsEnum &time_group_type,
                                       const time_type &now,
                                       time_type max_time) {
        if (now - _time_groups[time_group_type].time > max_time) {
            auto list = _time_groups[time_group_type].list;

            for (auto &id: list) {
                auto type = _time_group_type(_callback_map[id].time - now);

                if (type != time_group_type) {
                    _erase_from_time_group(time_group_type, id);
                    _insert_to_time_group(type, id);
                }
            }

            _time_groups[time_group_type].time = now;
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
        _cv.notify_all();
    }

    void EventLoop::_insert(id_type &id, time_type &delay, std::function<void()> cb) {
        _callback_map.insert({id, {std::move(cb), _now() + delay}});
        _insert_to_time_group(_time_group_type(delay), id);
        _cv.notify_all();
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
            _erase_from_time_group(TimeGroupsEnum::SEC, id);
            _insert_to_time_group(_time_group_type(delay), id);
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
        std::unique_lock<std::mutex> lk(_cv_m);

        while (_running) {
            auto now = _now();

            _update_time_group(TimeGroupsEnum::DAY, now, DAY_TO_MS - 1000);
            _update_time_group(TimeGroupsEnum::HOURS, now, HOURS_TO_MS - 1000);
            _update_time_group(TimeGroupsEnum::MIN, now, MIN_TO_MS - 1'000);

            auto list = _time_groups[TimeGroupsEnum::SEC].list;
            auto list_front = list.front();
            for (auto &i: list) {
                if (_callback_map[i].time < now) {
                    _callback_map[i].func();
                }
            }

            auto wait = (_time_groups[TimeGroupsEnum::SEC].list.empty()) ?
                        MIN_TO_MS - 1'000 : _callback_map[list_front].time - now;

            _cv.wait_for(lk, std::chrono::milliseconds(wait), [this, list_front]() {
                auto id = _time_groups[TimeGroupsEnum::SEC].list.front();
                return _callback_map[id].time != _callback_map[list_front].time;
            });
        }
    }


}