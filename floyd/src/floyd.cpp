
#include "floyd.h"

using namespace std;


void SerialFloyd(Mat &mat){
    for(int i = 0; i < mat.size(); i++){
        for(int j = 0; j < mat[i].size(); j++){
            for(int k = 0; k < mat.size(); k++){
                mat[i][j] = min(mat[i][j], mat[i][k] + mat[k][j]);
            }
        }
    }
}

void OpenMPFloyd(Mat &mat){
#pragma omp parallel for num_threads(4)
    for(int i = 0; i < mat.size(); i++){
        for(int j = 0; j < mat[i].size(); j++){
            for(int k = 0; k < mat.size(); k++){
                mat[i][j] = min(mat[i][j], mat[i][k] + mat[k][j]);
            }
        }
    }

}

void MPIFloyd(Mat &mat, int argc, char** argv){
    
}

