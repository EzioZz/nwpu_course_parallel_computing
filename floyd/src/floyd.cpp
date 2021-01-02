#include "floyd.h"

using namespace std;

void SerialFloyd(Mat &mat, int size){
    for(int k = 0; k < size; k++){
        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                mat[i][j] = min(mat[i][j], mat[i][k] + mat[k][j]);
            }
        }
    }
}

int MPIFloyd(Mat &mat, int argc, char** argv){
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    
    
    return 1;
}


void dataGenerator(int size){
    std::random_device e; // 改用random_device可以解决多线程情况下，产生重复随机数序列的问题。
    std::uniform_real_distribution<double> u(1, 100);
    
    ofstream fout;
    fout.open("../in.txt", ios::out);

    fout<<size<<std::endl;

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            int temp = (int)u(e);
            fout<<temp<<" ";
        }
        fout<<std::endl;
    }
}

void readMatrixFile(Mat &mat, int size){
    ifstream fin;
    fin.open("../in.txt", ios::in);
    int temp;

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            fin>>mat[i][j];
        }
    }

#ifdef DEBUG
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            cout<<mat[i][j]<<" ";
        }
        cout<<endl;
    }
#endif

}

