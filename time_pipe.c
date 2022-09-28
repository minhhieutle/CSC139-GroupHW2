#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#define READ_END 0
#define WRITE_END 1

// To test use the following command:
//g++ -pthread time_pipe.c ;./a.out ls -l | tee time_shm_out.txt

int main(int argc, char *argv[])
{
	int fd[2];
	pid_t pid;
	timeval startTime; 
	/* create the pipe */
	if (pipe(fd) == -1) {
		fprintf(stderr,"Pipe failed");
		return 1;
	}
	/* fork a child process */
	pid = fork();
	if (pid < 0) { /* error occurred */
		fprintf(stderr, "Fork Failed");
		return 1;
	}
	if (pid > 0) { /* parent process */
		/* close the unused end of the pipe */
		close(fd[WRITE_END]);
		wait(NULL);
		timeval end_time;
		timeval startTime; 
		timeval elapsed_time; 
		/* read from the pipe */
		read(fd[READ_END], &startTime, sizeof startTime);
		//	get the end time
		gettimeofday( &end_time, 0 );
		printf("\nEnd time: %ld.%06ld seconds\n", end_time.tv_sec, end_time.tv_usec );
		// calculate elapsed time timeval_t elapsed_time;
		timersub( &end_time, &startTime, &elapsed_time );
		// print elapsed time (microseconds right justified zero filled)
		printf("\nElapsed time: %ld.%06ld seconds\n", elapsed_time.tv_sec, elapsed_time.tv_usec );
		/* close the read end of the pipe */
		close(fd[READ_END]);
	}
	else { /* child process */
		/* close the unused end of the pipe */
		close(fd[READ_END]);
		
		// Record start time
		
		gettimeofday( &startTime, 0 );
		printf("\nStart time: %ld.%06ld seconds\n", startTime.tv_sec, startTime.tv_usec );
		write(fd[WRITE_END], &startTime, sizeof startTime);
		// Execute the command "ls -l"
		execvp(argv[1], &argv[2]);
		/* close the write end of the pipe */
		close(fd[WRITE_END]);
	}
	return 0;
}