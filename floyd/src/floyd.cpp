#include "floyd.h"
#include <algorithm>

using namespace std;

int getOwnerThread(int lineNum, int mat_size, int num_threads){
    // 获得这一行负责的线程编号。
    int p_thread;
    int slice = mat_size / num_threads;
    p_thread = lineNum / slice;
    return p_thread;
}

int MPIFloyd(Mat &mat, int mat_size, int argc, char** argv){

    double timeStart, timeEnd;

    MPI_Init(&argc, &argv);

    timeStart = MPI_Wtime();

    int num_threads, p_thread;
    MPI_Comm_size(MPI_COMM_WORLD, &num_threads);
    MPI_Comm_rank(MPI_COMM_WORLD, &p_thread);

    // 动态创建二维数组，保证数据连续的方式
    mat = (int**)malloc(sizeof(int*) * mat_size);
    int* data = (int*)malloc(sizeof(int) * mat_size * mat_size);
    for(int i = 0; i < mat_size; i++){
        mat[i] = &data[i * mat_size];
    }

    if(p_thread == (num_threads - 1)){
        // DataGenerator(mat_size);
        ReadMatrixFile(mat, mat_size);
    }
    
    MPI_Bcast(data, mat_size*mat_size, MPI_INT, num_threads - 1, MPI_COMM_WORLD);
    int slice = mat_size / num_threads; // 要求2:假设矩阵行数n可以被进（线）程数P整除，采用按行分块的方案

#ifdef DEBUG
    cout<<p_thread<<", num_threads :"<<num_threads<<endl;
    cout<<p_thread<<", slice :"<<slice<<endl;
    cout<<p_thread<<", mat_size :"<<mat_size<<endl;
    cout<<p_thread<<", mat[0][0] :"<<mat[0][0]<<endl;
#endif

    for(int k = 0; k < mat_size; k++){
        // 迭代之前，将所需的第k行的数据进行广播。
        MPI_Bcast(mat[k], mat_size, MPI_INT, getOwnerThread(k, mat_size, num_threads), MPI_COMM_WORLD);

        for(int i = slice * p_thread; i < slice * (p_thread + 1); i++){
            for(int j = 0; j < mat_size; j++){
                mat[i][j] = min(mat[i][j], mat[i][k] + mat[k][j]);
            }
        }

    }

#ifdef DEVLOG
     cout<<p_thread<<", 完成k次迭代 :"<<endl;
#endif

    // 要求4：所有进（线）程依次将结果数据写入数据文件。进（线）程0先写，进（线）程1再写，进（线）程p-1最后写，最终完成结果输出
    int write_signal = 0;
    MPI_Status status;
    if(p_thread == (num_threads - 1)){

        ofstream fout;
        fout.open("../out0.txt", ios::out);
        fout<<mat_size<<endl;

        for(int dst_thread = 0; dst_thread < num_threads; dst_thread++){
            MPI_Send(&write_signal, 1, MPI_INT, dst_thread, 0, MPI_COMM_WORLD);

#ifdef DEVLOG 
            cout<<p_thread<<": want "<<dst_thread<<" print"<<endl;
#endif

            MPI_Recv(&write_signal, 1, MPI_INT, dst_thread, 0, MPI_COMM_WORLD, &status);

#ifdef DEVLOG
            cout<<p_thread<<": "<<dst_thread<<" print finish"<<endl;
#endif

        }

        WriteSubMatrix(mat, mat_size, p_thread, slice);
    }
    else{
        MPI_Recv(&write_signal, 1, MPI_INT, num_threads - 1, 0, MPI_COMM_WORLD, &status);
        WriteSubMatrix(mat, mat_size, p_thread, slice);
        MPI_Send(&write_signal, 1, MPI_INT, num_threads - 1, 0, MPI_COMM_WORLD);
    }

    timeEnd = MPI_Wtime();

    if(p_thread == (num_threads - 1)){
        cout<<"计算时间为: "<<timeEnd - timeStart<<"s"<<endl;
    }

    MPI_Finalize();
    
    return 1;
}


void DataGenerator(int size){
    std::random_device e; // 改用random_device可以解决多线程情况下，产生重复随机数序列的问题。
    std::uniform_real_distribution<double> u(1, 100);
    
    ofstream fout;
    fout.open("../in.txt", ios::out);

    fout<<size<<std::endl;

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            if(i ==j ){
                fout<<0<<" ";
            }
            else{
                int temp = (int)u(e);
                fout<<temp<<" ";
            }
        }
        fout<<std::endl;
    }
}

void ReadMatrixFile(Mat &mat, int &mat_size){
    ifstream fin;
    fin.open("../in.txt", ios::in);
    int temp;

    fin>>mat_size;

    for(int i = 0; i < mat_size; i++){
        for(int j = 0; j < mat_size; j++){
            fin>>mat[i][j];
        }
    }

    return;
}

void WriteSubMatrix(Mat &mat, int mat_size, int p_thread, int slice){
    ofstream fout;
    fout.open("../out0.txt", ios::out | ios::app);
    // fout<<"p_thread: "<<p_thread<<endl;
    for(int i = slice * p_thread; i < slice * (p_thread + 1); i++){
        for(int j = 0; j < mat_size; j++){
            fout<<mat[i][j]<<" ";
        }
        fout<<endl;
    }

}