#include "header.h"
#include<iostream>
void* solver(void* arg)
{
	args* a = (args*)arg;

	double eps = 1.e-15,res = 0;
	int k = a->k,p = a->p;
	int i,count = 1,p_start = p;
	double x,y;
	double* first = a->first;
	int* first_count = a->first_count;
	double* last = a->last;
	int* last_count = a->last_count;
	int* count_max = a->count_max;
	double* posl_max = a->posl_max;
	bool*flag = a->flag;
	char* name = a->filename;

	std::ifstream in;
	in.open(name);

	if (!in.is_open())
	{
		a->err = -1;
		a->my_err = -1;
		double temp[2] = {a->thread_out, a->err};
		reduce_sum(p, temp, 2);
		a->thread_out = temp[0];
		a->err = temp[1];
		pthread_barrier_wait(a->barrier);
		return (void*)(intptr_t)-1;
	}
	if(!(in>>y))
	{

		if(in.eof())
		{
			in.close();
			a->res = 0; //если файл пустой
			a->thread_out = 1;
			a->number_neighbours[k] = -1;
			double temp[2] = {a->thread_out, a->err};
			reduce_sum(p, temp, 2);
			a->thread_out = temp[0];
			a->err = temp[1];
		pthread_barrier_wait(a->barrier);
			if(a->err<0)
				return (void*)(intptr_t)-1;
			a->die_not = false;
		}
		else
		{
			a->err = -2;
			a->my_err = -2;
			in.close();
			double temp[2] = {a->thread_out, a->err};
			reduce_sum(p, temp, 2);
			a->thread_out = temp[0];
			a->err = temp[1];
		pthread_barrier_wait(a->barrier);
			return (void*)(intptr_t)-2;
		}

	}
	if(a->die_not)
	{
		first[k] = y;
		first_count[k] = 1;
		count = 1;
		bool fl = true;
		while(in>>x)
		{
			if(fabs(x-y)<eps)
				count++;
			else if(fl)
			{

				posl_max[k] = y;
				count_max[k] = count;
				first_count[k] = count;
				y = x;
				fl = false;
				count =  1;	// т.е насчитали начало
			}
			else 
			{
				if(count>count_max[k])
				{
					posl_max[k] = y;
					count_max[k] = count;
				}
				count = 1;
			}
			y = x;
		}
		if(fl)
		{
			first_count[k] = count;
			flag[k] = true;
		}
		last_count[k] = count;
		last[k] = y;

		if(count>count_max[k])        
		{                             
			posl_max[k] = y;      
			count_max[k] = count; 
		}                             	

		if(!in.eof())
		{	
			a->err = -2;
			a->my_err = -2;
			in.close();
			double temp[2] = {a->thread_out, a->err};
			reduce_sum(p, temp, 2);
			a->thread_out = temp[0];
			a->err = temp[1];
			pthread_barrier_wait(a->barrier);
			p -= a->thread_out;              
			return (void*)(intptr_t)-2;
		}

		in.close();
		double temp[2] = {a->thread_out, a->err};
		reduce_sum(p, temp, 2);
		a->thread_out = temp[0];
		a->err = temp[1];
		p -= a->thread_out;              
		pthread_barrier_wait(a->barrier);

		if(a->err<0) 
			return nullptr;
		i = 1;
		int number_neighbours = -1;
		if(k == 0)
			number_neighbours = 0;
		while(k-i>=0) // ищем соседей
		{
			if(a->number_neighbours[k-i]!=-1)
			{
				number_neighbours = k-i;
				break;
			}
			else if(k==i)
			{
				number_neighbours = -2; // типо начало
			}
			i++;
		}
		reduce_sum(p,&(a->err),1);
		a->number_neighbours[k] = number_neighbours;
		a->thread_out = 0;
	}
	if(k == 0)
	{

		for(i = 0;i<p_start;i++)
		{
			if(a->number_neighbours[i] == -1)
				continue;

			else
				break;

		}
		int start = i;
		double prev = first[start];
		count = 0;
		int mcount =0;
		double loc_max1 = prev;
		for(i = start;i<p_start;i++)
		{
			if(a->number_neighbours[i] == -1)
				continue;
			if(fabs(prev-first[i])<eps && flag[i])
			{
				count += first_count[i];
			}
			else
			{
				if(fabs(prev-first[i])<eps)	
					count+= first_count[i];
				if(mcount<count)
				{
					mcount = count;
					loc_max1 = prev;
				}
				count = last_count[i];
				prev = last[i];
			}
		}
		if(mcount<count)
		{
			mcount = count;
			loc_max1 = prev;
		}
		int mcount2 = 0;
		int loc_max2 = 0;
		for(i = start;i<p_start;i++)
		{
			if(a->number_neighbours[i] == -1)
				continue;
			if(mcount2<count_max[i])
			{
				mcount2 = count_max[i];
				loc_max2 = posl_max[i];
			}
		}
		if(mcount>mcount2)
			posl_max[0] = loc_max1;
		else
			posl_max[0] = loc_max2;
		pthread_barrier_wait(a->barrier);


	}
	else
		pthread_barrier_wait(a->barrier);
	if(a->die_not)
	{

		in.open(name);
		res = 0;
		double amax = posl_max[0];
		while(in>>x)
		{
			if(x>amax)
				res++;	
		}
		a->res = res;
		in.close();
		pthread_barrier_wait(a->barrier);
		return (void*)(intptr_t)res;
	}
	else 
		pthread_barrier_wait(a->barrier);
	return nullptr;
}	
