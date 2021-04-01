#include "stopwatch.h"


void stopwatch::start(){
    start_time = std::chrono::system_clock::now();
}

void stopwatch::end(){
    end_time = std::chrono::system_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " ms"<<std::endl;
}