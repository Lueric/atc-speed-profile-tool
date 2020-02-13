/*------------------------------------------------------
**
** File:      common_util.h
** Author:    Eric Lu
** Created:   2018-10-25
**
** Copyright ©2012 Toronto Transit Commission
** 
**
** Function Description
** -----------------------------------------------------
** 
** provide common utility functions and variables for atc_speed_profile_tool
**
** -----------------------------------------------------
** Revision History
**
** 25 OCT 2018: Rev 1.0 - erilu
**                      - Initial Design
**
** Inputs:
** Describe Function/Program Inputs
** - If there are arguments, describe and list in order of entry
**
** Outputs:
** Describe Function/Program Outputs
**
** ----------------------------------------------------
*/

#ifndef ATC_SPEED_PROFILE_COMMON_H
#define ATC_SPEED_PROFILE_COMMON_H

#include <stdio.h>
#include "errorhandler.h"

/*
** Constants
** -----------------------------------------------------
*/

# ifndef _WIN32
#   define PATH_DELIM "/" 
# else
#   define PATH_DELIM "\\" 
# endif

/* collection of array length */
typedef enum stringLength{
    STR_MIN = 10,
    STR_SHORT = 20,
    STR_MEDIUM = 50,
    STR_LONG = 100,
    STR_MAX = 512,
    STR_EXTRA = 1024
} stringLength;

/*
** Structures
** -----------------------------------------------------
*/


/*
** Function Prototypes
** -----------------------------------------------------
*/
/** @brief  trim white space on leading and tailing of a string
 *  @param  *str string pointer to input string
 *  @return trimmed string
 */ 

char *
trim(char *str);

/** @brief copy a substring from the string
 *  @param  *sub string pointer to store the substring
 *  @param  *str string pointer to original string
 *  @param  ind index of original string where the substring starts
 *  @param  len number of characters to include in the substring
 *  @return operation succeed
 *          insufficient buffer size, *sub is untouched
 */ 
errorCode 
substr(char *sub, char *str, int ind, int len);

/** @brief  calculate integration using trapezoidal method
 *  @param  x0   first point x coordinate
 *  @param  y0   first point y coordinate
 *  @param  x1  second point x coordinate
 *  @param  y1  second point y coordinate 
 *  @return integration (discrete trapezoidal estimation)
 */
double
integrate_trapezoidal(double x0, double y0, double x1, double y1);

/** @brief  strip file path and return only file name
 *  @param  *str_path full path and filename string
 *  @return filename string
 */
char *
strip_path(char *str_path);

/** @brief  calculate and return length of a file pointer
 *  @param  *fp file pointer to check length
 *  @return length of a file pointer
 */
long 
file_length(FILE *fp);

/** @brief  copy a file to the target place
 *  @param  *target string of destination file path and name
 *  @param  *source string of original file path and name
 *  @return  YMSS_LOG_NOT_ACCESSIBLE
 *           OPERATION_SUCCEEDED
 */
errorCode 
copy_file(char *target, char *source);

/** @brief  alternative version to strtok, where consecutive delimiters return
 *          "", instead of NULL
 *  @param  *str string to tokenize
 *  @param  *delim delimiters
 *  @return  pointer to token
 *           
 */
char * 
cstrtok(char *str, char delim);

#endif