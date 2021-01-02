#ifndef _FLOYD_H
#define _FLOYD_H


#include <vector>
#include <algorithm>
#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <random>
#include <fstream>

using namespace std;

typedef int** Mat;

int MPIFloyd(Mat &mat, int argc, char** argv);

#endif