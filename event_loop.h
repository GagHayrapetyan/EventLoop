#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <functional>
#include <chrono>

#ifndef EVENTLOOP_EVENT_LOOP_H
#define EVENTLOOP_EVENT_LOOP_H

namespace el {


    class EventLoop {
    public:
        typedef unsigned int id_type;
        typedef unsigned int time_type;

        enum class TimeGroupsEnum {
            SEC, MIN, HOURS, DAY
        };

        struct TimeGroups {
            std::pair<time_type, std::list<id_type>> sec;
            std::pair<time_type, std::list<id_type>> min;
            std::pair<time_type, std::list<id_type>> hours;
            std::pair<time_type, std::list<id_type>> day;
        };

        struct CallBack {
            std::function<void()> func;
            time_type time;
        };

        bool _running;
        id_type _callback_id_counter;
        std::map<id_type, CallBack> _callback_map;
        TimeGroups _time_groups;
        time_type _min_timer;

        static time_type _now();

        id_type _callback_id();

        static TimeGroupsEnum _time_group_type(time_type &delay);

        void _erase_from_time_group(const TimeGroupsEnum &time_group_type, const id_type &id);

        void _insert_to_time_group(const TimeGroupsEnum &time_group_type, const id_type &id);

        void _erase(id_type &id);

        void _insert(id_type &id, time_type &delay, std::function<void()> cb);

        void _loop();

    public:
        EventLoop();

        id_type setTimeout(time_type delay, std::function<void()> cb);

        id_type setInterval(time_type delay, std::function<void()> cb);

        void clearTimeout(id_type id);

        void clearInterval(id_type id);

        void start();

        void stop();
    };

}

#endif //EVENTLOOP_EVENT_LOOP_H
