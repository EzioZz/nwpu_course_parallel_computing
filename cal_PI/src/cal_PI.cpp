#include <mpi.h>
#include <iostream>
#include <gmp.h>
#include <random>


using namespace std;

unsigned int count_time = 1000000;

int isInCircle(double &x, double &y){
    return (x*x + y*y) <= 1.0;
}

long long slave(){
    long long count_in_cirle = 0;
    for(int i = 0; i < count_time; i++){
        std::random_device e; // 改用random_device可以解决多线程情况下，产生重复随机数序列的问题。
        std::uniform_real_distribution<double> u(0, 1);
        double x = u(e);
        double y = u(e);
        if(isInCircle(x, y)) count_in_cirle++;
    }
    return count_in_cirle;
}

void cal_PI(int argc, char** argv){
    int size;
    int rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    long long global_count_in_circle = 0;

    long long count_in_cirle = slave();
    cout<<"id:"<<rank<<", count_in_cirle"<<count_in_cirle<<endl;
    MPI_Reduce(&count_in_cirle, &global_count_in_circle, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if(rank == 0){
        long long total_count_time = (long long)count_time * (long long)size;
        mpf_t a0, b0, c0;
        mpf_init(a0);
        mpf_init(b0);
        mpf_init(c0);
        mpf_set_si(a0, global_count_in_circle);
        mpf_set_si(b0, total_count_time);
        mpf_div(c0, a0, b0); // c0 = global_count_in_circle / total_count_time;

        mpf_set_si(b0, 4ll);
        mpf_mul(c0, c0, b0);

        gmp_printf("%.Ff\n", c0);

        cout<<total_count_time<<endl;
        cout<<global_count_in_circle<<endl;
    }
    MPI_Finalize();
}

int main(int argc, char** argv){

    cal_PI(argc, argv);

    return 0;
}


// 出现错误的代码。
// int main(int argc, char** argv){
//     int size;
//     int rank;
//     MPI_Init(&argc, &argv);
//     MPI_Comm_size(MPI_COMM_WORLD, &size);
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);

//     long long global_count_in_circle = 0;

    // if(rank == 0){
    //     double total_count_time = (double)count_time * (double) (size);
    //     cout<<total_count_time<<endl;
    //     cout<<global_count_in_circle<<endl;
    // }
    // else{
    //     long long count_in_cirle = slave();
    //     cout<<"id:"<<rank<<", count_in_cirle"<<count_in_cirle<<endl;
    //     MPI_Reduce(&count_in_cirle, &global_count_in_circle, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    // }
//     MPI_Finalize();

//     return 0;
// }
