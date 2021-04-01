#include <iostream>
#include "cal_PI.h"
#include "stopwatch.h"

int main(int argc, char** argv){

    int count_time = 0;
    if(argc == 2){
        count_time = atoi(argv[1]);
    }


    stopwatch watch;
    watch.start();
    cal_PI(argc, argv, count_time);
    watch.end();

    return 0;
}