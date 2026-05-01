#include "header.h"
void* solver(void* arg)
{
	args* a = (args*)arg;
	bool start_posl = false;
		
	int k = a->k,p = a->p,res =0;
	int global_err=0,i;
	
	double x,y,eps = 1.e-15;
	char* name = a->filename;
	std::ifstream in;
	in.open(name);
	if (!in.is_open())
	{
		a->err[k] = -1;
		pthread_barrier_wait(a->barrier);
        	return (void*)(intptr_t)-1;
	}
	if(!(in>>y))
	{
		a->res = res;
		in.close();
		pthread_barrier_wait(a->barrier);
		for(i=0;i<p;i++)
			global_err+=a->err[i];
		if(global_err)
			return (void*)(intptr_t)-1;
		return (void*)(intptr_t)res;
	}
	while((in>>x))
	{
		if(fabs(x-y)<eps)
			start_posl = true;
		else if (start_posl)
		{
			res++;
			start_posl = false;
		}
		y = x;
	}
	if(start_posl)
		res++;
	if(in.eof())
	{
		in.close();
		a->res = res;
	}
	else 
	{
		a->err[k] = -2;
		in.close();
		pthread_barrier_wait(a->barrier);
		return (void*)(intptr_t)-2;
	}
	pthread_barrier_wait(a->barrier);
        for(i=0;i<p;i++)
		global_err+=a->err[i];
        if(global_err)
                return (void*)(intptr_t)-1;  
	return (void*)(intptr_t)res;
}
