#include "header.h"
void* solver(void* arg)
{
	args* a = (args*)arg;
	int p = a->p;
	int k = a->k;
	int n1 = a->n1;
	int n2 = a->n2;
	int len_sec = n1/p;
	int ind = len_sec*k;
	int m = ind+len_sec;
	double* arr = a->arr;
	double* row1 = new double[n2];
	double* row2 = new double[n2]; // чужие строки
	double* row3 = new double[n2];
	double* row4 = new double[n2];
	double* my_row = new double[n2];
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
	if(k == p-1)
		m = n1;
	// заполняем row1..row4
	if(ind-2>=0)
		Copy_arr(row1,arr,n2,ind-2);

	if(ind - 1 >=0)
		Copy_arr(row2,arr,n2,ind-1);
	if(m < n1)
		Copy_arr(row3,arr,n2,m);
	if(m+1 < n1)
		Copy_arr(row4,arr,n2,m+1);

	for(int i = ind;i<m;i++)
	{
		if(i<2 || i+2>=n1) 
			continue;
		for(int j = 2;j<n2-2;j++)
		{
			if(5*arr[i*n2+j]>=arr[(i-2)*n2+j] + arr[(i+2)*n2+j] + arr[(i-2)*n2+j-2]+ arr[(i+2)*n2+j+2])
			{
				a->count++;
				a->arith_mean+=arr[i*n2+j];
			}
		}
	}
	reduce_sum(p,&(a->count),1);
	reduce_sum(p,&(a->arith_mean),1);
	double arith_mean;
	if(fabs(a->count) > 1.e-15)
		arith_mean = a->arith_mean / a->count;
	else
	{
		t = get_cpu_time() - t;
		a->t = t;
		delete[] row1;
		delete[] row2;
		delete[] row3;
		delete[] row4;
		delete[] my_row;
		return NULL; // т.к менять нечего
	}
	for(int i =ind;i<m-1;i+=2)
	{
		process_start(arr,row1,row3,row4,my_row,n1,n2,i,arith_mean,m);
		process_start(arr,row2,row3,row4,my_row,n1,n2,i+1,arith_mean,m);
	}
	if((m-ind)%2==1)
		process_start(arr,row1,row3,row4,my_row,n1,n2,m-1,arith_mean,m); // строк нечетно
	t = get_cpu_time() - t;
	a->t = t;
	delete[] row1;
        delete[] row2;
        delete[] row3;
        delete[] row4;
        delete[] my_row;
	return NULL;
}
void Copy_arr(double*arr1,double*arr2,int n2,int ind)
{
	for(int i = 0;i<n2;i++)
		arr1[i] = arr2[ind*n2 + i];
}
void process_start(double *arr,double *row1,double *row3,double *row4, double* my_row,int n1,int n2,int ind,double arith_mean,int m)
{
	int i = ind;
	if(ind>=2 && ind+2<m)
        {
                Copy_arr(my_row,arr,n2,ind);
                for(int j = 2;j<n2-2;j++)
                {
                        if(5*arr[i*n2+j]>=row1[j] + arr[(ind+2)*n2+j] + row1[j-2]+ arr[(i+2)*n2+j+2])
                                arr[i*n2+j] = arith_mean;
                }
                Copy_arr(row1,my_row,n2,0);
        }
        else if(ind>=2 && ind+2==m && ind+2<n1)
        {
                Copy_arr(my_row,arr,n2,ind);
                for(int j = 2;j<n2-2;j++)
                {
                        if(5*arr[i*n2+j]>=row1[j] + row3[j] + row1[j-2]+ row3[j+2])
                                arr[i*n2+j] = arith_mean;
                }
                Copy_arr(row1,my_row,n2,0);
        }
        else if(ind>=2 && ind+2==m+1 && ind+2<n1)
        {
                Copy_arr(my_row,arr,n2,ind);
                for(int j = 2;j<n2-2;j++)
                {
                        if(5*arr[i*n2+j]>=row1[j] + row4[j] + row1[j-2]+ row4[j+2])
                                arr[i*n2+j] = arith_mean;
                }
                Copy_arr(row1,my_row,n2,0);
        }
        else
                Copy_arr(row1,arr,n2,ind);
}
