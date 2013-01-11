#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#define __USE_GNU
#include <sched.h>
#undef __USE_GNU
#include <errno.h>
#include <math.h>
#include <pthread.h>

double waste_time(long n);
void *soma_vet(void *args);

struct call_args
{
	double *a, *soma,*c;
	int size,t;
};

int main(int argc, char **argv)
{
	int status;
	cpu_set_t affinity_mask;
	pthread_t tid[2];
	pthread_attr_t attr[2];
	struct call_args my_args[2];
	FILE *inp_a;
	long int size = atol(argv[2]);
	long int i;
	int THREADS_NUMBER=2;
	double *a, *c, *soma;
	a = (double *) malloc(sizeof(double) * size);
	c = (double *) malloc(sizeof(double) * size);
	soma = (double *) malloc(sizeof(double) * THREADS_NUMBER);
	inp_a = fopen(argv[1], "r");

	for (i=0; i<size; i++) 
	{
		fscanf(inp_a, "%lf\n", &a[i]);
	}
	fclose(inp_a);

	for(i=0; i<THREADS_NUMBER; i++)
	{
		my_args[i].a=a;
		my_args[i].c=c;
		my_args[i].soma=soma;
		my_args[i].size=size;
		my_args[i].t=i;
		CPU_ZERO(&affinity_mask);
		CPU_SET(0, &affinity_mask);
		if (sched_setaffinity(0, sizeof(affinity_mask), &affinity_mask) < 0) 
			perror("sched_setaffinity");
		pthread_attr_init(&attr[i]);
		pthread_create(&tid[i], &attr[i],soma_vet, &my_args[i]);
	}
	for(i=0; i<2; i++)
	{
		pthread_join(tid[i],NULL);
	}
	double result= 0.0;
	for(i=0; i<THREADS_NUMBER ; i++) {
		result += soma[i];
	}

	printf("Resultado da soma vetorial = %lf\n", result);
	return 0;
}

double waste_time(long n)
{
    double res = 0;
    long i = 0;
    while(i <n * 200) {
        i++;
        res += sqrt (i);
    }
    return res;
}

void *soma_vet(void *args)
{
	struct call_args *m = (struct call_args *) args;
	int i;
	double soma = 0.0;
	for (i=m->size/2*m->t; i<m->size/2*(m->t+1);i++){
		soma += waste_time(abs(m->a[i]));
	}
	m->soma[m->t] = soma;
}

