#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>

#define clear() printf("\033[H\033[J")
#define RED "\033[0;31m"
#define RESET_COLOR "\033[0m"

//! Random functions
int getValues (struct timeval times[], int size);

//! Alarms
void alarmDt1 (int sig);
void alarmDt2 (int sig);

//! Global variables
int full_time_sec_, full_time_usec_, dt_sec_, dt_usec_;
float full_time, dt;
volatile sig_atomic_t footprint_dt_ = 0;
struct timeval timestamp;

int main(int argc, char **argv)
{
	char answer[1];

    printf("Insert whole duration below: ");
	scanf("%f", &full_time);
	printf("Insert dt: ");
	scanf("%f", &dt);
	printf("Do you want to store the output to a file? (y/n): ");
	scanf("%s", answer);

	if( strcmp(answer, "y") && strcmp(answer, "n") )
		printf(RED "Undefined answer\n" RESET_COLOR);
	else
	{
		full_time_sec_ = (int)full_time;
		full_time_usec_ = 1000000*(full_time - full_time_sec_);
		dt_sec_ = (int)dt;
		dt_usec_ = 1000000*(dt - dt_sec_);

		int size = (int)(full_time/dt) + 1;
		struct timeval times[size];

		if(!strcmp(answer, "y"))
		{
			char file_name[100];
			printf("Insert exact path to file:\n");
			scanf("%s", file_name);

			FILE *fp;
			//! Open the file for writing
			fp = fopen (file_name,"w");

			size = getValues(times, size);

			for(int i=1; i<size; i++)
				fprintf(fp, "%f\n", (double)(times[i].tv_sec - times[i-1].tv_sec + 0.000001*(times[i].tv_usec - times[i-1].tv_usec)));

			//! Close the file
			fclose (fp);
		}else if(!strcmp(answer, "n"))
		{
			size = getValues(times, size);

			for(int i=1; i<size; i++)
			printf("%f\n", (double)(times[i].tv_sec - times[i-1].tv_sec + 0.000001*(times[i].tv_usec - times[i-1].tv_usec)));
		}
	}

	return 0;
}

int getValues(struct timeval times[], int size)
{
	//! Store the timestamp at times[] table
	gettimeofday(&timestamp, NULL);
	times[0] = timestamp;

	int i=1;
	while(i < size)
	{
		//! Call an alarm for dt
		footprint_dt_ = 0;
		if(dt_sec_ == 0)
		{
			ualarm(dt_usec_, 1);
			signal(SIGALRM, alarmDt2);
		}else
		{
			alarm(dt_sec_);
			signal(SIGALRM, alarmDt1);
		}

		while(footprint_dt_ == 0)
			; //! Do nothing

		//! Store the timestamp at times[] table
		times[i] = timestamp;

		i++;
	}

	size = i;
	return size;
}

void alarmDt1(int sig)
{
	signal(SIGALRM, SIG_IGN);
	if(dt_usec_ != 0)
	{
		signal(SIGALRM, alarmDt2);
		ualarm(dt_usec_, 1);
	}else
	{
		gettimeofday(&timestamp, NULL);
		footprint_dt_ = 1;
	}
}

void alarmDt2(int sig)
{
	signal(SIGALRM, SIG_IGN);

	gettimeofday(&timestamp, NULL);
	footprint_dt_ = 1;
}
