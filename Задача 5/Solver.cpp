#include "header.h"
#include <iostream>
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
	if(k==p-1)
		 m=n;

	double t = get_cpu_time();
	int start = ind,end=-1;
	double q = 0,f1 = 0,l1 = 0,f2 = 0,l2 = 0;
	double f22 = 0,l11 = 0;
	double* first1 = a->first1;
	double* last1 = a->last1;
	double* first2 = a->first2;
	double* last2 = a->last2;
	int* index_l = a->index_l; // чтобы запомнить где у меня last1 и второй раз не искать
	int* index_f = a->index_f; // чтобы запомнить где у меня first2 и второй раз не искать
	int* ind_f = a->ind_f;
	int* ind_l = a->ind_l;
	double eps = 1.e-15;
	bool* flag_connect = a->flag_connect;
	int i;
	first1[k] = b[ind];
	last2[k] = b[m-1];
	if(ind-2>=0)
	{
		if(fabs(b[ind-1])>eps && fabs(b[ind-2])>eps)
		{
			analys_segment(ind,ind-2,m,b,&f22,&l11,&ind_f[k],&ind_l[k]);
			analys_segment(ind,ind-1,m,b,&first2[k],&last1[k],&index_f[k],&index_l[k]);
			flag_connect[k] =true;
		}
		else if(fabs(b[ind-1])>eps)
		{
			analys_segment(ind,ind-1,m,b,&first2[k],&last1[k],&index_f[k],&index_l[k]);
			flag_connect[k] = true;
			ind_l[k] = index_l[k];
                	ind_f[k] = index_f[k];
		}
		else
		{
			analys_segment(ind,ind,m,b,&first2[k],&last1[k],&index_f[k],&index_l[k]);
			flag_connect[k] = false;
			ind_l[k] = index_l[k];
                	ind_f[k] = index_f[k];
		}
	}
	else if(ind == 1)
	{
		if(fabs(b[ind-1])>eps)
		{
                        analys_segment(ind,ind-1,m,b,&first2[k],&last1[k],&index_f[k],&index_l[k]);
			flag_connect[k] = true;
		}
                else
		{
                        analys_segment(ind,ind,m,b,&first2[k],&last1[k],&index_f[k],&index_l[k]);
			flag_connect[k] = false;
		}
		ind_l[k] = index_l[k];
		ind_f[k] = index_f[k];
	}
	else
	{
		analys_segment(ind,ind,m,b,&first2[k],&last1[k],&index_f[k],&index_l[k]);
	}
	pthread_barrier_wait(a->barrier);

	if(k == 0) // расставляем flag_connect и выбираем какой из двух вариантов оставить
	{
		int index = len_sec;
		int number = 0;
		number = index_f[0];
		for(int l = 1;l<p;l++)
		{
			if(index - number == 1 ) // это работает для len_sec > 1
			{	
				if(len_sec > 1)
					number = index_f[l];
			}
			else
			{
				if(index<2)
				{
					if(fabs(b[index-1])>eps)
						flag_connect[l] = true;
					else
						flag_connect[l] = false;
					number = index_f[l];
				}
				else if(fabs(b[index-2])>eps && fabs(b[index-1])>eps
					       	&& fabs(b[index-1]/b[index-2] - b[index]/b[index-1]) < eps) // тонкий момент
					flag_connect[l] = true;

				else if(fabs(b[index-2])<eps && fabs(b[index-1])>eps)
				{
					flag_connect[l] = true;
					number = index_f[l]; 
				}
				else
				{
					flag_connect[l] = false;
					number = index_f[l];
				}
				l11 = b[ind_l[l]];
				f22 = b[ind_f[l]];
				last1[l] = l11;
				first2[l] = f22;
				index_l[l] = ind_l[l];
				index_f[l] = ind_f[l];
			}
			index+=len_sec;
		}
	}
	//заполнили first1,first2,last1,last2,flag_connect;
	pthread_barrier_wait(a->barrier);
	if(k == 0 )
	{
		for(int l = 1;l<p;l++)
		{
			if(flag_connect[l])
				first1[l] = first2[l-1];
			if(index_l[l]>=index_f[l]) // у него first1[k] и first2[k] должны совпадать
				first2[l] = first1[l];

		}
		for(int l = p-1;l>0;l--)
		{
			if(flag_connect[l])
				last2[l-1] = last1[l];

			if(index_l[l-1]>=index_f[l-1]) // у него last1[k] и last2[k] должны совпадать
                        	last1[l-1] = last2[l-1];
		}
	}
	pthread_barrier_wait(a->barrier);
	f1 = first1[k];
	f2 = first2[k];
	l1 = last1[k];
	l2 = last2[k];
	if(index_l[k]>=index_f[k])
	{
		l1 = l2;
		for(i = ind;i<=index_l[k];i++)
                	b[i] = (l1+f1) / 2;
	}
	else
	{
		for(i = ind;i<=index_l[k];i++)
			b[i] = (l1+f1) / 2;

	
		for(i = index_f[k];i<m;i++)
			b[i] = (l2+f2) / 2;
	}
	q = 0.0;
	double l = 0,f = 0;
	start = index_l[k] + 1;
	end = start;
	for(i = index_l[k] + 1;i < index_f[k]-1;i++)
	{
		if(fabs(b[i])<eps && fabs(q)<eps)
			continue;
		else if(fabs(b[i]) < eps)
		{
			for(int j = start;j<=end;j++)
				b[j] = (l + f) / 2;
			q = 0.0;
		}
		else if(fabs(q)<eps)
		{
			q = b[i+1]/b[i];
			f = b[i];
			l = b[i+1];
		        start = i;
			end = i+1;	
		}
		else if(fabs(q-b[i+1]/b[i])>eps)
		{
			for(int j = start;j<=end;j++)
                                b[j] = (l + f) / 2;
			q = 0.0;
		}
		else
		{
			end = i+1;
			l = b[i+1];
		}
	}
	if(end == index_f[k]-1)
	{
		for(int j = start;j<=end;j++)
                	b[j] = (l + f) / 2;
	}
	t = get_cpu_time() - t;
	a->t = t;
	return nullptr;
}
void analys_segment(int ind_real,int ind,int m,double*b,double*f2,double*l1,int*ind_f,int*ind_l)
{
	double q = 0.0,eps = 1.e-15;
	int i,end = -1;
	for(i = ind;i<m-1;i++)
	{
		if(fabs(b[i])<eps && i==ind_real)
		{
			*l1 = b[i];
			end = i;
			*ind_l = i;
			break;
		}
		else if(fabs(b[i])<eps)
		{
			*l1 = b[i-1];
			end = i-1;
			*ind_l = i-1;
			break;
		}
		else if(fabs(q)<eps)
		{
			q = b[i+1]/b[i];
		}
		else if(fabs(q-b[i+1]/b[i])>eps && i>=ind_real)
		{
			*l1 = b[i];
			end = i;
			*ind_l = i;
			break;
		}
		else if(fabs(q-b[i+1]/b[i])>eps && i<ind_real)
		{
			q = 0.0;
		}
	}
	if(end == -1)
	{
		*l1 = b[m-1];
		*f2 = b[ind_real]; // так как k потоку досталась геом последовательность 
		*ind_l = m-1;
		*ind_f = ind_real; // у нас геом прогрессия в k потоке => идекс first2[k] не нужен
	}
	else
	{
		q = 0.0;
		bool num = false;
		int index = 0;
		for(int i = (*ind_l)+1;i<m-1;i++)
		{
			if(fabs(b[i])<eps)
			{
                        	q = 0.0;
				num = false;
			}
                	else if(fabs(q)<eps)
                	{
                        	q = b[i+1]/b[i];
				num = true;
				index = i;

                	}
                	else if(fabs(q-b[i+1]/b[i])>eps)
                	{
				q = 0.0;
				num = false;
                	}

		}
		if(!num)
		{
			*ind_f = m-1;
			*f2 = b[m-1];
		}
		else
		{
			*ind_f = index;
			*f2 = b[index];
		}
	}
}
