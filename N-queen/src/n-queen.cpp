#include "n-queen.h"
#include <math.h>
#include <vector>
#include <omp.h>


int checkDiagonal(std::vector<int> array, std::vector<int> flag){
    // 返回1表示合格。返回0表示不合格，存在对角线上的棋子
    for(int i = 0; i < array.size(); i++){
        for(int j = i + 1; j < array.size(); j++){
            // 搜索是否存在相同对角线上的
            int delta_x = abs(i - j);
            int delta_y = abs(array[i] - array[j]);
            if(delta_x == delta_y){
                return 0; //说明存在对角线上的棋子
            }
        }
    }
    return 1;
}

int search(std::vector<int> array, std::vector<int> flag){
    // array用来记录之前的放置历史。array.size()可以用来表示层数。
    // flag用来表示，哪一列已经用过了。flag[i] = 1表示，第i列已经被放置过。

    if(array.size() == flag.size()){ //递归已经到了最深的一层
        return 1;
    }

    int sum = 0;
// #pragma omp parallel for firstprivate(array, flag)
    for(int i = 0; i < flag.size(); i++){
        if(!flag[i]){ // 如果这一列没有放过
            flag[i] = 1; //放到这一列上
            array.push_back(i);
            if(checkDiagonal(array, flag)){
                sum += search(array, flag);
            }
            flag[i] = 0;
            array.pop_back();
        }
    }
    return sum;
}


int SerialNQueen(int n){
    // 输入：n表示输入矩阵的大小
    // 输出：返回n皇后能够摆放的个数。
    int sum = 0;
    for(int i = 0; i < n; i++){
        std::vector<int> array; 
        std::vector<int> flag(n, 0);
        flag[i] = 1;
        array.push_back(i);
        sum += search(array, flag);
    } 
    return sum;

}

int ParallelNQueen(int n, int numThreads){
    // 输入：n表示输入矩阵的大小, numThreads表示开启的线程个数
    // 输出：返回n皇后能够摆放的个数。
    int sum = 0;

    int k = omp_get_nested();
    omp_set_nested(1);
#pragma omp parallel for reduction(+:sum) num_threads(numThreads)
    for(int i = 0; i < n; i++){
        std::vector<int> array;  // 这两个都是threadprivate的变量
        std::vector<int> flag(n, 0);
        flag[i] = 1;
        array.push_back(i);
        sum = search(array, flag);
    } 
    return sum;

}