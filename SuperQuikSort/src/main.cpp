#include "mpi.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <memory.h>

using namespace std;


void Partion(int threadBase, int threadSize, int* pData, int nData){
    // 应该是每个线程都独立调用这个函数，所有每个线程的pData值和nData值都是不同的。
    // threadBase和threadSize都只是在做线程组的划分，用于告诉调用该线程的函数，自己属于那个线程组。

    if(threadSize == 1){
        // 该线程组中只有一个线程了，说明整个数组都已经都排序好了。
        return;
    }


    int num_threads; // 这个是总的线程数
    int rank; // 线程的编号
    MPI_Comm_size(MPI_COMM_WORLD, &num_threads);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

#ifdef LOG
    cout<<"rank: "<<rank<<", pData[1]: "<<pData[1]; // 用于验证，每个线程都会独立调用
#endif

    // 创建属于该线程组的通信器。
    int rankInGroup = rank - threadBase; // rankInGroup表示在线程组的中编号
    int ranks[threadSize];
    for(int i = 0; i < threadSize; i++){
        ranks[i] = threadBase + i;
    }

    MPI_Group world_group; 
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);
    
    MPI_Group localMPI_Group;
    MPI_Group_incl(world_group, threadSize, ranks, &localMPI_Group);

    MPI_Comm locaMPI_Comm;
    MPI_Comm_create(MPI_COMM_WORLD, localMPI_Group, &locaMPI_Comm);


#ifdef LOG
    cout<<"rank: "<<rank<<", rankInGroup: "<<rankInGroup<<endl;
#endif

    // 线程组中的0号（也就是threadBase）进行广播pivot
    int pivot;
    if(rankInGroup == 0){
        pivot = pData[nData / 2];
        MPI_Bcast(&pivot, 1, MPI_INT, threadBase, locaMPI_Comm); // 在线程组的广播空间中进行广播
    }
    else{
        MPI_Bcast(&pivot, 1, MPI_INT, threadBase, locaMPI_Comm);
    }

#ifdef LOG
    cout<<"rank: "<<rank<<", pivot: "<<pivot<<endl;
#endif

    // 根据收到的pivot进行分割。
    int* firstBigIndex = upper_bound(pData, pData + nData, pivot); // 

#ifdef LOG
    cout<<"rank: "<<rank<<", firstBigIndex: "<<*firstBigIndex<<endl;
#endif

    int lower_size = (firstBigIndex - pData) / sizeof(int);
    int upper_size = (nData - lower_size);

#ifdef LOG
    cout<<"rank: "<<rank<<", firstBigIndex: "<<*firstBigIndex<<endl;
#endif

    int* lower = (int*)malloc(lower_size * sizeof(int));
    int* upper = (int*)malloc(upper_size * sizeof(int));

    memcpy(lower, pData, lower_size);
    memcpy(upper, firstBigIndex, upper_size);

    // 清空pData中的内存
    free(pData);
    pData = NULL;

    // 进行节点间的数据交换与合并
    int step = threadSize / 2; // threadSize是2^n



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
        vec = (int*)malloc(length * sizeof(int));
        MPI_Bcast(vec, length, MPI_INT, 0, MPI_COMM_WORLD); // 接受0线程广播的vec
    }

    // [0, slice), [slice, 2 * slice), .... [rank * slice, (rank + 1) * slice)

    slice = length / num_threads; // num_threads 为 2^n，我的电脑只有4个。。。

    int* pData;
    pData = (int*)malloc(slice * sizeof(int));
    memcpy(pData, vec + (slice * rank), slice * sizeof(int));

    sort(pData, pData + slice);

#ifdef LOG
    if(rank == 0){
        cout<<"slice: "<<slice<<endl;
        cout<<"rank: "<<rank<<endl;
        for(int i = 0; i < slice; i++){
            cout<<"("<<i<<","<<pData[i]<<")";
        }
        cout<<endl;
    }
#endif

    if(rank != 0){
        free(vec);
        vec = NULL;
    }

    Partion(0, num_threads, pData, slice);


    MPI_Finalize();


    return 0;
}