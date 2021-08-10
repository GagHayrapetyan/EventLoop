#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <functional>
#include <chrono>
#include <condition_variable>
#include <thread>

#ifndef EVENTLOOP_EVENT_LOOP_H
#define EVENTLOOP_EVENT_LOOP_H

#define DAY_TO_MS 86'400'000
#define HOURS_TO_MS 3'600'000
#define MIN_TO_MS 60'000
#define SEC_TO_MS 1'000

namespace el {

    class EventLoop {
    public:
        typedef unsigned int id_type;
        typedef unsigned long time_type;

        enum class TimeGroupsEnum {
            SEC, MIN, HOURS, DAY
        };

        struct TimeGroups {
            time_type time;
            std::list<id_type> list;
        };

        struct CallBack {
            std::function<void()> func;
            time_type time;
        };

        std::mutex _cv_m;
        std::condition_variable _cv;
        bool _running;
        id_type _callback_id_counter;
        std::map<id_type, CallBack> _callback_map;
        std::map<TimeGroupsEnum, TimeGroups> _time_groups;

        static time_type _now();

        id_type _callback_id();

        static TimeGroupsEnum _time_group_type(time_type delay);

        void _erase_from_time_group(const TimeGroupsEnum &time_group_type, const id_type &id);

        void _insert_to_time_group(const TimeGroupsEnum &time_group_type, const id_type &id);

        void _update_time_group(const TimeGroupsEnum &time_group_type, const time_type &now, time_type max_time);

        void _erase(id_type &id);

        void _insert(id_type &id, time_type &delay, std::function<void()> cb);

        void _loop();

    public:
        EventLoop();

        /**
         * Raise an event after the delay expires and death.
         * @param delay Timeout delay in ms
         * @param cb Callback function
         * @return event id
         */
        id_type setTimeout(time_type delay, std::function<void()> cb);

        /**
         * Raise the event after the delay has elapsed. After the delay expires, the counter is reset to zero.
         * @param delay Timeout delay in ms
         * @param cb Callback function
         * @return event id
         */
        id_type setInterval(time_type delay, std::function<void()> cb);

        /**
         * Remove an event from the list.
         * @param id event id
         */
        void clearTimeout(id_type id);

        /**
         * Remove an event from the list.
         * @param id event id
         */
        void clearInterval(id_type id);

        /**
         * Start event loop
         */
        void start();

        /**
         * Stop event loop
         */
        void stop();
    };

}

#endif //EVENTLOOP_EVENT_LOOP_H
