#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    pid_t pid;
    int NUM_CHILDS=10; 
    int i, v=0, pids[NUM_CHILDS]; 
    for(i=0 ; i<NUM_CHILDS ; i++)
    {  
        pid = fork();  
        if (pid < 0) { //erro
            fprintf(stderr, "Fork falhou!\n");
            return 1;
        }
        else if (pid == 0) { //processo filho
            printf("\nprocesso pid %d", getpid());
            break;
        }
        else { //processo pai 
            pids[v]=wait(NULL);
            v++;
        }
    }
    if(pid != 0)
    {
       for(i=0 ; i<NUM_CHILDS;i++)
       {
        printf("\nprocesso filho %d acabou! meu pid é %d",pids[i],getpid());
       }
       printf("\n");
    }
    return 0;
}
