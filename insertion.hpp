#ifndef INSERTION_H
#define INSERTION_H
//Used from https://tinyurl.com/ysu428yz
template<typename T>
void InsertionSort(T* arr, int n){
  int i,j;
  T temp;
  for(i=1; i<n; ++i){
    temp = arr[i];
    j = i-1;
    while(j>=0 && arr[j]>temp){
      arr[j+1] = arr[j];
      j = j-1;
    }
    arr[j+1] = temp;
  }
}
#endif