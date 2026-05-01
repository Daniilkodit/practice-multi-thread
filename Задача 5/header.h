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
		int n = 0;
                int p = 0;
		double t =0;
		// Общая идея: первые элементы у потока и поледние самые важные, они могут быть соединены с предыдщуми
		// и следующими потоками, а то что в середине мы ручками аккуратно поменяем
		double* first1 = nullptr;
		double* last1 = nullptr;
		double* first2 = nullptr; // чтобы сказать где геом началась
	        double* last2 = nullptr; // чтобы сказать где геом закончилась
		bool* flag_connect = nullptr; // связаны ли k и k-1 поток т.е в k-1 идет геом потом продолжается в k
		int *index_f = nullptr; // для удобства т.к я нахожу где заканчивается first2[k]
		int *index_l = nullptr;// для удобства т.к я нахожу где заканчивается last1[k]
		int *ind_l = nullptr;
		int *ind_f = nullptr;
		pthread_barrier_t* barrier;

};
enum {TRASH=-2, N_OPEN};
void* solver(void*);
double get_full_time();
double get_cpu_time();
void analys_segment(int ind_real,int ind,int m,double*b,double*f2,double*l1,int*ind_f,int*ind_l);


