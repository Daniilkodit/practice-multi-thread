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
		pthread_barrier_t* barrier;

};
enum {TRASH=-2, N_OPEN};
void* solver(void*);
double get_full_time();
double get_cpu_time();


