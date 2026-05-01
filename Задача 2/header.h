#include<stdio.h>
#include<pthread.h>
#include<fstream>
#include<cmath>
class args
{
        public:
                char* filename = nullptr;
                int *err = nullptr;
                int k = 0;
                int p = 0;
                int res = 0;
		double* loc_min = nullptr;
		pthread_barrier_t* barrier;

};
enum {EMPTY = -3,TRASH, N_OPEN};
void* solver(void*);

