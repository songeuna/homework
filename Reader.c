#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<time.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>

#include<sys/wait.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define SHMSIZE 1024

int main(void)
{
	void *shared_Mem;
	int shmid;
	int *shmaddr;
	int fd;
	char buffer[BUFSIZ];


	fd = open("file.txt", O_RDWR | O_CREAT | O_TRUNC, \
			S_IRWXU | S_IWGRP | S_IRGRP | S_IROTH);
	
	if(fd != -1)
	{
	
		// shmget
		shmid = shmget((key_t)1234, sizeof(int) * SHMSIZE, 0666 | IPC_CREAT);
		if(shmid == -1)
		{
			fprintf(stderr,"shmget failed\n");
			exit(EXIT_FAILURE);
		}

		//shmat
		shared_Mem = shmat(shmid, (void *)0, 0);
		if(shared_Mem == (void *) -1)
		{
			fprintf(stderr, "shmat failed\n");
			exit(EXIT_FAILURE);
		}
	
		shmaddr = (int *)shared_Mem;

		// memory access
		printf("-----READER.c / DATA READ-----\n");

		for(int i = 0; i < SHMSIZE ; i++)
		{
			printf("shmaddr : %p, data: %d\n", shmaddr + i, *(shmaddr + i));
			sprintf(buffer, "shmaddr : %p, data: %d\n",shmaddr+i,*(shmaddr+i));
			write(fd,&buffer,strlen(buffer));
		}

		// shmdt
		if(shmdt(shared_Mem) == -1)
		{
			fprintf(stderr, "shmdt failed\n");
			exit(EXIT_FAILURE);
		}
	
		// shmctl
		if(shmctl(shmid, IPC_RMID, 0) == -1)
		{
			fprintf(stderr,"shmctl(IPC_RMID) failed\n");
			exit(EXIT_FAILURE);
		}
	}

	close(fd);
	exit(EXIT_SUCCESS);
	

}

