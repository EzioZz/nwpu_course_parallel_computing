#include <vector>
#include <algorithm>

using namespace std;

typedef vector<vector<int> > Mat;

void SerialFloyd(Mat &mat);
void OpenMPFloyd(Mat &mat);
void MPIFloyd(Mat &mat);