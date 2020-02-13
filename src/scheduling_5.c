/** @file   scheduling_5.c
 *  @author Eric Lu
 *  @date   2018-08-08
 *  @ver    5.0
 * 
 *  Copyright ©2018 Toronto Transit Commission
 * 
 *  @brief  Periodically execute the specified Command with 
 *  the rate specified by the timer.
 *  
 *  @input
 *  Command to execute
 * 	Timer to repeat
 *  
 *  
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <windows.h>

#define _VER 5.0

/* Number of seconds used by the Sleep function */
#define DEFAULT_TIMER 60

int curr_yr;
int curr_mon;
int curr_day;
int curr_hrs;
int curr_mins;

/* get_curr_time() gets the system time and calculates the offset from the true time. The function
manipulates the contents of the integer pointers that are sent to the function. */
void get_curr_time(int *curr_yr, int *curr_mon, int *curr_day, int *curr_hrs, int *curr_mins)
{
	SYSTEMTIME str_t;
	GetLocalTime(&str_t);
	*curr_yr = str_t.wYear;
	*curr_mon = str_t.wMonth;
	*curr_day = str_t.wDay;
	*curr_hrs = str_t.wHour;
	*curr_mins = str_t.wMinute;

}

void display_usage(){
	printf("\n\n");
	printf("Program Description: \n");
	printf("Execute the specified command periodically with the specified interval\n\n");
	printf("Usage: \n");
	printf("SCHEDULING_5 [command] [interval]\n\n");
	printf("%12s    command to execute \n", "command");
	printf("%12s    repetition interval in second (s) \n", "interval");
	printf("\n");

}

int main(int argc, char **argv )
{
	char automated_call[BUFSIZ] = ""; 
	int sec = 0;
	int timer = 0;
	FILE *fp = NULL;

	if (argc < 2 || argc > 3){
		display_usage();
		exit(EXIT_SUCCESS);
	}
	
	strcpy(automated_call, argv[1]); // program to run
	if ((fp=fopen(automated_call,"rb"))==NULL){
		fprintf(stderr, "program is not accessible!\n"); 
		exit(EXIT_FAILURE);
	} 
	fclose(fp);

	if (argc == 2){
		timer = DEFAULT_TIMER;
	} else {
		/* timer to restart */
		timer = atoi(argv[2]); 
		if (timer < DEFAULT_TIMER) timer = DEFAULT_TIMER;
	}
	
	get_curr_time(&curr_yr, &curr_mon, &curr_day, &curr_hrs, &curr_mins);
	
	/* Run continuous */
	while(1)
	{
		system("cls");
		system(automated_call);		
		for(sec = timer; sec > 0; sec--)
		{
			get_curr_time(&curr_yr, &curr_mon, &curr_day, &curr_hrs, &curr_mins);
			printf("Running Program %s every %d seconds. \n", automated_call, timer);
			printf("Do not terminate this program.\n");
			printf("Waiting %d seconds to search log files.\n", sec);
			printf("The current system time is %.2d:%.2d on %.2d-%.2d-%.2d\n", curr_hrs, curr_mins,
				   curr_day, curr_mon, curr_yr);
			Sleep(1000);
			system("cls");
		}
	}

	exit(EXIT_SUCCESS);
}

