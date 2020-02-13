#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <windows.h>
#include <time.h>

/*
** Header File - stdconst.h
** Copyright �2011 Toronto Transit Commission
** 
**
** Function Description
** -----------------------------------------------------
** 
** This is a header file containing all the constands and the
** standard libraies. It is linked to SCS_station_extract_Q.c
** and unarchiving_dir().
** 
** -----------------------------------------------------
** Revision History
**
** 01 12 2011: Rev 1.0 - mpanesar
**                     - file created 
**
** March 9 2012: Rev 1.1 - ewchan
**                       - Added new constant APPLICATION_CYCLE
**
** September 17 2012: Rev 1.2 - ewchan
**                            - Update number of stations to support Bay Lower
**                            - Update version to 1.3.1
**
** September 20 2012: Rev 1.3 - ewchan
**                            - Update version to 1.3.2
**
** November 30 2012: Rev 1.4 - ewchan
**                           - Update version to 1.3.3
**
** March 6 2013: Rev 1.5 - ewchan
**                       - Update version to 2.0.0
**                       - Major release to support acceleration plots
** 
** May 14 2013: Rev 1.6 - ewchan
**                      - Update version to 2.0.1
**                      - To distinguish with the older 2.0.0 that
**                        was never tested with real acceleration
**                        data
**
** June 11 2019: Rev 4.1 - erilu
**                       - Updated version to 4.1
**                       - Increased STATION_ARRAY_SIZE to 156 for TYSSE
**
*/
/*
** Type Definitions
** -----------------------------------------------------
*/
#define boolean int
#define bool int

/*
** Constants
** -----------------------------------------------------
*/

#define VERSION_NAME "4.1"  /* This the version of the software displayed to the user for the SCS Station Extract Program */
#define VERSION_NAME_MULTIPLOT "1.3.0" /* Version name for the Multiplot tool */

#define STATION_ARRAY_SIZE 156    /* Number of stations (both travel directions) */
#define STATION_NAME_SIZE 6      /* Max size of station name */
#define TITLE_NAME_SIZE 100      /* Max Size of Station Graph Title */
#define CHAINAGE_NAME_SIZE 10    /* Max size of Chainage name */

#define LOWER_SIG_TAG_LIMIT 6000 /* Range of valid Signal Tag IDs */
#define UPPER_SIG_TAG_LIMIT 9999

#define TRUE 1     /* For use with boolean type def */
#define FALSE 0    /* For use with boolean type def */

#define LOWER_RANGE_SH_LINE_ID 65  /* Range of Sheppard Line Station IDs - only stations for metric calculations */
#define UPPER_RANGE_SH_LINE_ID 74

#define NO_NEW_SIGNAL_TAG_DETECTED -1 /* Values to indicate when a new signal tag read  */
#define SIGNAL_TAG_DETECTED        25 /* Values selected to allow for clear Excel plots */
#define POSITION_LOST               5
#define CHAINAGE_TAG               50 

#define RUN_UNARCHIVE              10 /* Maximun number of times unarchiving_dir() will run*/ 
#define MAX_FILE_NUMBER           100 /* Number(#) of file name in close#.txt */

#define MIN_DISTANCE_TO_ALLOW_SIG_REPRINT 30 /* If signals are more than 10 metres apart, allow re-printing of repeat signals */

#define NORMAL_MODE 0  /* Most users shall run the program in normal (default) mode */
#define DEBUG_MODE  1  /* Program under debug mode will print to screen extra info for debugging */

#define MAX_OUTPUT_FILE_NUM 100      /* Maximum number of files per platform output log files */
                                     /* Worst case, in a 24 h period for 1 log file - take SH line as shortest */
                                     /* 3 round trips per hour - 1 hour = 3 platform stops * 24 hours = 72 log files */
                                     
#define MAX_TMP_FILE_NUM 800 /* Worst case of 72 log files per platform. Total of 10 */
                                               /* 72 files * 10 platforms = 720 */ 

#define DATA_FILE "TRANS.dat" /* Data file for reading in tag IDs and corresponding signal names */

#define STATION_STOP_PERMITTED_SPEED  34  /* 34 km/h or lower to start checking if train is stopped at a station */

#define APPLICATION_CYCLE 0.100  /* SCS Application Cycle in [s] */

/* Counters (in s) to keep the Command Open after the program completes */
#define COUNTER_NORMAL_EXIT  10
#define COUNTER_ERROR_EXIT   30

#ifdef WINDOWS /* If the OS is windows, GetCurrentDir uses _getcwd */     
    #include <direct.h>     
    #define GetCurrentDir _getcwd 
#else /* For all other OS's, use getcwd */     
    #include <unistd.h>     
    #define GetCurrentDir getcwd  
#endif 


/* 
** Supporting Functions (If any)
** -----------------------------------------------------
*/
/* This is the unarhiving function, it unarchives wgnuplot.exe. 
** Its agrument is the current working directory of the program. 
*/ 

int unarchiving_dir(char * directory_path); 
