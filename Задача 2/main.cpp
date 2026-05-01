#include "header.h"
int main(int argc,char*argv[])
{

	int p = argc-1,k;
	if(argc==1)
	{
		printf("Usage:%s filename\n",argv[0]);
		return 1;
	}
	pthread_barrier_t barrier;
	pthread_t* tid = new pthread_t[p];
	int *err = new int[p]();
	double* loc_min = new double[p]();
	args*a = new args[p];
	pthread_barrier_init(&barrier, NULL, p);
	for(k=0;k<p;k++)
	{
		a[k].filename = argv[k+1];
		a[k].err = err;
		a[k].loc_min = loc_min;
		a[k].k = k;
		a[k].p = p;
		a[k].barrier = &barrier;
	}

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
	int global_err=0;
	for(k=0;k<p;k++)
	{
		if(err[k])
		{
			global_err+=err[k];
			char*name = a[k].filename;
			switch(err[k])
			{
				case(EMPTY):
					printf("File %s is empty(an empty sequence cannot have a local minimum)\n",name);
                                        break;
				case(N_OPEN):
					printf("Cannot open %s\n",name);
					break;
				case(TRASH):
					printf("Trash in %s\n",name);
					break;
				default:
					printf("Unknown error %s\n",name);
					break;
			}
		}
	}
	if(global_err)
	{
		delete[] a;
		delete[] tid;
		delete[] err;
		delete[] loc_min;
		pthread_barrier_destroy(&barrier);
		return 1;
	}
	int res=0;
	for(int i=0;i<p;i++)
		res+=a[i].res;
	printf("Res = %d\n",res);

	 delete[] a;
         delete[] tid;   
         delete[] err;
	 delete[] loc_min;
	 pthread_barrier_destroy(&barrier);
	 return 0;
}
