#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// To test use the following command:
//g++ -pthread time_shm.c ;./a.out ls -l | tee time_shm_out.txt

int main(int argc, char *argv[])
{

    pid_t pid;
    timeval startTime;
	(gettimeofday(&startTime, 0));
	int shmID = shmget(IPC_PRIVATE, 32000, 0600);
    timeval* shmPtr = (timeval*)shmat(shmID, NULL, 0);

    /* fork a child process */
	pid = fork();
	if (pid < 0) { /* error occurred */
		fprintf(stderr, "Fork Failed");
		return 1;
	}
	if (pid > 0) { /* parent process */
		wait(NULL);
		timeval end_time;
		timeval elapsed_time; 
        timeval startTime = *shmPtr;
		
		//	get the end time
		gettimeofday( &end_time, 0 );
		printf("\nEnd time: %ld.%06ld seconds\n", end_time.tv_sec, end_time.tv_usec );
		// calculate elapsed time timeval_t elapsed_time;
		timersub( &end_time, &startTime, &elapsed_time );
        shmdt(shmPtr);
		// print elapsed time (microseconds right justified zero filled)
		printf("\nElapsed time: %ld.%06ld seconds\n", elapsed_time.tv_sec, elapsed_time.tv_usec );
        
	}
	else { /* child process */
        *shmPtr = startTime; 
		//detach
        shmdt(shmPtr);
		printf("\nStart time: %ld.%06ld seconds\n", startTime.tv_sec, startTime.tv_usec );
		// Execute the command "ls -l"
		execvp(argv[1], &argv[2]);
	}

	shmctl (shmID, IPC_PRIVATE, 0);
    return 0;
}