#ifndef STOPWATCH_H
#define STOPWATCH_H

#include "stopwatch.h"
#include <chrono>
#include <iostream>


class stopwatch {
    typedef std::chrono::system_clock clock;
    clock::time_point start_time;
    clock::time_point end_time;
public:

    void start();

    void end();

};

#endif