#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "errorhandler.h"
#include "log.h"

int 
main (int argc, char *argv[])
{
  int input = 0;

  char buff[70];
  struct tm end_time = {.tm_year=2018-1900, // = year 2018
                        .tm_mon=11-1,    // = 11th month
                        .tm_mday=8,   // = 8th day
                        .tm_hour=7,   // = 15 hours
                        .tm_min=42,   // = 30 minutes
                        .tm_sec=0    // = 20 secs
  };
  time_t rawtime = {0};
  time_t endtime = {0};
  struct tm *cur_time;
  
  time(&rawtime);
  cur_time = localtime(&rawtime);

  printf("\n");
  printf("current time:\n");
  printf("tm_year [%d]\n", cur_time->tm_year);
  printf("tm_mon [%d]\n", cur_time->tm_mon);
  printf("tm_mday [%d]\n", cur_time->tm_mday);
  printf("tm_hour [%d]\n", cur_time->tm_hour);
  printf("tm_min [%d]\n", cur_time->tm_min);
  printf("tm_sec [%d]\n", cur_time->tm_sec);
  printf("tm_yday [%d]\n", cur_time->tm_yday);
  printf("tm_wday [%d]\n", cur_time->tm_wday);
  printf("tm_isdst [%d]\n", cur_time->tm_isdst);
  printf("timestamp [%ld]\n", rawtime);

  printf("\n");
  printf("end time:\n");
  printf("tm_year [%d]\n", end_time.tm_year);
  printf("tm_mon [%d]\n", end_time.tm_mon);
  printf("tm_mday [%d]\n", end_time.tm_mday);
  printf("tm_hour [%d]\n", end_time.tm_hour);
  printf("tm_min [%d]\n", end_time.tm_min);
  printf("tm_sec [%d]\n", end_time.tm_sec);
  printf("tm_yday [%d]\n", end_time.tm_yday);
  printf("tm_wday [%d]\n", end_time.tm_wday);
  printf("tm_isdst [%d]\n", end_time.tm_isdst);

  endtime = mktime(&end_time);
  printf("timestamp [%ld]\n", endtime);


  while (0)
  {
    printf("Enter a value: \n");
    scanf("%d", &input);
    if (0 == input)
    {
      exit(EXIT_SUCCESS);
    }
    else if (input > 0)
    {
      printf("error %d thrown: %s\n", 
             throw_err(input), 
             get_err_description(input));
      log_info("error %d thrown: %s\n", 
                throw_err(input), 
                get_err_description(input));
    }
    else
    {
      printf("error %d thrown: %s\n", 
             throw_err(input), 
             get_err_description(input));
      log_info("error %d thrown: %s\n", 
                throw_err(input), 
                get_err_description(input));
      printf("Try again\n");

    }

  }

  exit(EXIT_SUCCESS);
}
