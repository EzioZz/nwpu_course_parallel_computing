// 只是在第一层上派生除了线程。没有价值

#include <fstream>
#include <iostream>
#include <string.h>
#ifndef MPICH_SKIP_MPICXX
#define MPICH_SKIP_MPICXX
#endif
#include <mpi.h>

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
#define N 8

void outPut(const int &size, int *array, ofstream &file) //输出到文件
{
  for (int i = 0; i < size; i++) {
    file << array[i] << " ";
  }
  file << endl;
}

bool isContact(const int &deep,
               const int *const &array) //判断对角线上是否有冲突
{
  int temp;
  for (int i = 1; i < deep + 1; i++) {
    temp = array[deep - i];
    if (array[deep] - i == temp || array[deep] + i == temp) //这条语句完成判断
      return true;
  }
  return false;
}

void range(const int &size, const int &deep, int *const &flags, int *&array,
           int &count, ofstream &file) //进行递归推导
{
  for (int i = 0; i < size;
       i++) //从第到第个，判断是否还有没用过并且没有冲突的数据
  {
    if (!flags[i]) //判断是否被用过，这里使用的是按内容寻址的方式
    {

      array[deep] = i; //如果i没有被使用过，那么现在使用i
      if (deep != 0) //不是第一行的元素要判断对角线上是否有冲突
      {
        if (isContact(deep, array)) //判断对角线是否有冲突，主要deep是层次
          continue; //如果有冲突，继续循环，寻找下一个试探点
      }
      flags[i] = 1; //目前第i个点可用，这里进行标记，第i个点已经被占用了
      if (deep == size - 1) //当深度为，就是找到了一个序列，满足八皇后要求了
      {
        outPut(size, array, file); //将结果输出到文件
        count++;                   //次数加一
      } else                       //没有找全所有的棋子
      {
        range(size, deep + 1, flags, array, count,
              file); //进一步递归调用，完成没完成的棋子
        array[deep] = -1; //递归回来，要恢复原状，以备下次继续递归到新的序列
      }
      flags[i] = 0; //恢复标志
    }
  }
}

void mpi_range(const int &size, int *&flags, int *&array,
               const int &myId) // mpi开启递归调用
{
  ofstream file;
  file.open("temp.txt",
            std::ios::out |
                std::ios::app); //输出的文件，每个进程都以追加的方式存放数据
  flags = new int[N]; //两个数据结构，flags用来存储位置是否被占用信息
  array = new int[N];                //存储第i行的棋子放的位置
  memset(flags, 0, sizeof(int) * N); //赋初值
  memset(array, -1, sizeof(int) * N);
  flags[myId] = 1; //第i个进程执行第一行为i的计算
  array[0] = myId;

  int count = 0;      //计数为
  int totalCount = 0; //总计数为
  if (myId < 8)
    range(N, 1, flags, array, count, file); //开始递归
  MPI_Reduce(&count, &totalCount, 1, MPI_INT, MPI_SUM, 0,
             MPI_COMM_WORLD); //规约所有递归结构
  if (myId == 0)              //主进程输出计算结果
    cout << totalCount << endl;
  file.close();
}

int main(int argc, char *argv[]) {
  int size;
  int myId;
  ofstream file;
  MPI_Init(&argc, &argv);               //初始化mpi
  MPI_Comm_size(MPI_COMM_WORLD, &size); //获取开启的进程数量
  MPI_Comm_rank(MPI_COMM_WORLD, &myId); //获取当前进程的id号

  int *flags; //标记
  int *array; //存放棋盘次序
  file.open(
      "temp.txt"); //用来清空文件--采用一次打开然后关闭文件，仅仅是在计算前，清空文件中原有的内容，后面文件打开均采用追加的方式
  file.close();
  mpi_range(N, flags, array, myId); //开始递归计算
  MPI_Finalize();                   //计算终止
  return 0;
}
