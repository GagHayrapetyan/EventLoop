cmake_minimum_required(VERSION 3.20)

project(JsEventLoop)

option(AS_LIB "Enable if you want to use it as a library" OFF)
if (AS_LIB)
    add_library(JsEventLoop event_loop.h event_loop.cpp)
else ()
    add_executable(JsEventLoop event_loop.h event_loop.cpp example.cpp)
endif (AS_LIB)

set_target_properties(JsEventLoop PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)