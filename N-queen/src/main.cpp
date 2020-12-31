#include "n-queen.h"
#include "stopwatch.h"
#include <iostream>

int main(int argc, char *argv[]) {

  stopwatch watch;
  int res;

  int n_queen = 13;
  std::cout<<"棋盘的大小为:"<<n_queen<<std::endl;
  res = ParallelNQueen(n_queen, 2);
  for(int i = 1; i <= 8; i++){
    std::cout<<"线程个数为"<<i<<"；时间为:";
    watch.start();
    res = ParallelNQueen(n_queen, i);
    watch.end();
  }


  watch.start();
  res = SerialNQueen(n_queen);
  watch.end();

  std::cout<<res<<std::endl;

  return 0;
}
