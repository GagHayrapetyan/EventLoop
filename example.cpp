//
// Created by gag on 09.08.21.
//

#include <iostream>
#include "event_loop.h"

int main() {
    el::EventLoop e;

    e.setTimeout(2,[](){
        std::cout<<"xxx"<<std::endl;
    });

    e.setInterval(100,[](){
        std::cout<<"yyy"<<std::endl;
    });

    for(auto &i: e._callback_map){
        std::cout<<i.first<<std::endl;
    }

    e._callback_map[0].func();
    e._callback_map[1].func();

    std::cout<<"**********************"<<std::endl;

    for(auto &i: e._callback_map){
        std::cout<<i.first<<std::endl;
    }

    e._callback_map[1].func();
    e.setTimeout(2,[](){
        std::cout<<"xxx"<<std::endl;
    });

    e._callback_map[1].func();

    e._callback_map[2].func();

    for(auto &i: e._callback_map){
        std::cout<<i.first<<std::endl;
    }

    e.clearInterval(1);

    for(auto &i: e._callback_map){
        std::cout<<i.first<<std::endl;
    }
}