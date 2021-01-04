#include "mpi.h"
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;


void Partion(int threadBase, int threadSize){
    int num_threads; // 这个是总的线程数
    int rank; // 线程的编号
    MPI_Comm_size(MPI_COMM_WORLD, &num_threads);
    MPI_Comm_rank()
}


int main(int argc, char** argv){

    MPI_Init(&argc, &argv);
    int num_threads, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &num_threads);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int length;
    int* vec;
    int slice;
    if(rank == 0){ // 要求1：进（线）程0负责读入和分发待排序数据。
        ifstream fin; fin.open("../in.txt", ios::in);
        fin>>length;
        MPI_Bcast(&length, 1, MPI_INT, 0, MPI_COMM_WORLD);
        vec = (int*)malloc(length * sizeof(int));
        for(int i = 0; i < length; i++){
            fin>>vec[i];
        }
        MPI_Bcast(vec, length, MPI_INT, 0, MPI_COMM_WORLD);
    }
    else{
        MPI_Bcast(&length, 1, MPI_INT, 0, MPI_COMM_WORLD);
        slice = length / num_threads; // num_threads 为 2^n，我的电脑只有4个。。。
        vec = (int*)malloc(slice * sizeof(int));
        MPI_Bcast(vec, length, MPI_INT, 0, MPI_COMM_WORLD); // 接受0线程广播的vec
    }


    // [0, slice), [slice, 2 * slice), .... [rank * slice, (rank + 1) * slice)
    // 

    sort(vec + rank * slice, vec + (rank + 1) * slice);
    int pivot;
    if(rank == 0){ // 选择主元，然后进行广播
        pivot = *(vec + slice / 2);
        MPI_Bcast(&pivot, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    else{
        MPI_Bcast(&pivot, 1, MPI_INT, 0, MPI_COMM_WORLD);




    }


    MPI_Finalize();


    return 0;
}