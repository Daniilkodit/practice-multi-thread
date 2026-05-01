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
	double* first = new double[p]{};
	int*  first_count = new int[p]();
	int* last_count = new int[p]();
	double*last = new double[p]{};
	double*posl_max = new double[p]{};
	int* count_max = new int[p]{};
	bool* flag = new bool[p]{false};
	int* number_neighbours = new int[p];
	for(int i=0;i<p;i++)
	{
		number_neighbours[i] = 0;
	}
	args*a = new args[p];
	pthread_barrier_init(&barrier, NULL, p);
	for(k=0;k<p;k++)
	{
		a[k].filename = argv[k+1];
		a[k].first = first;
		a[k].first_count = first_count;
		a[k].last_count = last_count;
		a[k].last = last;
		a[k].flag = flag;
		a[k].k = k;
		a[k].p = p;
		a[k].barrier = &barrier;
		a[k].number_neighbours = number_neighbours;
		a[k].posl_max = posl_max;
		a[k].count_max = count_max;
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
		if(a[k].my_err<0)
		{
			global_err+=a[k].my_err;
			int err = a[k].my_err;
			char*name = a[k].filename;
			switch(err)
			{
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
		delete[] first;
		delete[] first_count;
		delete[] last;
		delete[] last_count;
		delete[] flag;
		delete[] count_max;
		delete[] posl_max;
		delete[] number_neighbours;
		pthread_barrier_destroy(&barrier);
		return 1;
	}
	int res=0;
	for(int i=0;i<p;i++)
		res+=a[i].res;
	printf("Res = %d\n",res);

	delete[] a;
	delete[] tid;
	delete[] first;
	delete[] first_count;
	delete[] last;
	delete[] last_count;
	delete[] flag;
	delete[] count_max;
	delete[] posl_max;
	delete[] number_neighbours;
	pthread_barrier_destroy(&barrier);
		
	return 0;
}
