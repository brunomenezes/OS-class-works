#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

//To use defines such as IPC_PRIVATE and IPC_CREAT 
#include <sys/ipc.h>

//To use shared memory
#include <sys/shm.h>

/*Will explain and use later
#define __USE_GNU
#include <sched.h>
#undef __USE_GNU
#include <errno.h>
#define NUMBER_OF_CORES 4*/
#include <math.h>



//Just to waste more time in computation than a single add operation
double waste_time(double n)
{
    double res = 0;
    long i = 0;
    while(i <n * 200) {
        i++;
        res += sqrt(i);
    }
    return res;
}

int main(int argc, char **argv)
{
	/*Will explain and use later
	//unsigned long int affinity_mask;
	cpu_set_t affinity_mask;
	CPU_ZERO(&affinity_mask);*/

	pid_t pid;
	FILE *inp_a, *inp_b, *out_c;
	//reading the vector size from command line argument
	long int size = atol(argv[3]);
	//reading the number of processes from command line argument
	int np = atoi(argv[4]);

	long int i;
	int j;
	double *a, *c;
	//vector allocation - malloc just for A.  C will be shared
	a = (double *) malloc(sizeof(double) * size);

	//opening 1 input files - name passed as command line argument
	inp_a = fopen(argv[1], "r");

	for (i=0; i<size; i++) 
	{
		//reading input values to vectors A
		fscanf(inp_a, "%lf\n", &a[i]);
	}
	//closing input files
	fclose(inp_a);

	//Creating shared memory region for output vector
	int sid_c = shmget(IPC_PRIVATE, sizeof(double)*size, SHM_R|SHM_W|IPC_CREAT);

	
	//process creation loop
	for (j=0; j<np; j++)
	{	
		/*Will explain and use later
		CPU_ZERO(&affinity_mask);
		CPU_SET((j+1)%NUMBER_OF_CORES, &affinity_mask);
		if (sched_setaffinity(0, sizeof(affinity_mask), &affinity_mask) < 0) 
			perror("sched_setaffinity 1");*/

		//creating child processes
		pid=fork();

		//If error...	
		if (pid < 0) { //erro
        		fprintf(stderr, "Fork %d falhou!\n", j);
        		return 1;
		}
		else if (pid == 0) { //child
			//attach shared memory region (vector C)
			c = (double *) shmat(sid_c, NULL, 0);
			double partial_sum = 0.0;
			//computed a portion of the output related to process J
			for (i=((size/np)*j); i<((size/np)*(j+1)); i++) 
			{
				partial_sum+=a[i] ; //making a sum of the elements
			}
			//detach shared region
			c[j] = partial_sum; // storing the result on the shared variable
			shmdt(c);
			//child must exit loop (only parent will keep creating processes)
			break;
		}
	}
	
	//parent only
	if (pid>0)
	{
		//wait for all children to finish
		for(i=0; i<np; i++)
			wait();
		//attach shared memory region (vector C)
		c = (double *) shmat(sid_c, NULL, 0);

		//open output file
		out_c = fopen(argv[2], "w");
		double result = 0.0; // variable to sum the final result
		for (i=0; i<np; i++) {
			result += c[i];
		}
			//write output
		fprintf(out_c, "%lf\n", result);
		//close file
		fclose(out_c);
		//detach shared region
		shmdt(c);
		//Destroy shared region
		shmctl(sid_c, IPC_RMID, NULL);
		
		//freeing memory
		free(a);
	}
	return 0;
}
