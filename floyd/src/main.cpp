#include "omp.h"
#include <vector>
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <random>
#include <fstream>
#include "stopwatch.h"
#include "floyd.h"


using namespace std;

// typedef vector<vector<int>> Mat;


void dataGenerator(int size_x, int size_y){
    std::random_device e; // 改用random_device可以解决多线程情况下，产生重复随机数序列的问题。
    std::uniform_real_distribution<double> u(1, 100);
    
    ofstream fout;
    fout.open("../in.txt", ios::out);

    fout<<size_x<<" "<<size_y<<std::endl;

    for(int i = 0; i < size_x; i++){
        for(int j = 0; j < size_y; j++){
            int temp = (int)u(e);
            fout<<temp<<" ";
        }
        fout<<std::endl;
    }
}

void readMatrixFile(Mat &mat){
    ifstream fin;
    fin.open("../in.txt", ios::in);
    int temp;
    mat.clear();
    int size_x, size_y;
    fin>>size_x>>size_y;
    mat.resize(size_x);
    for(auto &x : mat){
        x.resize(size_y);
    }

    for(int i = 0; i < size_x; i++){
        for(int j = 0; j < size_y; j++){
            fin>>mat[i][j];
        }
    }

#ifdef DEBUG
    for(auto &vec : mat){
        for(auto &x : vec){
            std::cout<<x<<" ";
        }
        std::cout<<std::endl;
    }
#endif


}

int main(int argc, char **argv){
    int max_num_threads = omp_get_max_threads();
    std::cout<<max_num_threads<<std::endl;

    dataGenerator(1000, 1000);
    Mat mat;
    readMatrixFile(mat);
    Mat mat_temp = mat;
    stopwatch watch;
    watch.start();
    SerialFloyd(mat);
    watch.end();

    watch.start();
    OpenMPFloyd(mat_temp);
    watch.end();

}
