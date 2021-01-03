#include "../src/floyd.h"
#include <stdlib.h>
#include <vector>

using namespace std;

void SerialFloyd(Mat &mat, int &mat_size){

    mat = (int**)malloc(sizeof(int*) * mat_size);
    int* data = (int*)malloc(sizeof(int) * mat_size * mat_size);
    for(int i = 0; i < mat_size; i++){
        mat[i] = &data[i * mat_size];
    }
    ReadMatrixFile(mat, mat_size);
    for(int k = 0; k < mat_size; k++){
        for(int i = 0; i < mat_size; i++){
            for(int j = 0; j < mat_size; j++){
                mat[i][j] = min(mat[i][j], mat[i][k] + mat[k][j]);
            }
        }
    }
    ofstream fout;
    fout.open("../out1.txt", ios::out);
    fout<<mat_size<<endl;
    for(int i = 0; i < mat_size; i++){
        for(int j = 0;  j < mat_size; j++){
            fout<<mat[i][j]<<" ";
        }
        fout<<endl;
    }

}

int main(int argc, char** argv){
    Mat mat;
    int mat_size = 4000;
    SerialFloyd(mat, mat_size);

    // 比较两个结果    
    vector<vector<int> > mat0(mat_size, vector<int>(mat_size, 0));
    vector<vector<int> > mat1(mat_size, vector<int>(mat_size, 0));


    ifstream fin;
    fin.open("../out0.txt", ios::in);
    fin>>mat_size;
    for(int i = 0; i < mat_size; i++){
        for(int j = 0; j < mat_size; j++){
            fin>>mat0[i][j];
        }
    }

    fin.close();

    fin.open("../out1.txt", ios::in);
    fin>>mat_size;
    for(int i = 0; i < mat_size; i++){
        for(int j = 0; j < mat_size; j++){
            fin>>mat1[i][j];
        }
    }

    int isSame = 1;

    for(int i = 0; i < mat_size; i++){
        for(int j = 0; j < mat_size; j++){
            if(mat0[i][j] != mat1[i][j]){
                isSame = 0;
            }
        }
    }

    if(isSame) cout<<"相同"<<endl;
    else cout<<"不相同"<<endl;


    return 0;
}