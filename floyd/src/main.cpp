#include "stopwatch.h"
#include "floyd.h"
#include <mpi.h>
#include <memory.h>

using namespace std;



Mat mat;

int main(int argc, char **argv){

    int mat_size = 4000;

    MPIFloyd(mat, mat_size, argc, argv);

}
