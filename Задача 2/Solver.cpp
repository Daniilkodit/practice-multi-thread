#include "header.h"
#include<iostream>
void* solver(void* arg)
{
	args* a = (args*)arg;

	int k = a->k,p = a->p,res =0;
	int global_err=0,i;
	
	double x,loc_min=0,mean=0;
	char* name = a->filename;
	std::ifstream in;
	in.open(name);
	if (!in.is_open())
	{
		a->err[k] = -1;
		pthread_barrier_wait(a->barrier);
        	return (void*)(intptr_t)-1;
	}
	if(!(in>>loc_min))
	{
		in.close();
		a->err[k] = -3; //если файл пустой то cчитаю что у него нет локального минимума
		pthread_barrier_wait(a->barrier);
		return (void*)(intptr_t)-3; 
	}
	while((in>>x))
		if(x<loc_min) loc_min = x;
	if(!in.eof())
	{
		a->err[k] = -2;
		in.close();
		pthread_barrier_wait(a->barrier);
		return (void*)(intptr_t)-2;
	}
	else
	{
		in.close();
		a->loc_min[k] = loc_min;
	}
	pthread_barrier_wait(a->barrier);
        for(i=0;i<p;i++)
		global_err+=a->err[i];
        if(global_err)
                return (void*)(intptr_t)-1;  
	for(i=0;i<p;i++)
		mean += a->loc_min[i];
	mean = mean/p;

	in.open(name);
	while((in>>x))
		if(x<mean) res++;
	in.close();
	a->res = res;
	return (void*)(intptr_t)res;
}	
