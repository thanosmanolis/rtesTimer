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

//! Global variables
int full_time_sec_, full_time_usec_, dt_sec_, dt_usec_;
float full_time, dt;

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
			printf("Insert file name:\n");
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
	int new_dt_sec, new_dt_usec;
	float new_dt, total;
	float shift_sum = 0;

	//! Store the timestamp at times[] table
	struct timeval timestamp;
	gettimeofday(&timestamp, NULL);
	times[0] = timestamp;

	//! Sleep for dt
	sleep(dt_sec_);
	usleep(dt_usec_);

	int i=1;
	do
	{
		//! Store the timestamp at times[] table
		gettimeofday(&timestamp, NULL);
		times[i] = timestamp;

		total = (double)times[i].tv_sec - (double)times[0].tv_sec + 0.000001*((double)times[i].tv_usec - (double)times[0].tv_usec);

		if(total+dt <= full_time)
		{
			//! Calculate the average sleep offset until now, and abstract it from next sleep
			shift_sum += (double)(times[i].tv_sec - times[i-1].tv_sec + 0.000001*(times[i].tv_usec - times[i-1].tv_usec) - dt);
			new_dt = dt - shift_sum/i;
			new_dt_sec = (int)new_dt;
			new_dt_usec = 1000000*(new_dt - new_dt_sec);

			//! Sleep for dt
			sleep(new_dt_sec);
			usleep(new_dt_usec);
		}

		i++;
	}while( (i < size) && (total + dt <= full_time) );

	size = i;
	return size;
}
