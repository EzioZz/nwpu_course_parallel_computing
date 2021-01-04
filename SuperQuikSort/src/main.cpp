#include "mpi.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <memory.h>
#include <string>

using namespace std;

int* merge(int* pList0, int nList0, int* pList1, int nList1){
    int nRes = nList0 + nList1;
    int* pRes = (int*)malloc(nRes * sizeof(int));
    
    int p0 = 0;
    int p1 = 0;
    int count = 0;

    while(p0 < nList0 && p1 < nList1){
        if(pList0[p0] <= pList1[p1]){

            pRes[count] = pList0[p0];
            p0++;
            count++;
        }
        else{
            pRes[count] = pList1[p1];
            p1++;
            count++;
        }
    }

    while(p0 < nList0){
        pRes[count] = pList0[p0];
        p0++;
        count++;
    }

    while(p1 < nList1){
        pRes[count] = pList1[p1];
        p1++;
        count++;
    }

    

    return pRes;
}

int main(int argc, char** argv){


    MPI_Init(&argc, &argv);
    int num_threads, rank;

    double start_time = MPI_Wtime();

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
    int nData;
    pData = (int*)malloc(slice * sizeof(int));
    memcpy(pData, vec + (slice * rank), slice * sizeof(int));

    sort(pData, pData + slice);

#ifdef LOG
    cout<<"slice: "<<slice;
    cout<<"rank: "<<rank<<":";
    for(int i = 0; i < slice; i++){
        cout<<"("<<i<<","<<pData[i]<<") ";
    }
    cout<<endl;
#endif

    if(rank != 0){
        free(vec);
        vec = NULL;
    }

    nData = slice;
    
    // Partion(0, num_threads, pData, nData);
    // 换一种partion的方法
    int threadBase = 0;
    int threadSize = num_threads;

    while(threadSize > 1){

#ifdef LOG
        cout<<"threadSize: "<<threadSize<<endl;
        cout<<"rank: "<<rank<<", threadBase = "<<threadBase<<endl;
#endif

        // 
        int rankInGroup = rank - threadBase; // rankInGroup表示在线程组的中编号
        // int ranks[threadSize];
        // for(int i = 0; i < threadSize; i++){
        //     ranks[i] = threadBase + i;
        // }

        // MPI_Group world_group; 
        // MPI_Comm_group(MPI_COMM_WORLD, &world_group);
        
        // MPI_Group localMPI_Group;
        // MPI_Group_incl(world_group, threadSize, ranks, &localMPI_Group);

        // MPI_Comm localMPI_Comm;
        // MPI_Comm_create(MPI_COMM_WORLD, localMPI_Group, &localMPI_Comm);

        

#ifdef LOG
    cout<<"rank: "<<rank<<", rankInGroup: "<<rankInGroup<<endl;
#endif

        // 线程组中root(threadBase)进行广播pivot
        MPI_Status* status;
        int pivot;
        if(rank == threadBase){
            pivot = pData[nData / 2];
            // MPI_Bcast(&pivot, 1, MPI_INT, threadBase, localMPI_Comm); // 在线程组的广播空间中进行广播
            for(int i = 1; i < threadSize; i++){
                MPI_Send(&pivot, 1, MPI_INT, threadBase + i, 0, MPI_COMM_WORLD);
            }

        }
        else{
            // MPI_Bcast(&pivot, 1, MPI_INT, threadBase, localMPI_Comm);
            MPI_Recv(&pivot, 1, MPI_INT, threadBase, MPI_ANY_TAG, MPI_COMM_WORLD, status);
        }

#ifdef LOG
        cout<<"rank: "<<rank<<", pivot: "<<pivot<<endl;
#endif

        // 根据收到的pivot进行分割。
        int* firstBigIndex = upper_bound(pData, pData + nData, pivot); // 

        int nLower = (firstBigIndex - pData); // 两个指针相减自己会除sizeof(int)
        int nUpper = (nData - nLower);

#ifdef LOG
    cout<<"rank: "<<rank<<", firstBigIndex: "<<*firstBigIndex<<endl;
    cout<<"rank: "<<rank<<", firstBigIndex at location "<<(firstBigIndex - pData)<<endl;
    cout<<"rank: "<<rank<<", nLower = "<<nLower<<", nUpper = "<<nUpper<<endl;
#endif

        int* pLower = (int*)malloc(nLower * sizeof(int));
        int* pUpper = (int*)malloc(nUpper * sizeof(int));

        memcpy(pLower, pData, nLower * sizeof(int));
        memcpy(pUpper, firstBigIndex, nUpper * sizeof(int));

        // 清空pData中的内存
        free(pData);
        pData = NULL;
        nData = 0;

        // 进行节点间的数据交换与合并
        int step = threadSize / 2; // threadSize是2^n
        
        int* pNewLower;
        int nNewLower;

        int* pNewUpper;
        int nNewUpper;
        
        // 进行同一线程组内，上下两部分的交换和合并操作。
    
        if(rankInGroup < threadSize / 2){
    #ifdef LOG
            cout<<"small rank: "<<rank<<" want!! recv nNewLower =  "<<nNewLower<<", from "<<rank + step<<endl;
    #endif
            MPI_Recv(&nNewLower, 1, MPI_INT, rank + step, MPI_ANY_TAG, MPI_COMM_WORLD, status);
    #ifdef LOG
            cout<<"small rank: "<<rank<<" have!! recv nNewLower =  "<<nNewLower<<", from "<<rank + step<<endl;
    #endif
            pNewLower = (int*)malloc(sizeof(int) * nNewLower);
            MPI_Recv(pNewLower, nNewLower, MPI_INT, rank + step, MPI_ANY_TAG, MPI_COMM_WORLD, status);

            MPI_Send(&nUpper, 1, MPI_INT, rank + step, 0, MPI_COMM_WORLD);
            MPI_Send(pUpper, nUpper, MPI_INT, rank + step, 0, MPI_COMM_WORLD);

            // 这里进行 Lower和NewLower 的合并
            pData = merge(pLower, nLower, pNewLower, nNewLower);
            nData = nLower + nNewLower;
    #ifdef LOG
            cout<<"small rank: "<<rank<<", nData = "<<nData<<endl;
    #endif
        }
        else{
            MPI_Send(&nLower, 1, MPI_INT, rank - step, 0, MPI_COMM_WORLD);
    #ifdef LOG
            cout<<"big rank: "<<rank<<"send nLower =  "<<nLower<<", to "<<rank - step<<endl;
    #endif
            MPI_Send(pLower, nLower, MPI_INT, rank - step, 0, MPI_COMM_WORLD);

            MPI_Recv(&nNewUpper, 1, MPI_INT, rank - step, MPI_ANY_TAG, MPI_COMM_WORLD, status);
            pNewUpper = (int*)malloc(sizeof(int) * nNewUpper);
            MPI_Recv(pNewUpper, nNewUpper, MPI_INT, rank - step, MPI_ANY_TAG, MPI_COMM_WORLD, status);

            // 这里进行 Upper和NewUpper的合并
            pData = merge(pUpper, nUpper, pNewUpper, nNewUpper);
            nData = nUpper + nNewUpper;
    #ifdef LOG
            cout<<"big rank: "<<rank<<", nData = "<<nData<<endl;
    #endif
        }

    #ifdef LOG
        cout<<"partion结束，rank: "<<rank<<", nData = "<<nData<<endl;
    #endif

        // if(localMPI_Group != MPI_GROUP_NULL) MPI_Group_free(&localMPI_Group);
        // if(localMPI_Comm != MPI_COMM_NULL) MPI_Comm_free(&localMPI_Comm);

        // free(pLower); pLower = NULL;
        // free(pUpper); pUpper = NULL;
        // free(pNewLower); pNewLower = NULL;
        // free(pNewUpper); pNewUpper = NULL;

        if(rankInGroup < threadSize / 2){
            threadBase = threadBase;
        }
        else{
            threadBase = threadBase + threadSize /2 ;
        }

        threadSize /= 2;
        MPI_Barrier(MPI_COMM_WORLD);
        

#ifdef LOG
        if(rank == 0){
            cout<<endl<<endl<<"---------------------完成了一次迭代-----------------------"<<endl<<endl;
            cout.flush();
        }
        MPI_Barrier(MPI_COMM_WORLD);
#endif

    }
#ifdef LOG 
    cout<<"partion结束, 返回main函数"<<endl;
#endif


    // 按照线程rank的顺序进行输出
    // ofstream fout;
    // MPI_Status* status;

    // string s = "out";
    // string path = "../" + s + to_string(rank) + ".txt";
    // fout.open(path, ios::out);
    // cout<<"rank："<<rank<<"的输出, size = "<<nData<<endl;
    // for(int i = 0; i < nData; i++){
    //     cout<<pData[i]<<" ";
    // }
    // cout<<endl;

    double end_time = MPI_Wtime();

    cout<<"array length: "<<length<<endl;
    cout<<"cost time: "<<end_time - start_time<<"s"<<endl;

    MPI_Finalize();


    return 0;
}

// void Partion(int threadBase, int threadSize, int* &pData, int &nData){
//     // 应该是每个线程都独立调用这个函数，所有每个线程的pData值和nData值都是不同的。
//     // threadBase和threadSize都只是在做线程组的划分，用于告诉调用该线程的函数，自己属于那个线程组。
//     // threadBase为开始编号，threadSize为线程组中线程的个数。

//     if(threadSize == 1){
//         // 该线程组中只有一个线程了，说明整个数组都已经都排序好了。
//         return;
//     }


//     int num_threads; // 这个是总的线程数
//     int rank; // 线程的编号
//     MPI_Comm_size(MPI_COMM_WORLD, &num_threads);
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);

//     // 创建属于该线程组的通信器。
//     int rankInGroup = rank - threadBase; // rankInGroup表示在线程组的中编号
//     int ranks[threadSize];
//     for(int i = 0; i < threadSize; i++){
//         ranks[i] = threadBase + i;
//     }

//     MPI_Group world_group; 
//     MPI_Comm_group(MPI_COMM_WORLD, &world_group);
    
//     MPI_Group localMPI_Group;
//     MPI_Group_incl(world_group, threadSize, ranks, &localMPI_Group);

//     MPI_Comm localMPI_Comm;
//     MPI_Comm_create(MPI_COMM_WORLD, localMPI_Group, &localMPI_Comm);


// #ifdef LOG
//     cout<<"rank: "<<rank<<", rankInGroup: "<<rankInGroup<<endl;
// #endif

//     // 线程组中的0号（也就是threadBase）进行广播pivot
//     int pivot;
//     if(rankInGroup == 0){
//         pivot = pData[nData / 2];
//         MPI_Bcast(&pivot, 1, MPI_INT, threadBase, localMPI_Comm); // 在线程组的广播空间中进行广播
//     }
//     else{
//         MPI_Bcast(&pivot, 1, MPI_INT, threadBase, localMPI_Comm);
//     }

// #ifdef LOG
//     cout<<"rank: "<<rank<<", pivot: "<<pivot<<endl;
// #endif

//     // 根据收到的pivot进行分割。
//     int* firstBigIndex = upper_bound(pData, pData + nData, pivot); // 

//     int nLower = (firstBigIndex - pData); // 两个指针相减自己会除sizeof(int)
//     int nUpper = (nData - nLower);

// #ifdef LOG
//     cout<<"rank: "<<rank<<", firstBigIndex: "<<*firstBigIndex<<endl;
//     cout<<"rank: "<<rank<<", firstBigIndex at location "<<(firstBigIndex - pData)<<endl;
//     cout<<"rank: "<<rank<<", nLower = "<<nLower<<", nUpper = "<<nUpper<<endl;
// #endif

//     int* pLower = (int*)malloc(nLower * sizeof(int));
//     int* pUpper = (int*)malloc(nUpper * sizeof(int));

//     memcpy(pLower, pData, nLower);
//     memcpy(pUpper, firstBigIndex, nUpper);

//     // 清空pData中的内存
//     free(pData);
//     pData = NULL;
//     nData = 0;

//     // 进行节点间的数据交换与合并
//     int step = threadSize / 2; // threadSize是2^n
    
//     int* pNewLower;
//     int nNewLower;

//     int* pNewUpper;
//     int nNewUpper;
    
//     // 进行同一线程组内，上下两部分的交换和合并操作。
//     MPI_Status* status;
//     if(rankInGroup < threadSize / 2){
// #ifdef LOG
//         cout<<"small rank: "<<rank<<" want!! recv nNewLower =  "<<nNewLower<<", from "<<rank + step<<endl;
// #endif
//         MPI_Recv(&nNewLower, 1, MPI_INT, rank + step, 0, MPI_COMM_WORLD, status);
// #ifdef LOG
//         cout<<"small rank: "<<rank<<" have!! recv nNewLower =  "<<nNewLower<<", from "<<rank + step<<endl;
// #endif
//         pNewLower = (int*)malloc(sizeof(int) * nNewLower);
//         MPI_Recv(pNewLower, nNewLower, MPI_INT, rank + step, 0, MPI_COMM_WORLD, status);

//         MPI_Send(&nUpper, 1, MPI_INT, rank + step, 0, MPI_COMM_WORLD);
//         MPI_Send(pUpper, nUpper, MPI_INT, rank + step, 0, MPI_COMM_WORLD);

//         // 这里进行 Lower和NewLower 的合并
//         pData = merge(pLower, nLower, pNewLower, nNewLower);
//         nData = nLower + nNewLower;
//     }
//     else{
//         MPI_Send(&nLower, 1, MPI_INT, rank - step, 0, MPI_COMM_WORLD);
//         MPI_Send(pLower, nLower, MPI_INT, rank - step, 0, MPI_COMM_WORLD);

//         MPI_Recv(&nNewUpper, 1, MPI_INT, rank - step, 0, MPI_COMM_WORLD, status);
//         pNewUpper = (int*)malloc(sizeof(int) * nNewUpper);
//         MPI_Recv(pNewUpper, nNewUpper, MPI_INT, rank - step, 0, MPI_COMM_WORLD, status);

//         // 这里进行 Upper和NewUpper的合并
//         pData = merge(pUpper, nUpper, pNewUpper, nNewUpper);
//         nData = nUpper + nNewUpper;
//     #ifdef LOG
//         cout<<"big rank: "<<rank<<", nData = "<<nData<<endl;
// #endif
//     }

// #ifdef LOG
//     cout<<"partion结束，rank: "<<rank<<", nData = "<<nData<<endl;
// #endif

//     if(localMPI_Group != MPI_GROUP_NULL) MPI_Group_free(&localMPI_Group);
//     if(localMPI_Comm != MPI_COMM_NULL) MPI_Comm_free(&localMPI_Comm);

//     free(pLower); pLower = NULL;
//     free(pUpper); pUpper = NULL;
//     free(pNewLower); pNewLower = NULL;
//     free(pNewUpper); pNewUpper = NULL;

//     // 线程组的分裂
//     // if(rankInGroup < threadSize / 2){
//     //     Partion(threadBase, threadSize / 2, pData, nData);
//     // }
//     // else{
//     //     Partion(threadBase + threadSize / 2, threadSize / 2, pData, nData);
//     // }

// }
