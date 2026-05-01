#include "header.h"
void* solver(void* arg)
{
	static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
	args* a = (args*)arg;
	unsigned int p = a->p;
	unsigned int k = a->k;
	LLu* astart = a->start;
	LLu n = a->n;
	LLu sum = 0;
	LLu N = a->N;
	LLu count = 0,start = k*N,end = start+N;
	static LLu aend = N*p;
	static LLu acount = 0;
	static LLu asum = 0;
	if(n == 1 )
	{
		a->sum = 2;
		return nullptr;
	}

	if(n==0)
		return nullptr;

	if(start == 0)
	{
		start = 3;
		count++;
		sum = 2;
	}
#if 1
	cpu_set_t cpu;
	CPU_ZERO(&cpu);

	int n_cpus = get_nprocs();
	int cpu_id = n_cpus - 1 - (k % n_cpus);

	CPU_SET(cpu_id, &cpu);
	pthread_t tid = pthread_self();
	pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpu);
#endif	
	double t = get_cpu_time();
	astart[k] = start;
	pthread_barrier_wait(a->barrier); // пусть они начнут одновременно
	while(1)
	{

		for(LLu number = start+((start+1)%2);number<end;number+=2)
		{
			bool fl = true;
			for(LLu q = 3;q * q <= number;q += 2)
			{
				if(number % q == 0)
				{
					fl = false;
					break;
				}
			}
			if(fl)
			{
				count++;
				sum+= number;
			}
		}
		pthread_mutex_lock(&m);
		acount+=count;
		asum+=sum;
		if(acount>=n)
		{
			pthread_mutex_unlock(&m);
			// мое n-е простое лежит между start[w] и end
			end = aend; 
			break;
		}
		start = aend;
		aend = aend+N;
		end = aend;
		pthread_mutex_unlock(&m);
		sum = 0;
		count = 0;
		astart[k] = start;
	}
	pthread_barrier_wait(a->barrier);
	
	if(k == 0)
	{
		start = astart[0]; // ищем отрезок в котором точно лежит n-е простое
		for(unsigned int i=1;i<p;i++)
		{
			if(start>astart[i])
				start = astart[i];
		}
		astart[0] = start;
	}
	pthread_barrier_wait(a->barrier);
	start = astart[0];
	end = aend; // получили отрезок у каждого потока [start, end] где точно лежит n-e простое
	//уберем из acount и asum все лишнее;
	LLu len_sec = (end-start)/p;
	start = start + k*len_sec;
	if(k != p-1)
		end = start + len_sec;
	
	count = 0;
	sum = 0;
	pthread_barrier_wait(a->barrier);
	for(LLu number = start+((start+1)%2);number<end;number+=2)
	{
		bool fl = true;
		for(LLu q = 3;q * q <= number;q += 2)
		{
			if(number % q == 0)
			{
				fl = false;
				break;
			}
		}
		if(fl)
		{
			count++;
			sum+= number;
		}
	}
	pthread_mutex_lock(&m);
	acount -= count;
	asum -= sum;
	pthread_mutex_unlock(&m);
	// теперь у меня сумма и количество простых посчитано до start
	//и самое важное я знаю сколько в каждом куске простых и их суммы!!
	pthread_barrier_wait(a->barrier);
	start = astart[0];
	a->count_segment[k] = count;
	a->sum_segment[k] = sum;
	pthread_barrier_wait(a->barrier);
	if(k==0)
	{
		unsigned i = 0;
		for(i =0;a->count_segment[i] + acount <= n ;i++)
		{
			asum += a->sum_segment[i];
			acount += a->count_segment[i];
			if(i == p-1)
				break;
		}
		// получи что на i сегменте n-e простое число
		start = start +i*len_sec;
		end = start +len_sec;
		if(acount == n)
		{
			a->sum = asum;
			t = get_cpu_time() - t;
        		a->t = t;
			return nullptr;
		}
		for(LLu number = start+((start+1)%2);number<end;number+=2)
		{
			bool fl = true;
			for(LLu q = 3;q * q <= number;q += 2)
			{
				if(number % q == 0)
				{
					fl = false;
					break;
				}
			}
			if(fl)
			{

				acount++;
				asum+= number;
				if(acount == n)
				{
					a->sum = asum;
					t = get_cpu_time() - t;
					a->t = t;
					return nullptr;
				}
			}
		}
	}
	t = get_cpu_time() - t;
	a->t = t;
	return NULL;
}
