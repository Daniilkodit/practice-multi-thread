#include "header.h"
int main(int argc,char*argv[])
{

	unsigned int k,p;
	LLu n;
	std::string name;
	if(!(argc == 3 && sscanf(argv[1],"%u",&p) == 1
			       	&& sscanf(argv[2],"%llu",&n)))
	{
		printf("Usage:%s p n\n",argv[0]);
                return 1;
	}

	if(n < p)
		p = n;
	if(n==0)
	{
		printf("RES = 0\n");
		return 0;
	}
	pthread_barrier_t barrier;
	pthread_t* tid = new pthread_t[p];
	args*a = new args[p];
	LLu* start = new LLu[p];
	LLu* count_segment = new LLu[p];
	LLu* sum_segment = new LLu[p];
	pthread_barrier_init(&barrier, NULL, p);

	for(k=0;k<p;k++)
	{
		a[k].k = k;
		a[k].p = p;
		a[k].n = n;
		a[k].start = start;
		a[k].count_segment = count_segment;
		a[k].sum_segment = sum_segment;
		a[k].barrier = &barrier;
	}
	double elapsed = get_full_time();
	for(k=1;k<p;k++)
	{
		if(pthread_create(tid+k,NULL,solver,a+k))
		{
			printf("Creating the %d thread failed\n",k);
			return 1;
		}
	}
	solver(a+0);

	for(k=1;k<p;k++)
        {
		pthread_join(tid[k],0);
	}
	elapsed = get_full_time() - elapsed;

	printf("RES = %llu\n",a[0].sum);
	printf("\nGeneral time %.2f\n",elapsed);

	for(k=0;k<p;k++)
		printf("Thread %d time: %.2f\n",k+1,a[k].t);

	delete[] a;
        delete[] tid;   
	delete[] start;
	delete[] sum_segment;
	delete[] count_segment;
	pthread_barrier_destroy(&barrier);

	return 0;
}
