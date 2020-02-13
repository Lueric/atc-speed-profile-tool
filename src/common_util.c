/*------------------------------------------------------
**
** File:      common_util.c
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

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "common_util.h"
#include "errorhandler.h"

/*
** Source Code
** -----------------------------------------------------
*/
static char str_temp[STR_MAX] = "";
static char *p_tok;
static char *p_ret;
static int n_tok = 1;
/* 
** Supporting Functions (If any)
*/

/** @brief  trim white space on leading and tailing of a string
 *  @param  *str string pointer to input string
 *  @return trimmed string
 */ 

char *
trim(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

/** @brief copy a substring from the string
 *  @param  *sub string pointer to store the substring
 *  @param  *str string pointer to original string
 *  @param  ind index of original string where the substring starts
 *  @param  len number of characters to include in the substring
 *  @return operation succeed
 *          insufficient buffer size, *sub is untouched
 */ 
errorCode 
substr(char *sub, char *str, int ind, int len){

    /* check length of sub and str */
    if (str != NULL && sub != NULL && (strlen(str) >= ind+len) && 
        len <= STR_MAX) {
        memcpy(sub, &str[ind], len);
        sub[len] = '\0';

        return throw_err(err_no_error);
    }
    /* insufficient buffer size */
    else return throw_err(err_insufficient_buffer_size);  

}

/** @brief  calculate integration using trapezoidal method
 *  @param  x0   first point x coordinate
 *  @param  y0   first point y coordinate
 *  @param  x1  second point x coordinate
 *  @param  y1  second point y coordinate 
 *  @return integration (discrete trapezoidal estimation)
 */
double
integrate_trapezoidal(double x0, double y0, double x1, double y1)
{
  return (y0+y1)*(x1-x0)/2;
}

/** @brief  strip file path and return only file name
 *  @param  *str_path full path and filename string
 *  @return filename string
 */
char *
strip_path(char *str_path)
{
  
  char *token = NULL;
  char *token_prev = NULL;

  if (NULL == str_path)
  {
    return NULL;
  }
  if (strlen(str_path) >= STR_MAX)
  {
    /* overflow */
    return NULL;
  }
  strcpy(str_temp, str_path);
  token = strtok(str_temp, PATH_DELIM);
  while (NULL != token)
  {
    token_prev = token;
    token = strtok(NULL, PATH_DELIM);
  }
  return token_prev;

}

/** @brief  calculate and return length of a file pointer
 *  @param  *fp file pointer to check length
 *  @return length of a file pointer
 */
long 
file_length(FILE *fp){
    long flen = 0;
    /* go to the end of file */
    fseek(fp, 0, SEEK_END);
    /* get the end position */
    flen = ftell(fp);
    /* go to the beginning of file */
    rewind(fp); 
    return flen;
}

/** @brief  copy a file to the target place
 *  @param  *target string of destination file path and name
 *  @param  *source string of original file path and name
 *  @return  YMSS_LOG_NOT_ACCESSIBLE
 *           OPERATION_SUCCEEDED
 */
errorCode 
copy_file(char *target, char *source){
    FILE *fp_target = NULL;
    FILE *fp_source = NULL;
    char buff = 0;

    fp_source = fopen(source, "rb");
    if (fp_source == NULL) {
        /* source file not exist */
        return throw_err(err_file_not_accessible); 
    }

    fp_target = fopen(target, "wb");
    if (fp_target == NULL) {
        /* target file can not be created */
        fclose(fp_source);
        return throw_err(err_file_not_accessible); 
    }
    /* read a byte from source file */
    buff = fgetc(fp_source); 
    if (feof(fp_source)) {
        /* file is empty, nothing to copy */
        fclose(fp_source);
        fclose(fp_target);
        return throw_err(err_no_error); 
    }
    
    do {
        /* write the byte to target file */
        fputc(buff, fp_target); 
        /* read a byte from source file */
        buff = fgetc(fp_source); 
    } while (!feof(fp_source));

    fclose(fp_source);
    fclose(fp_target);
    return throw_err(err_no_error);
}

/** @brief  alternative version to strtok, where consecutive delimiters return
 *          "", instead of NULL
 *  @param  *str string to tokenize
 *  @param  *delim delimiters
 *  @return  pointer to token
 *           
 */
char * 
cstrtok(char *str, char delim)
{
  if (NULL != str)
  /* new string to tokenize */
  {
    p_ret = str;
    p_tok = str;
    n_tok = 1;
  }
  else
  {
    p_ret = p_tok;
    if (0 == n_tok)
    {
      return NULL;
    }
  }
  if ( delim <= 0 )
  {
    return p_ret;
  }
  else
  {
    while ( (*p_tok != 0) && (*p_tok != delim) )
    {
      p_tok++;
    }
    if (delim == *p_tok)
    {
      *p_tok = '\0';
      p_tok++;
      n_tok++;
    }
    n_tok--;
    return p_ret;
  }
}