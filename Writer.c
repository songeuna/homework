#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//#include<wait.h>
#include<unistd.h>
#include<signal.h>
#include<errno.h>
#include<time.h>

#include<sys/wait.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define SHMSIZE 1024

void sigHandler(int sig)
{
}

int main()
{	
	pid_t p_pid;
	pid_t c_pid;

	//shared memory 변수
	int shmid;
	int *shmaddr;
	void *shared_Mem = (void *) 0; //NULL로 초기화

	int stat_val;
	char buffer[BUFSIZ];

	// shmget
	shmid = shmget((key_t)1234, sizeof(int) * SHMSIZE, 0666 | IPC_CREAT);
	if(shmid == -1)
	{
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}

	// shmat
	shared_Mem = shmat(shmid, (void *)0, 0);
	if(shared_Mem == (void *) -1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}

	shmaddr = (int *)shared_Mem;

	//sigHandler 등록
	signal(SIGUSR1, sigHandler);
	
	
	//Creat fork
	c_pid = fork();

	switch(c_pid)
	{
		case -1: //fork error
			perror("Fork error");
			exit(1);

		case 0: //child process
			
			pause();
				
			p_pid = getppid();

			execl("./Reader","./Reader",shmaddr,NULL);

		default: //parent process
		
			printf("-----PARENT PROCESS/ DATA SAVE-----\n");
			//memory access
			for(int i = 0; i < SHMSIZE; i++)
			{
				*(shmaddr + i) = i + 1;
				printf("shmaddr : %p, data : %d\n",shmaddr + i, *(shmaddr + i ));
			}

			
			kill(c_pid,SIGUSR1);
	}


	c_pid = wait(&stat_val); // 자식 프로세스가 끝나면 stat_val에 값을 리턴/ 정상종료시 종료된 자식프로세스 PID
}
