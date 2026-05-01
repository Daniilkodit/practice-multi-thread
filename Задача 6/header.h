#include<stdio.h>
#include<pthread.h>
#include<fstream>
#include<cmath>
#include <unistd.h>
#include <sched.h>
#include <sys/resource.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
class args
{
        public:
                double *arr = nullptr;
                int k = 0;
		int n1 = 0;
		int n2 = 0;
                int p = 0;
		double t = 0;
		double count = 0;
		double arith_mean = 0;
		pthread_barrier_t* barrier;

};
enum {TRASH=-2, N_OPEN};
void* solver(void*);
void reduce_sum(int p, double *a, int n);
void Copy_arr(double*arr1,double*arr2,int n2,int ind);
void process_start(double *arr,double *row1,double *row3,double *row4, double* my_row,int n1,int n2,int ind,double arith_mean,int m);
double get_full_time();
double get_cpu_time();


