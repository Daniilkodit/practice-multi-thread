#include "header.h"
int main(int argc,char*argv[])
{

	int p,k,n1,n2;
	std::string name;
	if(!(argc == 5 && sscanf(argv[1],"%d",&p) == 1
			       	&& sscanf(argv[2],"%d",&n1)
			       	&& sscanf(argv[3],"%d",&n2)))
	{
		printf("Usage:%s p n1 n2 filename\n",argv[0]);
                return 1;
	}
	if(n1 < p)
		p = n1;
	name = argv[4];
	pthread_barrier_t barrier;
	pthread_t* tid = new pthread_t[p];
	double *arr = new double[n1*n2];
	args*a = new args[p];
	pthread_barrier_init(&barrier, NULL, p);
	std::ifstream in;
	in.open(name);
	if (!in.is_open())
	{
		printf("Ошибка: невозможно открыть файл %s\n", name.c_str());
		return -1;
	}
	for ( int i = 0; i < n1; i++)
	{
		for(int j=0;j<n2;j++)
		{
			if (!(in >> arr[i*n2 +j]))
			{
				printf("Ошибка: недостаточно данных или неверный формат в файле %s\n", name.c_str()); 
				return -2;
			}
		}
	}
	in.close();
	for(k=0;k<p;k++)
	{
		a[k].arr = arr;
		a[k].k = k;
		a[k].p = p;
		a[k].n1 = n1;
		a[k].n2 = n2;
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
	printf("%d %d\n",n1,n2);
	for(int i =0;i<n1;i++)
	{
		for(int j =0;j<n2;j++)
			printf("%lf ",arr[i*n2+j]);
		printf("\n");
	}

	printf("\nGeneral time %.2f\n",elapsed);
	for(k=0;k<p;k++)
		printf("Thread %d time: %.2f\n",k+1,a[k].t);
	delete[] arr;
        delete[] tid;   
        delete[] a;

	pthread_barrier_destroy(&barrier);
	return 0;
}
