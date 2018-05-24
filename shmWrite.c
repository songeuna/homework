#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<time.h>
#include<signal.h>

#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/msg.h>

#define SHMSIZE	10

#define BUFFER_SIZE PIPE_BUF
#define FIFO_NAME "/tmp/fifo"

struct my_msg_st{
	long int my_msg_type;
	pid_t W_pid;
	pid_t R_pid;
};

void sigHandler(int sig)
{
	static int count = 0;
	int byteCount;
}

int main(void)
{
	void *shared_Mem = (void *) 0;
	int shmid;
	int *shmaddr;
	int i;
	struct my_msg_st msg;
	int running = 1;
	int msgid;
	int msg_to_receive = 0;

	//pid 얻어오기
	msg.W_pid = getpid();

	//creat message q
	msgid = msgget((key_t)1234, 0666 | IPC_CREAT);

	msg.my_msg_type = 1;

	//sigHandler 등록
	signal(SIGUSR1, sigHandler);

	//msgget error
	if(msgid == -1)
	{
		fprintf(stderr,"msg get failed with error : %d\n",errno);
		exit(EXIT_FAILURE);
	}

	//msgsnd error
	if(msgsnd(msgid, (void *)&msg, sizeof(msg), 0) == -1)
	{
		fprintf(stderr,"msg send failed with error : %d\n",errno);
		exit(EXIT_FAILURE);
	}

	printf("shmWrite PID : %d\n",msg.W_pid);
	
	pause();
	//sleep(5);
	
	//memory q에 있는 Write PID값 읽어오기
	if(msgrcv(msgid, (void *)&msg, sizeof(msg), msg_to_receive, 0) == -1)
	{
		fprintf(stderr,"msg receive failed with error : %d\n",errno);
		exit(EXIT_FAILURE);
	}

	printf("shmWrite PID : %d\n",msg.R_pid);

	


	//공유메모리에 데이터 쓰기
	
	// 1. shmget
	shmid = shmget((key_t)1234, sizeof(int) *SHMSIZE, 0666 | IPC_CREAT);
	if(shmid == -1)
	{
		fprintf(stderr, "shmget railed\n");
		exit(EXIT_FAILURE);
	}
	
	// 2. shmat 
	shared_Mem = shmat(shmid,(void *)0, 0);
	if(shared_Mem == (void *) -1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}
	printf("Memory attached at 0x%p\n",shared_Mem);
	shmaddr = (int *)shared_Mem;

	// 3. memory access //data write
	for(i = 1; i < SHMSIZE; i++)
	{
		*(shmaddr + i) = i + 1;
		printf("shmaddr:%p, data:%d\n", shmaddr + i, *(shmaddr + i));
	}
	
	kill(msg.R_pid, SIGUSR1);

	pause();

	
	// 4. shmdt
	if(shmdt(shared_Mem) == -1)
	{
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}								

	// 5. shmctl : IPC_RMID
	if(shmctl(shmid, IPC_RMID, 0) == -1)
	{
		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}
						
	exit(EXIT_SUCCESS);
}
						
