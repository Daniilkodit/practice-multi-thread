#include "header.h"
void* solver(void* arg)
{
	args* a = (args*)arg;
	int p = a->p;
	int k = a->k;
	int n = a->n;
	int len_sec = n/p;
	int ind = len_sec*k;
	int m = ind+len_sec;
	double* b = a->arr;
#if 1
	cpu_set_t cpu;
	CPU_ZERO(&cpu);

	int n_cpus = get_nprocs();
	int cpu_id = n_cpus - 1 - (k % n_cpus);

	CPU_SET(cpu_id, &cpu);
	pthread_t tid = pthread_self();
	pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpu);
#endif	
	double neighbour_left[2]{};
	double neighbour_right[2]{};


	if(k==p-1)
	 	m=n;
	if(ind ==0)
	{
		neighbour_left[0] = b[0];
		neighbour_left[1] = b[1];	
	}
	if(ind == 1)
	{
		neighbour_left[0] = b[1]; //сначала i = 3 
                neighbour_left[1] = b[0]; //потом i = 2
	}
	if(ind-2 >= 0)
	{
		neighbour_left[0] = b[ind-2];
		neighbour_left[1] = b[ind-1];
	}

	if(m < n)
		neighbour_right[0] = b[m];
	if(m+1 < n)
		neighbour_right[1] = b[m+1];
	
	pthread_barrier_wait(a->barrier);
	double t = get_cpu_time();
	for(int shift = 0;shift < 2;shift++)
	{
		for(int i = ind+shift;i<m;i+=2)
		{
			if(i - 2 >= 0 && i + 2 < m && neighbour_left[shift]*b[i+2]>=0)
			{
				double temp = b[i];
				b[i] = sqrt(neighbour_left[shift]*b[i+2]);
				neighbour_left[shift] = temp;
			}
			else if(i-2 >=0 && i+2==m && i+2<n && neighbour_left[shift]*neighbour_right[0]>=0)
			{
				b[i] = sqrt(neighbour_left[shift]*neighbour_right[0]);
			}
			else if(i-2 >=0 && i+2==m+1 && i+2<n && neighbour_left[shift]*neighbour_right[1]>=0)
			{
				b[i] = sqrt(neighbour_left[shift]*neighbour_right[1]);
			}
			else if(i - 2 >= 0 && i + 2 < m) // если произведение < 0 то left надо поменять
			{
				neighbour_left[shift] = b[i];
			}

		}
	}	
	t = get_cpu_time() - t;
	a->t = t;
	return NULL;
}
