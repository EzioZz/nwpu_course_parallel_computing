#include "mpi.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

void read_row_striped_matrix(char *s, void ***subs, void **storage,
                             MPI_Datatype dtype, int *m, int *n,
                             MPI_Comm comm) {
  if (id == (p - 1)) {
    for (i = 0; i < p - 1; i++) {
      x = fread(*storage, datum_size, BLOCK_SIZE(i, p, *m) * *n, infileptr);
      MPI_Send(*storage, BLOCK_SIZE(i, p, *m) * *n, dtype, i, DATA_MSG, comm); // p为线程的个数，
    }
    else {
      MPI_Recv(*storage, local_rows * *n, dtype, p - 1, DATA_MSG, comm,
               &status);
    }
  }
}
void print_row_striped_matrix(void **a, MPI_Datatype dtype, int m,
                              int n， MPI_Comm comm) {

  if (id == 0) {
    print_submatrix(a, dtype, local_rows, n);
    if (p > 1) {
      for (i = 1; i < p; i++) {
        MPI_Send(&prompt, 1, MPI_INT, i, PROMPT_MSG, MPI_COMM_WORLD);
        MPI_Recv(&prompt, 1, dtype, i, PROMPT_MSG, MPI_COMM_WORLD, &status);
      }
    } // if (p > 1)
  }   // if (id == 0)
  else {
    MPI_Recv(&prompt, 1, dtype, 0, PROMPT_MSG, MPI_COMM_WORLD, &status);
    print_submatrix(a, dtype, local_rows, n);
    MPI_Send(&prompt, 1, MPI_INT, 0, PROMPT_MSG, MPI_COMM_WORLD);
  }
}

void compute_shortest_paths(int id, int p, dtype **a, int n) {

  for (k = 0; k < n; k++) {
    root = BLOCK_OWNER(k, p, n);
    if (root == id) {
      offset = k - BLOCK_LOW(id, p, n);
      for (j = 0; j < n; j++) {
        tmp[j] = a[offset][j];
      }
    }
    MPI_Bcast(tmp, n, MPI_TYPE, root, MPI_COMM_WORLD);
    for (i = 0; i < BLOCK_SIZE(id, p, n); i++)
      for (j = 0; j < n; j++)
        a[i][j] = MIN(a[i][j], a[i][k] + tmp[j]);
  }
}

int main(int argc, char *argv[]) { return 0; }
