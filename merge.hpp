#include<cstdlib>
#ifndef MERGE_H
#define MERGE_H

//Used from https://tinyurl.com/nwpuf2f4
template<typename T>
void Merge(T *arr, int s, int e){
  int z,x,y,m;
  T* temp = (T*) malloc((e-s+1)*sizeof(T));
  m = (s+e)/2;
  z = 0;
  x = s;
  y = m+1;
  while(x<=m && y<=e){
    if(arr[x]<arr[y]){
      temp[z] = arr[x];
      ++x;
      ++z;
    } else {
      temp[z] = arr[y];
      ++y;
      ++z;
    }
  }
  while(x<=m){
    temp[z] = arr[x];
    ++x;
    ++z;
  }
  while(y<=e){
    temp[z] = arr[y];
    ++y;
    ++z;
  }
  for(int i=s; i<=e; ++i){
    arr[i] = temp[i-s];
  }
  free(temp);
}

template<typename T>
void MergeSort(T* arr, int s, int e){
  if(s<e){
    int m = (s+e)/2;
    MergeSort(arr, s, m);
    MergeSort(arr, m+1, e);
    Merge(arr, s, e);
  }
}

#endif