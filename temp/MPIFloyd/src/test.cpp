#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "mpi.h"

using namespace std;

#define MAX 10
#define NOT_CONNECTED -1

int distances[MAX][MAX];
int result[MAX][MAX];
// number of nodes
int nodesCount;

// initialize all distances to
void Initialize() {
    memset(distances, NOT_CONNECTED, sizeof(distances));
    memset(result, NOT_CONNECTED, sizeof(result));

    for (int i = 0; i < MAX; ++i)
        distances[i][i] = 0;
}

int cmp(const void *a, const void *b) { return *(int *)a - *(int *)b; }

int main(int argc, char *argv[]) {
    Initialize();

    // get the nodes count
    scanf("%d", &nodesCount);

    // edges count
    int m;
    scanf("%d", &m);

    while (m--) {
        // nodes - let the indexation begin from 1
        int a, b;

        // edge weight
        int c;

        scanf("%d-%d-%d", &a, &c, &b);
        distances[a][b] = c;
    }

    int size, rank;

    MPI_Init(&argc, &argv);

    MPI_Datatype rtype;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int slice = (nodesCount) / size;  // nodesCount节点的个数，size表示线程的个数

    MPI_Bcast(distances, MAX * MAX, MPI_INT, 0, MPI_COMM_WORLD); // 0号线程将distance矩阵广播给所有线程
    MPI_Bcast(&nodesCount, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&slice, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Floyd-Warshall
    int sent = 1;

    for (int k = 1; k <= nodesCount; ++k) { 
        int th = 1;
        for (; th <= size; th++) {
            if (1 + slice * (th - 1) <= k && k <= slice * th) // 如果k在前面的某个区块
                sent = th;
        }
        if (1 + slice * (th - 1) <= k && k <= nodesCount) // 如果k在最后一个区块
            sent = size;

        MPI_Bcast(&distances[k], nodesCount + 1, MPI_INT, sent - 1,
                  MPI_COMM_WORLD); // k所在区块负责的线程将，k行数据发送出去。

        if (rank != size - 1) { // 如果当前线程不是最后一个线程
            for (int i = 1 + slice * (rank); i <= slice * (rank + 1); ++i) { 
                // 如果不是线程号为size - 1的线程，那么就选择在[1 + slice * (rank), lice * (rank + 1)]中的做为出发节点
                if (distances[i][k] != NOT_CONNECTED) {
                    for (int j = 1; j <= nodesCount; ++j) {
                        if (distances[k][j] != NOT_CONNECTED &&
                            (distances[i][j] == NOT_CONNECTED ||
                             distances[i][k] + distances[k][j] <
                                 distances[i][j])) {
                            distances[i][j] = distances[i][k] + distances[k][j];
                        }
                    }
                }
            }
        }
        else { // rank == size - 1 // 如果当前线程是最后一个线程
            for (int i = 1 + slice * rank; i <= nodesCount; ++i) {
                // 如果是最后一个线程，那么就选择剩余的那一些。
                if (distances[i][k] != NOT_CONNECTED) {
                    for (int j = 1; j <= nodesCount; ++j) {
                        if (distances[k][j] != NOT_CONNECTED &&
                            (distances[i][j] == NOT_CONNECTED ||
                             distances[i][k] + distances[k][j] <
                                 distances[i][j])) {
                            distances[i][j] = distances[i][k] + distances[k][j];
                        }
                    }
                }
            }
        }
    }

    for (int k = 1; k <= nodesCount; ++k) { // 上面也有这段类似的代码，k行所在区块负责的线程，将k行发送出去。
        int th = 1;
        for (; th <= size; th++) {
            if (1 + slice * (th - 1) <= k && k <= slice * th)
                sent = th;
        }
        if (1 + slice * (th - 1) <= k && k <= nodesCount)
            sent = size;

        MPI_Bcast(&distances[k], nodesCount + 1, MPI_INT, sent - 1,
                  MPI_COMM_WORLD);
    }

    MPI_Reduce(distances, result, MAX * MAX, MPI_INT, MPI_MIN, 0,
               MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 1; i <= nodesCount; i++) {
            for (int j = 1; j <= nodesCount; j++) {
                printf("%d ", result[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }

    /* Shut down MPI */
    MPI_Finalize();

    return 0;
}
