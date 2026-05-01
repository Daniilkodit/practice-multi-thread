#include<stdio.h>
#include<pthread.h>
#include<fstream>
#include<cmath>
#include<iostream>
class args
{
        public:
                char* filename = nullptr;
                double err = 0;
		int my_err = 0;
                int k = 0;
                int p = 0;
                int res = 0;
		int* number_neighbours = nullptr; // соседи которые живые
		double* first = nullptr;
		int* first_count = nullptr;
		double* last = nullptr;
		int* last_count = nullptr;
		int* count_max = nullptr;
		double* posl_max = nullptr;
		double amax = 0;
		double thread_out = 0;
		bool*flag = nullptr;
		bool die_not = true;
		pthread_barrier_t* barrier;

};
enum {TRASH = -2, N_OPEN};
void* solver(void*);
void reduce_sum(int p, double *a, int n);

