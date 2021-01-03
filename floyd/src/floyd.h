#ifndef _FLOYD_H
#define _FLOYD_H

#include <vector>
#include <algorithm>
#include "mpi.h"
#include <stdio.h>
#include <iostream>
#include <random>
#include <fstream>

using namespace std;

typedef int** Mat;

// void SerialFloyd(Mat &mat, int &mat_size);

int MPIFloyd(Mat &mat, int mat_size, int argc, char** argv);
void ReadMatrixFile(Mat &mat, int &mat_size);
void DataGenerator(int size);
void WriteSubMatrix(Mat &mat, int mat_size, int p_thread, int slice);

#endif