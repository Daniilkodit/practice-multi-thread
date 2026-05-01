#include "header.h"
int main(int argc,char*argv[])
{

	int p,k,n;
	std::string name;
	if(!(argc == 4 && sscanf(argv[1],"%d",&p) == 1
			       	&& sscanf(argv[2],"%d",&n)))
	{
		printf("Usage:%s p n filename\n",argv[0]);
                return 1;
	}
	if(n < p)
		p = n;
	name = argv[3];
	pthread_barrier_t barrier;
	pthread_t* tid = new pthread_t[p];
	double *arr = new double[n];
	double *first1 = new double[p]{};
	double *last1 = new double[p]{};
	double *last2 = new double[p]{};
	double *first2 = new double[p]{};
	int *index_f = new int[p]{};
	int *index_l = new int[p]{};
	int *ind_f = new int[p]{};
	int *ind_l = new int[p]{};
	bool *flag_connect = new bool[p]{};
	args*a = new args[p];
	pthread_barrier_init(&barrier, NULL, p);
	std::ifstream in;
	in.open(name);
	if (!in.is_open())
	{
		printf("Ошибка: невозможно открыть файл %s\n", name.c_str());
		return -1;
	}
	for ( int i = 0; i < n; i++)
	{
		if (!(in >> arr[i]))
		{
			printf("Ошибка: недостаточно данных или неверный формат в файле %s\n", name.c_str()); 
			return -2;
		}
	}
	in.close();
	for(k=0;k<p;k++)
	{
		a[k].arr = arr;
		a[k].k = k;
		a[k].p = p;
		a[k].n = n;
		a[k].barrier = &barrier;
		a[k].first1 = first1;
		a[k].last1 = last1;
		a[k].first2 = first2;
                a[k].last2 = last2;
		a[k].index_f = index_f;
		a[k].index_l = index_l;
		a[k].flag_connect = flag_connect;
		a[k].ind_f = ind_f;
		a[k].ind_l = ind_l;
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

	printf("RESULT %d:",p);
	for(int i =0;i<n;i++)
		printf(" %lf",arr[i]);

	printf("\nGeneral time %.2f\n",elapsed);
	for(k=0;k<p;k++)
		printf("Thread %d time: %.2f\n",k+1,a[k].t);

	delete[] a;
        delete[] tid;   
        delete[] arr;
	delete[] first1;
	delete[] first2;
	delete[] last1;
	delete[] last2;
	delete[] flag_connect;
	delete[] index_l;
	delete[] index_f;
	delete[] ind_l;
	delete[] ind_f;

	pthread_barrier_destroy(&barrier);
	return 0;
}
