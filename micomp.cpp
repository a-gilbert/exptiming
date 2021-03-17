#include<fstream>
#include<limits>
#include<cstdlib>
#include<cstdio>
#include<random>
#include<iostream>
#include<chrono>
#include<mpi.h>
#include "insertion.hpp"
#include "merge.hpp"

//write type info to disk
void write_tinfo(){
  std::fstream fobj;
  fobj.open("tinfo.csv", std::fstream::out);
  fobj << "type,size\n";
  fobj << "char," << sizeof(char) << std::endl;
  fobj << "int," << sizeof(int) << std::endl;
  fobj << "float," << sizeof(float) << std::endl;
  fobj << "double," << sizeof(double) << std::endl;
  fobj.close();
}

//shufflers to fill different array types. 
//TODO: find out how to write only one of these with templating
void cshuffle(std::mt19937_64* gen, char* arr, int n){
  std::uniform_int_distribution<> cg(std::numeric_limits<char>::min(), 
                                      std::numeric_limits<char>::max());
  for(int i=0; i<n; ++i){
    arr[i] = cg(*gen);
  }
}

void ishuffle(std::mt19937_64* gen, int* arr, int n){
  for(int i=0; i<n; ++i){
    arr[i] = (*gen)();
  }
}

void dshuffle(std::mt19937_64* gen, double* arr, int n){
  std::uniform_real_distribution<> fg(-2.0, 2.0);
  for(int i=0; i<n; ++i){
    arr[i] = fg(*gen);
  }
}

template<typename T>
double ITimer(T* arr, int n){
  auto t1 = std::chrono::steady_clock::now();
  InsertionSort(arr, n);
  auto t2 = std::chrono::steady_clock::now();
  std::chrono::duration<double> diff = t2-t1;
  return diff.count();
}

template<typename T>
double MTimer(T* arr, int n){
  auto t1 = std::chrono::steady_clock::now();
  MergeSort(arr, 0, n-1);
  auto t2 = std::chrono::steady_clock::now();
  std::chrono::duration<double> diff = t2-t1;
  return diff.count();
}


int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);
  //input args
  long int maxn = std::atol(argv[1]);
  long int ntrials = std::atol(argv[2]);
  long int seed = std::atol(argv[3]);
  //parallel stuff.
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  //holds out file for current rank.
  char buff[100];
  sprintf(buff, "MICRank%02d.csv", rank);
  std::fstream my_fobj(buff, std::fstream::out); 
  //different types of arrays used for sorting.
  char* carr;
  int* iarr;
  double* darr;

  std::mt19937_64 gen;
  gen.seed(seed);
  my_fobj << "sort,type,size,trial,rtime\n";
  //insertion sort tests
  //char test
  if(rank==0){
    write_tinfo();
    for(long int i=2; i<maxn; i=2*i){
      carr = (char*)malloc(i*sizeof(char));
      for(long int j=0; j<ntrials; ++j){
        cshuffle(&gen, carr, i);
        my_fobj << "insertion,char," << i << "," << j << ",";
        my_fobj <<  ITimer(carr, i) << std::endl; 
      }
      free(carr);
    }
  } else if(rank==1){
  //int test
    for(long int i=2; i<maxn; i=2*i){
      iarr = (int*)malloc(i*sizeof(int));
      for(long int j=0; j<ntrials; ++j){
        ishuffle(&gen, iarr, i);
        my_fobj << "insertion,int," << i << "," << j << ",";
        my_fobj << ITimer(iarr, i) << std::endl;
      }
      free(iarr);
    }
  } else if(rank==2){
  //double test
    for(long int i=2; i<maxn; i=2*i){
      darr = (double*)malloc(i*sizeof(double));
      for(long int j=0; j<ntrials; ++j){
        dshuffle(&gen, darr, i);
        my_fobj << "insertion,double," << i << "," << j << ",";
        my_fobj << ITimer(darr, i) << std::endl;
      }
      free(darr);
    }
  } else if(rank==3){
    for(long int i=2; i<maxn; i=2*i){
      carr = (char*)malloc(i*sizeof(char));
      for(long int j=0; j<ntrials; ++j){
        cshuffle(&gen, carr, i);
        my_fobj << "merge,char," << i << "," << j << ",";
        my_fobj <<  MTimer(carr, i) << std::endl; 
      }
      free(carr);
    } 
  } else if(rank==4){
     //int test
    for(long int i=2; i<maxn; i=2*i){
      iarr = (int*)malloc(i*sizeof(int));
      for(long int j=0; j<ntrials; ++j){
        ishuffle(&gen, iarr, i);
        my_fobj << "merge,int," << i << "," << j << ",";
        my_fobj << MTimer(iarr, i) << std::endl;
      }
      free(iarr);
    }
  } else if(rank==5){
    for(long int i=2; i<maxn; i=2*i){
      darr = (double*)malloc(i*sizeof(double));
      for(long int j=0; j<ntrials; ++j){
        dshuffle(&gen, darr, i);
        my_fobj << "merge,double," << i << "," << j << ",";
        my_fobj << MTimer(darr, i) << std::endl;
      }
      free(darr);
    }
  }
  my_fobj.close();
  MPI_Finalize();
}