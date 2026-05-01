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
typedef unsigned long long int LLu;
class args
{
        public:
		LLu N = 30000;
		LLu sum = 0;
                unsigned int k = 1;
		LLu n = 0;
		LLu* start = nullptr;
		LLu* count_segment = nullptr;
		LLu* sum_segment = nullptr;
                unsigned int p = 0;
		double t =0;
		pthread_barrier_t* barrier;

};
enum {TRASH=-2, N_OPEN};
void* solver(void*);
double get_full_time();
double get_cpu_time();


