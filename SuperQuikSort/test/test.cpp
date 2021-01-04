#include <iostream>
#include <memory.h>

using namespace std;

int test(int* &pData, int &nData){

    free(pData);
    nData = 12;
    pData = (int*)malloc(nData * sizeof(int));

    for(int i = 0; i < nData; i++){
        pData[i] = 1;
    }

    int* pTest;
    int nTest;

    free(pTest);


}

int* merge(int* pList0, int nList0, int* pList1, int nList1){
    int nRes = nList0 + nList1;
    int* pRes = (int*)malloc(nRes * sizeof(int));
    
    int p0 = 0;
    int p1 = 0;
    int count = 0;

    while(p0 < nList0 && p1 < nList1){
        if(pList0[p0] <= pList1[p1]){
            pRes[count] = pList0[p0];
            p0++;
            count++;
        }
        else{
            pRes[count] = pList1[p1];
            p1++;
            count++;
        }
    }

    while(p0 < nList0){
        pRes[count] = pList0[p0];
        p0++;
        count++;
    }

    while(p1 < nList1){
        pRes[count] = pList1[p1];
        p1++;
        count++;
    }

    return pRes;
}

int main(){

    int* pData;
    int nData = 10;
    pData = (int*)malloc(nData * sizeof(int));
    for(int i = 0; i < nData; i++){
        pData[i] = i*3;
    }

    test(pData, nData);

    return 0;
}