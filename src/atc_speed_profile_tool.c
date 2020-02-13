/*------------------------------------------------------
**
** File: atc_speed_profile_tool.c
** Author: Eric Lu
** Created: 2018-10-25
**
** Copyright ©2012 Toronto Transit Commission
** 
**
** Function Description
** -----------------------------------------------------
** 
** Analyze traindata.csv files generated from CSS playback files, generate 
** run profiles (position vs speed) per train per station
**
** -----------------------------------------------------
** Revision History
**
** 25 OCT 2018: Rev 1.0 - erilu
**                      - Initial Design
**
** Inputs:
** Configuration Files:
**  config\atc_block_lut.csv
** Train Data Files:
**  traindata.csv (Generated from CSS playback files)
**
** Outputs:
** Run Profile Files:
** run_profiles\*.csv
**
** ----------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <inttypes.h>
#include "simclist.h"
#include "errorhandler.h"
#include "common_util.h"
#include "atc_speed_profile_tool.h"

/*
** Type Definitions
** -----------------------------------------------------
*/


/*
** Constants
** -----------------------------------------------------
*/


/*
** Structures
** -----------------------------------------------------
*/


/*
** Variables
** -----------------------------------------------------
*/


/* system operation errno */
extern int errno;
/* linked list to store input data */
static list_t input_data_list;
static uint64_t input_list_id = 0;
/* linked list to store output data */
static list_t output_data_list;
static uint64_t output_list_id = 0;
/* linked list to store lut data */
static list_t lut_data_list;
static uint64_t lut_list_id = 0;
/* unique run profile count */
static uint64_t run_cnt = 0;

/*
** Function Prototypes
** -----------------------------------------------------
*/

/*
** Source Code
** -----------------------------------------------------
*/

/* 
** Supporting Functions (If any)
*/

/* required functions to manipulate the linked list */
/* comparator function required for list_sort function */
/* using the event sorting_str to sort the list */
int 
input_data_comparator(const void *a, const void *b) 
{
  const inputData *A = (inputData *) a;
  const inputData *B = (inputData *) b;

  return strcmp(A->sorting_str, B->sorting_str);
}

/* meter function required for the list_append function */
/* return size of the inputData structure */
size_t 
input_data_meter(const void *el) 
{
  return sizeof(inputData);
}

/* required functions to manipulate the linked list */
/* comparator function required for list_sort function */
/* using the event sorting_str to sort the list */
int 
output_data_comparator(const void *a, const void *b) 
{
  const outputData *A = (outputData *) a;
  const outputData *B = (outputData *) b;

  return strcmp(A->sorting_str, B->sorting_str);
}

/* meter function required for the list_append function */
/* return size of the outputData structure */
size_t 
output_data_meter(const void *el) 
{
  return sizeof(outputData);
}

/* required functions to manipulate the linked list */
/* comparator function required for list_sort function */
/* using the event sorting_str to sort the list */
int 
lut_data_comparator(const void *a, const void *b) 
{
  const lutData *A = (lutData *) a;
  const lutData *B = (lutData *) b;

  return strcmp(A->sorting_str, B->sorting_str);
}

/* meter function required for the list_append function */
/* return size of the lutData structure */
size_t 
lut_data_meter(const void *el) 
{
  return sizeof(lutData);
}

/** @brief  add input data structure to the static input data list
 *  @param  *data  pointer to the new input data structure 
 *  @return err_list_append_failed
 *      
 */
int 
add_to_input_data_list(inputData *data) 
{
  
  if (1 != list_append(&input_data_list, data)) 
  {
      return throw_err(err_list_append_failed);
  }  

  return 0;
}

/** @brief  add output data structure to the static output data list
 *  @param  *data  pointer to the new output data structure 
 *  @return err_list_append_failed
 *      
 */
int 
add_to_output_data_list(outputData *data) 
{
  
  if (1 != list_append(&output_data_list, data)) 
  {
      return throw_err(err_list_append_failed);
  }  

  return 0;
}

/** @brief  add lookup table data structure to the static lut data list
 *  @param  *data  pointer to the new lut data structure 
 *  @return err_list_append_failed
 *      
 */
int 
add_to_lut_data_list(lutData *data) 
{
  
  if (1 != list_append(&lut_data_list, data)) 
  {
      return throw_err(err_list_append_failed);
  }  

  return 0;
}

/** @brief  convert inputHeader and format into string 
 *  @param  *data  inputData to be converted 
 *  @return NULL      input is not valid
 *          converted static string             
 */
char *
input_header_to_string()
{
  strcpy(str_static_data, "");
  snprintf(str_static_data, 
          STR_EXTRA, 
          "[                 col_time][%d]\n[             col_location][%d]\n[                col_block][%d]\n[           col_run_number][%d]\n[            col_direction][%d]\n[     col_destination_code][%d]\n[     col_origination_code][%d]\n[       col_schedule_class][%d]\n[                col_cc_id][%d]\n[ col_current_driving_mode][%d]\n[col_selected_driving_mode][%d]\n[            col_talkative][%d]\n[               col_motion][%d]\n[            col_skip_stop][%d]\n[           col_doors_open][%d]\n[           col_door_fault][%d]\n[                col_alarm][%d]\n[      col_emergency_brake][%d]\n[                col_speed][%d]", 
          input_header.col_time,
          input_header.col_location,
          input_header.col_block,
          input_header.col_run_number,
          input_header.col_direction,
          input_header.col_destination_code,
          input_header.col_origination_code,
          input_header.col_schedule_class,
          input_header.col_cc_id,
          input_header.col_current_driving_mode,
          input_header.col_selected_driving_mode,
          input_header.col_talkative,
          input_header.col_motion,
          input_header.col_skip_stop,
          input_header.col_doors_open,
          input_header.col_door_fault,
          input_header.col_alarm,
          input_header.col_emergency_brake,
          input_header.col_speed);

  return str_static_data;
}

/** @brief  convert inputData and format into string 
 *  @param  *data  inputData to be converted 
 *  @return NULL      input is not valid
 *          converted static string  
 *             
 */
char *
input_data_to_string(inputData *data)
{
  char tmp[STR_EXTRA] = "";
  if (NULL == data)
  {
    return NULL;
  }
  
  snprintf(tmp, STR_EXTRA, "%010" PRIu64 ",", data->id);
  strcpy(str_static_data, tmp);
  if (NULL != data->sorting_str)
  {    
    strcat(str_static_data, data->sorting_str);
  }
  strcat(str_static_data, ",");
  if (NULL != data->input_data_file)
  {    
    strcat(str_static_data, data->input_data_file);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_timestamp)
  {    
    strcat(str_static_data, data->str_timestamp);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_time)
  {    
    strcat(str_static_data, data->str_time);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_location)
  {    
    strcat(str_static_data, data->str_location);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_block)
  {    
    strcat(str_static_data, data->str_block);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_run_number)
  {    
    strcat(str_static_data, data->str_run_number);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_direction)
  {    
    strcat(str_static_data, data->str_direction);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_destination_code)
  {    
    strcat(str_static_data, data->str_destination_code);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_origination_code)
  {    
    strcat(str_static_data, data->str_origination_code);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_schedule_class)
  {    
    strcat(str_static_data, data->str_schedule_class);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_cc_id)
  {    
    strcat(str_static_data, data->str_cc_id);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_current_driving_mode)
  {    
    strcat(str_static_data, data->str_current_driving_mode);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_selected_driving_mode)
  {    
    strcat(str_static_data, data->str_selected_driving_mode);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_talkative)
  {    
    strcat(str_static_data, data->str_talkative);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_motion)
  {    
    strcat(str_static_data, data->str_motion);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_skip_stop)
  {    
    strcat(str_static_data, data->str_skip_stop);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_doors_open)
  {    
    strcat(str_static_data, data->str_doors_open);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_door_fault)
  {    
    strcat(str_static_data, data->str_door_fault);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_alarm)
  {    
    strcat(str_static_data, data->str_alarm);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_emergency_brake)
  {    
    strcat(str_static_data, data->str_emergency_brake);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_speed)
  {    
    strcat(str_static_data, data->str_speed);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_from_station)
  {    
    strcat(str_static_data, data->str_from_station);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_to_station)
  {    
    strcat(str_static_data, data->str_to_station);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_direction_code)
  {    
    strcat(str_static_data, data->str_direction_code);
  }
  strcat(str_static_data, ",");

  snprintf(tmp, STR_EXTRA, "%010ld,%03d,%d,", 
           data->timestamp, 
           data->cc_id, 
           data->direction);
  strcat(str_static_data, tmp);
  snprintf(tmp, STR_EXTRA, "% .1f,", 
           data->signed_measured_speed_km_h);
  strcat(str_static_data, tmp);

  if (NULL != data->str_station_code)
  {    
    strcat(str_static_data, data->str_station_code);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_platform)
  {    
    strcat(str_static_data, data->str_platform);
  }
  strcat(str_static_data, ",");

  snprintf(tmp, STR_EXTRA, "%d,%d", 
           data->is_platform, 
           data->is_motion);
  strcat(str_static_data, tmp);

  return str_static_data;
}

/** @brief  convert lutData and format into string 
 *  @param  *data  lutData to be converted 
 *  @return NULL      input is not valid
 *          converted static string  
 *             
 */
char *
lut_data_to_string(lutData *data)
{
  char tmp[STR_EXTRA] = "";
  if (NULL == data)
  {
    return NULL;
  }

  snprintf(tmp, STR_EXTRA, "%010" PRIu64 ",", data->id);
  strcpy(str_static_data, tmp);
  if (NULL != data->sorting_str)
  {    
    strcat(str_static_data, data->sorting_str);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_id)
  {    
    strcat(str_static_data, data->str_id);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_location)
  {    
    strcat(str_static_data, data->str_location);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_block)
  {    
    strcat(str_static_data, data->str_block);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_direction)
  {    
    strcat(str_static_data, data->str_direction);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_direction_code)
  {    
    strcat(str_static_data, data->str_direction_code);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_direction_num)
  {    
    strcat(str_static_data, data->str_direction_num);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_platform)
  {    
    strcat(str_static_data, data->str_platform);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_station_code)
  {    
    strcat(str_static_data, data->str_station_code);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_block_length)
  {    
    strcat(str_static_data, data->str_block_length);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_from_station)
  {    
    strcat(str_static_data, data->str_from_station);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_to_station)
  {    
    strcat(str_static_data, data->str_to_station);
  }
  strcat(str_static_data, ",");

  snprintf(tmp, STR_EXTRA, "%d,%f,%d", 
           data->direction,
           data->block_length,
           data->is_platform);
  strcat(str_static_data, tmp);

  return str_static_data;
}

/** @brief  convert outputData and format into string 
 *  @param  *data  outputData to be converted 
 *  @return NULL      input is not valid
 *          converted static string  
 *             
 */
char *
output_data_to_string(outputData *data)
{
  char tmp[STR_EXTRA] = "";
  if (NULL == data)
  {
    return NULL;
  }

  snprintf(tmp, STR_EXTRA, "%010" PRIu64 ",", data->id);
  strcpy(str_static_data, tmp);
  if (NULL != data->sorting_str)
  {    
    strcat(str_static_data, data->sorting_str);
  }
  strcat(str_static_data, ",");
  if (NULL != data->input_data_file)
  {    
    strcat(str_static_data, data->input_data_file);
  }
  strcat(str_static_data, ",");
  if (NULL != data->output_data_file)
  {    
    strcat(str_static_data, data->output_data_file);
  }
  strcat(str_static_data, ",");

  snprintf(tmp, STR_EXTRA, "%010" PRIu64 ",", data->input_data_id);
  strcat(str_static_data, tmp);
  
  if (NULL != data->str_timestamp)
  {    
    strcat(str_static_data, data->str_timestamp);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_cc_id)
  {    
    strcat(str_static_data, data->str_cc_id);
  }
  strcat(str_static_data, ",");

  snprintf(tmp, STR_EXTRA, "%f,", data->log_time_s);
  strcat(str_static_data, tmp);

  if (NULL != data->segment_id)
  {    
    strcat(str_static_data, data->segment_id);
  }
  strcat(str_static_data, ",");

  snprintf(tmp, STR_EXTRA, "%f,%f,%f,%f,%f,%d,%d,", 
           data->distance_travelled_0_m,
           data->distance_travelled_1_m,
           data->accum_distance_travelled_ft,
           data->permitted_speed_km_h,
           data->measured_speed_km_h,
           data->current_tag_id,
           data->ti_tag);
  strcat(str_static_data, tmp);

  if (NULL != data->signal_name)
  {    
    strcat(str_static_data, data->signal_name);
  }
  strcat(str_static_data, ",");
  if (NULL != data->signal_name_graphing)
  {    
    strcat(str_static_data, data->signal_name_graphing);
  }
  strcat(str_static_data, ",");

  snprintf(tmp, STR_EXTRA, "%f,%f,", 
           data->civil_speed_km_h, 
           data->travel_time_s);
  strcat(str_static_data, tmp);

  if (NULL != data->str_from_station)
  {    
    strcat(str_static_data, data->str_from_station);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_to_station)
  {    
    strcat(str_static_data, data->str_to_station);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_direction_code)
  {    
    strcat(str_static_data, data->str_direction_code);
  }
  strcat(str_static_data, ",");

  snprintf(tmp, STR_EXTRA, "%010ld,%03d,", 
           data->timestamp, 
           data->cc_id);
  strcat(str_static_data, tmp);

  if (NULL != data->str_station_code)
  {    
    strcat(str_static_data, data->str_station_code);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_platform)
  {    
    strcat(str_static_data, data->str_platform);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_direction_num)
  {    
    strcat(str_static_data, data->str_direction_num);
  }
  strcat(str_static_data, ",");

  snprintf(tmp, STR_EXTRA, "%02d", data->run_cnt);
  strcat(str_static_data, tmp);

  return str_static_data;
}

/** @brief  print block look up table on the screen
 *  @param  none
 *  @return errorCode
 *             
 */
int 
display_lut_data_list()
{
  int err = 0;
  lutData *p_data = NULL;
  bool b_enabled = true;

  /* print all lut data */  
  if ( list_empty(&lut_data_list) )
  {
    return throw_err(err_lut_is_empty);    
  }
  else
  {
    /* iteration start */
    /* check err, 0 - not able to start iteration; 1 - ok to iterate */
    if (1 != list_iterator_start(&lut_data_list))
    {
      /* iteration not able to start */
      return throw_err(err_list_iteration_failed);
    }
    else 
    {
      while (list_iterator_hasnext(&lut_data_list) && (b_enabled)) 
      { 
        /* check for next element */
        p_data = (lutData *)list_iterator_next(&lut_data_list);
        if (NULL == p_data)
        {
          /* error on retrieving element from list */
          err = throw_err(err_list_retrieval_failed);
          b_enabled = false;
        }
        else
        {
          fprintf
          (
            stdout, 
            "[%6s][%6s][%s]\n", 
            "TRACE", 
            "LUT", 
            lut_data_to_string(p_data)
          );
        }
      }

      if (1 != list_iterator_stop(&lut_data_list))
      {
        /* iteration not able to stop */
        return throw_err(err_list_stop_failed);
      }
      else
      {
        return err;
      }
    }
  }
}

/** @brief  print input data list on the screen
 *  @param  none
 *  @return none
 *             
 */
int 
display_input_data_list()
{
  int err = 0;
  inputData *p_data = NULL;
  bool b_enabled = true;

  /* print all input data */ 
  if ( list_empty(&input_data_list) )
  {
    return throw_err(err_list_is_empty);
  }
  else
  {
    /* iteration start */
    /* check err, 0 - not able to start iteration; 1 - ok to iterate */
    if (1 != list_iterator_start(&input_data_list))
    {
      /* iteration not able to start */
      return throw_err(err_list_iteration_failed);
    }
    else 
    {
      while (list_iterator_hasnext(&input_data_list) && (b_enabled)) 
      { 
        /* check for next element */
        p_data = (inputData *)list_iterator_next(&input_data_list);
        if (NULL == p_data)
        {
          /* error on retrieving element from list */
          err = throw_err(err_list_retrieval_failed);
          b_enabled = false;
        }
        else
        {
          fprintf
          (
            stdout, 
            "[%6s][%6s][%s]\n", 
            "TRACE", 
            "INPUT", 
            input_data_to_string(p_data)
          );
        }
      }
      if (1 != list_iterator_stop(&input_data_list))
      {
        /* iteration not able to stop */
        return throw_err(err_list_stop_failed);
      }
      else 
      {
        return err;
      }
    }
  }
}

/** @brief  print output data list on the screen
 *  @param  none
 *  @return none
 *             
 */
int 
display_output_data_list()
{
  int err = 0;
  outputData *p_data = NULL;
  bool b_enabled = true;

  /* print output data list */
  
  if ( list_empty(&output_data_list) )
  {
    return throw_err(err_list_is_empty);
  }
  else
  {
    /* iteration start */
    /* check err, 0 - not able to start iteration; 1 - ok to iterate */
    if (1 != list_iterator_start(&output_data_list))
    {
      /* iteration not able to start */
      return throw_err(err_list_iteration_failed);
    }
    else 
    {
      while (list_iterator_hasnext(&output_data_list) && (b_enabled)) 
      { 
        /* check for next element */
        p_data = (outputData *)list_iterator_next(&output_data_list);
        if (NULL == p_data)
        {
          /* error on retrieving element from list */
          err = throw_err(err_list_retrieval_failed);
          b_enabled = false;
        }
        else
        {
          fprintf
          (
            stdout, 
            "[%6s][%6s][%s]\n", 
            "TRACE", 
            "OUTPUT", 
            output_data_to_string(p_data)
          );
        }
      }
      if (1 != list_iterator_stop(&output_data_list))
      {
        /* iteration not able to stop */
        return throw_err(err_list_stop_failed);
      }
      else
      {
        return err;
      }
    }
  }
}

/** @brief  initialize lookup list
 *  @param  none
 *  @return none
 *             
 */
void 
init_lut_data_list()
{
  list_init(&lut_data_list);
  list_attributes_copy(&lut_data_list, lut_data_meter, 1);
  list_attributes_comparator(&lut_data_list, lut_data_comparator);
}

/** @brief  initialize input data list
 *  @param  none
 *  @return none
 *             
 */
void 
init_input_data_list()
{
  /* list initialization */
  list_init(&input_data_list);
  list_attributes_copy(&input_data_list, input_data_meter, 1);
  list_attributes_comparator(&input_data_list, input_data_comparator);
}

/** @brief  initialize output data list
 *  @param  none
 *  @return none
 *             
 */
void 
init_output_data_list()
{
  /* list initialization */
  list_init(&output_data_list);
  list_attributes_copy(&output_data_list, output_data_meter, 1);
  list_attributes_comparator(&output_data_list, output_data_comparator);
}

/** @brief  sort input data list
 *  @param  none
 *  @return none
 *             
 */
int 
sort_input_data_list()
{
  if (list_empty(&input_data_list))
  {
    return throw_err(err_list_is_empty);    
  }
  else
  {
    list_sort(&input_data_list, -1);
    return 0;
  }
}

/** @brief  clear and free lookup table list
 *  @param  none
 *  @return none
 *             
 */
void 
free_lut_data_list()
{
  list_destroy(&lut_data_list);
}

/** @brief  clear and free input data list
 *  @param  none
 *  @return none
 *             
 */
void 
free_input_data_list()
{
  list_destroy(&input_data_list);
}

/** @brief  clear and free output data list
 *  @param  none
 *  @return none
 *             
 */
void 
free_output_data_list()
{
  list_destroy(&output_data_list);
}

/** @brief  convert date time formatted string to time_t seconds format
 *      this function validates the str_date_time, returns negative if date is 
 *      not valid. 
 *  @param  *str_date_time  date time string in "2018/08/27 20:00:00" 
 *  @return err_date_not_valid    negative on error
 *          timestamp in seconds  
 *             
 */
time_t
str_to_seconds(const char *str_date_time)
{
  struct tm tm_time = {0};
  time_t timestamp = -1;

  if ((NULL == str_date_time) || (strlen(str_date_time) < 19))
  {
    return throw_err(err_file_format_not_valid);
  }
  /* parse date time string to tm_time */
  /* 2018/08/27 20:00:00 */  
  sscanf( str_date_time, 
          "%d/%d/%d %d:%d:%d", 
          &tm_time.tm_year, 
          &tm_time.tm_mon,
          &tm_time.tm_mday,
          &tm_time.tm_hour,
          &tm_time.tm_min,
          &tm_time.tm_sec
  );

  /* year is >= 1900 */
  if (tm_time.tm_year < 1900)
  {
    return throw_err(err_date_not_valid);
  }
  tm_time.tm_year -= 1900;
  /* month >= 1 and <=12 */
  if ((tm_time.tm_mon > 12) || (tm_time.tm_mon < 1))
  {
    return throw_err(err_date_not_valid);
  }
  tm_time.tm_mon -= 1;
  /* day >= 1 and <= 31 */
  if ((tm_time.tm_mday > 31) || (tm_time.tm_mday < 1))
  {
    return throw_err(err_date_not_valid);
  }
  /* day >= 1 and <= 31 */
  if ((tm_time.tm_mday > 31) || (tm_time.tm_mday < 1))
  {
    return throw_err(err_date_not_valid);
  }
  /* hour >= 0 and <= 23 */
  if ((tm_time.tm_hour > 23) || (tm_time.tm_hour < 0))
  {
    return throw_err(err_date_not_valid);
  }
  /* minute >= 0 and <= 59 */
  if ((tm_time.tm_min > 59) || (tm_time.tm_min < 0))
  {
    return throw_err(err_date_not_valid);
  }
  /* second >= 0 and <= 59 */
  if ((tm_time.tm_sec > 59) || (tm_time.tm_sec < 0))
  {
    return throw_err(err_date_not_valid);
  }

  timestamp = mktime(&tm_time);
  if (-1 == timestamp) 
  {
    return throw_err(err_date_not_valid);
  }
  return timestamp;
}

/** @brief  check if a line is header
 *      traverse through the line until reach end, count number of digits
 *      in the line, compare to the limit, return true if less than the limit
 *  @param  *line  line string in question 
 *  @param  *num_digit_hi  number of digits high limit
 *  @return true    line is header
 *          false   line is not header
 */
bool 
is_header(char *line, int num_digit_hi)
{
  /* header is a line with number of digits less than num_digit */
  int cnt = 0;
  char *p_line = line;

  if ((NULL == line) || (strlen(line) <= 1))
  {
    return false;
  }
  while ('\0' != *p_line)
  {
    if(*p_line >= '0' && *p_line <= '9')
    {
      cnt++;
    }
    p_line++;
  }
  if(cnt <= num_digit_hi)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/** @brief  check if a line is in valid csv format with minimum number of 
 *            columns
 *      traverse through the line until reach end, count number of delimiter
 *      ',' in the case of csv, compare to the limit, return true if more 
 *      than the limit
 *  @param  *line  line string in question 
 *  @param  *num_col_lo  number of columns low limit
 *  @return true    line is valid csv
 *          false   line is not valid csv
 */
bool
is_csv_line(char *line, int num_col_lo)
{
  /* valid csv line contain ',' more than or equal num_col)lo-1 */
  int cnt = 0;
  char *p_line = line;

  if ((NULL == line) || (strlen(line) <= 1))
  {
    return false;
  }
  while ('\0' != *p_line)
  {
    if (',' == *p_line)
    {
      cnt++;
    }
    p_line++;
  }
  if (cnt >= (num_col_lo - 1))
  {
    return true;
  }
  else
  {
    return false;
  }
}

/** @brief  parse the header line and determine the column location for each
 *            variables.
 *      split the line with delimiter ",", match token to header name and 
 *        store column number to input header.
 *  @param  *line  header line string 
 *  @return                 0     header parsed successfully
 *          err_missing_header    some header info is missing
 */
int
parse_input_header(char *line)
{
  char delim = ',';
  char *token = NULL;
  int num = 0;
  int i = 0;
  bool b_header_covered[k_header_cnt] = {false};
  
  token = cstrtok(line, delim);

  while (NULL != token)
  {
    if (0 == strcmp(trim(token), k_header_names[k_header_id_time]))
    {
      input_header.col_time = num;
      b_header_covered[k_header_id_time] = true;
    }
    else if (0 == strcmp(trim(token), k_header_names[k_header_id_location]))
    {
      input_header.col_location = num;
      b_header_covered[k_header_id_location] = true;
    }
    else if (0 == strcmp(trim(token), k_header_names[k_header_id_block]))
    {
      input_header.col_block = num;
      b_header_covered[k_header_id_block] = true;
    }
    else if (0 == strcmp(trim(token), k_header_names[k_header_id_run_number]))
    {
      input_header.col_run_number = num;
      b_header_covered[k_header_id_run_number] = true;
    }
    else if (0 == strcmp(trim(token), k_header_names[k_header_id_direction]))
    {
      input_header.col_direction = num;
      b_header_covered[k_header_id_direction] = true;
    }
    else if (0 == strcmp(trim(token), k_header_names[k_header_id_destination_code]))
    {
      input_header.col_destination_code = num;
      b_header_covered[k_header_id_destination_code] = true;
    }
    else if (0 == strcmp(trim(token), k_header_names[k_header_id_origination_code]))
    {
      input_header.col_origination_code = num;
      b_header_covered[k_header_id_origination_code] = true;
    }
    else if (0 == strcmp(trim(token), k_header_names[k_header_id_schedule_class]))
    {
      input_header.col_schedule_class = num;
      b_header_covered[k_header_id_schedule_class] = true;
    }
    else if (0 == strcmp(trim(token), k_header_names[k_header_id_cc_id]))
    {
      input_header.col_cc_id = num;
      b_header_covered[k_header_id_cc_id] = true;
    }
    else if (0 == strcmp(trim(token), k_header_names[k_header_id_current_driving_mode]))
    {
      input_header.col_current_driving_mode = num;
      b_header_covered[k_header_id_current_driving_mode] = true;
    }
    else if (0 == strcmp(trim(token), k_header_names[k_header_id_selected_driving_mode]))
    {
      input_header.col_selected_driving_mode = num;
      b_header_covered[k_header_id_selected_driving_mode] = true;
    }
    else if (0 == strcmp(trim(token), k_header_names[k_header_id_talkative]))
    {
      input_header.col_talkative = num;
      b_header_covered[k_header_id_talkative] = true;
    }
    else if (0 == strcmp(trim(token), k_header_names[k_header_id_motion]))
    {
      input_header.col_motion = num;
      b_header_covered[k_header_id_motion] = true;
    }
    else if (0 == strcmp(trim(token), k_header_names[k_header_id_skip_stop]))
    {
      input_header.col_skip_stop = num;
      b_header_covered[k_header_id_skip_stop] = true;
    }
    else if (0 == strcmp(trim(token), k_header_names[k_header_id_doors_open]))
    {
      input_header.col_doors_open = num;
      b_header_covered[k_header_id_doors_open] = true;
    }
    else if (0 == strcmp(trim(token), k_header_names[k_header_id_door_fault]))
    {
      input_header.col_door_fault = num;
      b_header_covered[k_header_id_door_fault] = true;
    }
    else if (0 == strcmp(trim(token), k_header_names[k_header_id_alarm]))
    {
      input_header.col_alarm = num;
      b_header_covered[k_header_id_alarm] = true;
    }
    else if (0 == strcmp(trim(token), k_header_names[k_header_id_emergency_brake]))
    {
      input_header.col_emergency_brake = num;
      b_header_covered[k_header_id_emergency_brake] = true;
    }
    else if (0 == strcmp(trim(token), k_header_names[k_header_id_speed]))
    {
      input_header.col_speed = num;
      b_header_covered[k_header_id_speed] = true;
    }
    else
    {
      ;
    }
    token = cstrtok(NULL, delim);
    num++;
  }

  while ((b_header_covered[i]) && (i < k_header_cnt))
  {
    i++;
  }
  if (i < k_header_cnt)
  {
    return throw_err(err_missing_header);
  }
  else
  {
    return 0; 
  }    
}

/** @brief  parse the data line string into the input_data structure, 
 *      convert time string into timestamp, validate the cc number, 
 *      calculate the sorting string, and generate id. returns negative on
 *      error.
 *  @param  *input_data  pointer to the new input data structure 
 *  @param  *str_data_line  input data string
 *  @param  *str_data_file  input data file
 *  @return err_cc_not_valid
 *          err_date_not_valid
 *          err_maximum_number_exceeded
 */
int 
parse_input_data(inputData *input_data, char *str_data_line, char *str_data_file)
{
  char delim = ',';
  char *token = NULL;
  char col_data[k_header_cnt][STR_LONG] = {""};
  
  int i = 0;
  int cc_num = 0;
  time_t timestamp = -1;
  char *p_temp;

  token = cstrtok(str_data_line, delim);
  while ((NULL != token) && (i < k_header_cnt))
  {
     strcpy(col_data[i], trim(token));
     token = cstrtok(NULL, delim);
     i++;
  }

  /* str_time[STR_MEDIUM] */
  strcpy(input_data->str_time, col_data[input_header.col_time]);

  /* str_location[STR_MEDIUM] */
  strcpy(input_data->str_location, col_data[input_header.col_location]);
  /* str_block[STR_SHORT] */
  strcpy(input_data->str_block, col_data[input_header.col_block]);
  /* str_run_number[STR_MIN] */  
  strcpy(input_data->str_run_number, col_data[input_header.col_run_number]);
  /* str_direction[STR_MIN] */
  strcpy(input_data->str_direction, col_data[input_header.col_direction]);
  /* str_destination_code[STR_MIN] */
  strcpy(input_data->str_destination_code, col_data[input_header.col_destination_code]);
  /* str_origination_code[STR_MIN] */
  strcpy(input_data->str_origination_code, col_data[input_header.col_origination_code]);
  /* str_schedule_class[STR_MIN] */
  strcpy(input_data->str_schedule_class, col_data[input_header.col_schedule_class]);
  /* str_cc_id[STR_MIN] */
  strcpy(input_data->str_cc_id, col_data[input_header.col_cc_id]);
  /* str_current_driving_mode[STR_MIN] */
  strcpy(input_data->str_current_driving_mode, col_data[input_header.col_current_driving_mode]);
  /* str_selected_driving_mode[STR_MIN] */
  strcpy(input_data->str_selected_driving_mode, col_data[input_header.col_selected_driving_mode]);
  /* str_talkative[STR_MIN] */
  strcpy(input_data->str_talkative, col_data[input_header.col_talkative]);
  /* str_motion[STR_MIN] */
  strcpy(input_data->str_motion, col_data[input_header.col_motion]);
  /* str_skip_stop[STR_MIN] */
  strcpy(input_data->str_skip_stop, col_data[input_header.col_skip_stop]);
  /* str_doors_open[STR_MIN] */
  strcpy(input_data->str_doors_open, col_data[input_header.col_doors_open]);
  /* str_door_fault[STR_MIN] */
  strcpy(input_data->str_door_fault, col_data[input_header.col_door_fault]);
  /* str_alarm[STR_MIN] */
  strcpy(input_data->str_alarm, col_data[input_header.col_alarm]);
  /* str_emergency_brake[STR_MIN] */
  strcpy(input_data->str_emergency_brake, col_data[input_header.col_emergency_brake]);
  /* str_speed[STR_MIN] */
  strcpy(input_data->str_speed, col_data[input_header.col_speed]);

  /* id */
  if (LIST_MAX_SIZE == input_list_id)
  {
    /* reach the maximum number of input list id */
    return throw_err(err_maximum_number_exceeded);
  }
  input_data->id = input_list_id++;
  /* input_data_file */
  strcpy(input_data->input_data_file, strip_path(str_data_file));

  /* YYYYMMDDHHMMSS converted from str_time */
  timestamp = str_to_seconds(input_data->str_time);
  if (0 > timestamp){
    input_data->timestamp = -1;
    return throw_err(err_date_not_valid);
  }
  input_data->timestamp = timestamp;
  snprintf(input_data->str_timestamp, STR_MEDIUM, "%010ld", timestamp);
  
  /* sorting value "nnnYYYYMMDDHHMMSS" */
  /* combination of str_cc_id, str_timestamp */
  cc_num = (int) strtol(input_data->str_cc_id, &p_temp, 10);
  if ((cc_num <= 0) || (cc_num > 999))
  {
    /* cc number is not valid */
    input_data->cc_id = 0;
    return throw_err(err_cc_not_valid);
  }
  input_data->cc_id = cc_num;
  snprintf(input_data->sorting_str, STR_MEDIUM, "%03d", cc_num);
  strcat(input_data->sorting_str, input_data->str_timestamp);

  if ((NULL == input_data->str_motion) || 
      (strlen(input_data->str_motion) != 1))
  {
    //input_data->is_motion = -1;
    return throw_err(err_motion_not_valid);
  }
  if ('0' == input_data->str_motion[0])
  {
    input_data->is_motion = false;
  } 
  else if ('1' == input_data->str_motion[0])
  {
    input_data->is_motion = true;
  } 
  else
  {
    //input_data->is_motion = -1;
    return throw_err(err_motion_not_valid);
  } 

  /* str_from_station initialize */
  strcpy(input_data->str_from_station, "");
  /* str_to_station initialize */
  strcpy(input_data->str_to_station, "");
  /* str_direction_code initialize */
  strcpy(input_data->str_direction_code, "");  
  /* str_station_code initialize */
  strcpy(input_data->str_station_code, "");
  /* str_platform initialize */
  strcpy(input_data->str_platform, "");

  /* direction initialize */
  input_data->direction = 0;
  /* signed_measured_speed_km_h initialize */
  input_data->signed_measured_speed_km_h = 0;
  /* is_platform initialize */
  input_data->is_platform = 0;  

  /* line parsed ok, no error found */
  return 0; 
}

/** @brief  parse lookup table data line string into the lut_data structure, 
 *      convert id, direction number, block length, validate block, direction,
 *      returns negative on error.
 *  @param  *input_data  pointer to the new lut data structure 
 *  @param  *str_data_line  input data string
 *  @return err_file_format_not_valid
 *          err_maximum_number_exceeded
 */
int 
parse_lut_data(lutData *input_data, char *str_data_line)
{
  char delim = ',';
  char *token = NULL;
  char col_data[LUT_HEADER_CNT][STR_LONG] = {""};
  char str_buf[STR_MAX] = "";
  
  int i = 0;
  int block_num = 0;
  char *p_temp = NULL;

  token = cstrtok(str_data_line, delim);
  while ((NULL != token) && (i < k_lut_header_cnt))
  {
     strcpy(col_data[i], trim(token));
     token = cstrtok(NULL, delim);
     i++;
  }

  /* str_id[STR_MIN] */
  strcpy(input_data->str_id, col_data[0]);
  input_data->id = (uint64_t) strtol(input_data->str_id, &p_temp, 10);
  /* str_location[STR_MEDIUM] */
  strcpy(input_data->str_location, col_data[1]);
  /* str_block[STR_SHORT] */
  strcpy(input_data->str_block, col_data[2]);
  /* str_direction[STR_MIN] */
  strcpy(input_data->str_direction, col_data[3]);
  /* str_direction_code[STR_MIN] */
  strcpy(input_data->str_direction_code, col_data[4]);
  /* str_direction_num[STR_MIN] */
  strcpy(input_data->str_direction_num, col_data[5]);
  /* str_platform[STR_MIN]	*/
  strcpy(input_data->str_platform, col_data[6]);
  /* str_station_code[STR_MIN]	*/
  strcpy(input_data->str_station_code, col_data[7]);
  /* str_block_length[STR_SHORT] */
  strcpy(input_data->str_block_length, col_data[8]);
  sscanf(input_data->str_block_length, "%lf", &input_data->block_length);
  /* str_from_station[STR_MEDIUM] */
  strcpy(input_data->str_from_station, col_data[9]);
  /* str_to_station[STR_MEDIUM] */
  strcpy(input_data->str_to_station, col_data[10]);

  /* check str_direction_num */
  if (  (NULL == input_data->str_direction_num) ||
        (strlen(input_data->str_direction_num) > 1) ||
        ( (0 != strcmp(input_data->str_direction_num, "1")) && 
          (0 != strcmp(input_data->str_direction_num, "2"))  ))
  {
    return throw_err(err_file_format_not_valid);
  }
  input_data->direction = 
    (int) strtol(input_data->str_direction_num, &p_temp, 10);
  /* check str_platform */
  if (  (NULL == input_data->str_platform) ||
        (strlen(input_data->str_platform) > 1) )  
  {
    return throw_err(err_file_format_not_valid);
  }
  if (1 == strlen(input_data->str_platform))
  {
    input_data->is_platform = true;
  }
  else
  {
    input_data->is_platform = false;
  }

  /* sorting value "nnnYYYYMMDDHHMMSS" */
  /* combination of str_block, str_direction */
  /* check input_data->str_block */
  p_temp = NULL;  
  if (NULL == input_data->str_block)
  {
    return throw_err(err_file_format_not_valid);
  }
  strcpy(str_buf, input_data->str_block);
  if (  (NULL == strtok(str_buf, "_")) || 
        (NULL == (p_temp = strtok(NULL, "_"))) )
  {
    return throw_err(err_file_format_not_valid);
  }
  else if ( (block_num = strtol(p_temp, &p_temp, 10)) <= 0 )
  {
    return throw_err(err_file_format_not_valid);
  }

  /* check input_data->str_direction */
  if (  (NULL == input_data->str_direction) ||
        (strlen(input_data->str_direction) > 1) ||
        ( (0 != strcmp(input_data->str_direction, "R")) &&
          (0 != strcmp(input_data->str_direction, "L"))  ))
  {
    return throw_err(err_file_format_not_valid);
  }
  
  snprintf(input_data->sorting_str, STR_MEDIUM, "B_%04d%s", block_num, input_data->str_direction);
  //strcpy(input_data->sorting_str, input_data->str_block);
  //strcat(input_data->sorting_str, input_data->str_direction);

  /* line parsed ok, no error found */
  return 0; 
}

/** @brief  calculate run profile file name from the output data 
 *  @param  filename  buffer to store resulted filename
 *  @param  data      pointer to output data
 *  @return err_insufficient_buffer_size 
 *  
 */
int
get_output_file(char *filename, outputData *data)
{
  char str_temp[STR_MAX] = "";
  char str_station_code[STR_MIN] = "";
  char str_platform[STR_MIN] = "";

  int i =0;
  bool b_available = false;
  FILE *fp = NULL;

  if ((strlen(RUN_PROFILE_PATH) +
       strlen(RUN_PROFILE_PREFIX) + 
       strlen(data->str_station_code) + 
       strlen(data->str_platform) + 
       2 + /* file_run_cnt length */
       3 + /* cc_id length */
       strlen(data->input_data_file)) >= (STR_MAX))
  {
    return throw_err(err_insufficient_buffer_size);
  }

  if (NULL != data->str_platform && strcmp(data->str_platform, "") > 0)
  {
    strcpy(str_platform, data->str_platform);
  }
  else 
  {
    strcpy(str_platform, "_");
  }
  if (NULL != data->str_station_code && strcmp(data->str_station_code, "") > 0)
  {
    strcpy(str_station_code, data->str_station_code);
  }
  else 
  {
    strcpy(str_station_code, "___");
  }

  while (!b_available && i <= MAX_FILE_CNT)
  {
    snprintf
    (
      str_temp, 
      STR_MAX, 
      "%s%s%s%s_%02d_CC%03d_%s", 
      RUN_PROFILE_PATH,
      RUN_PROFILE_PREFIX,
      str_station_code, 
      str_platform, 
      i, 
      data->cc_id,
      data->input_data_file
    );
    fp = fopen(str_temp,"r");
    if (NULL == fp)
    {
      b_available = true;
    }
    else
    {
      fclose(fp); 
    }
    i++;
  }
    
  strcpy(filename, str_temp);  
  return 0;
}

/** @brief  match input_data str_block(segment_id) and str_direction           
 *          to lookup table, retrieve direction, str_station_code, 
 *          str_platform, is_platform, and store in input_data 
 *  @param  *input_data  input data 
 *  @return 0
 *          err_lut_is_empty 
 *          err_lut_match_not_found
 *          err_file_format_not_valid
 *          err_list_iteration_failed 
 * 
 */
int
expand_data_use_lut(inputData *input_data)
{
  lutData *p_lut_data = NULL;  
  bool is_matched = false;
  double speed_km_h = 0;
  int err = 0;

  bool b_enabled = true;

  if (  (NULL == input_data->str_block) ||
        (strlen(input_data->str_block) < 2) ||
        (NULL == input_data->str_direction) ||
        (strlen(input_data->str_direction) < 1) )
  { 
    return throw_err(err_file_format_not_valid);
  }
  if ( list_empty(&lut_data_list) )
  {
    return throw_err(err_lut_is_empty);
  }
  /* iterate through the lut list until find match */
  /* iteration start */
  /* check err, 0 - not able to start iteration; 1 - ok to iterate */
  if (1 != list_iterator_start(&lut_data_list))
  {
    /* iteration not able to start */
    return throw_err(err_list_iteration_failed);
  }

  while ((!is_matched) && (list_iterator_hasnext(&lut_data_list)) && (b_enabled)) 
  { 
    /* check for next element */
    p_lut_data = (lutData *)list_iterator_next(&lut_data_list);
    if (NULL == p_lut_data)
    {
      /* error on retrieving element from list */
      err = throw_err(err_list_retrieval_failed);
      b_enabled = false;
    }
    else
    {
      /* process lut data constructs output data structure */
      /* generate output list id */
      if (0 == strcmp(p_lut_data->str_block, input_data->str_block))
      {
        strcpy(input_data->str_station_code, p_lut_data->str_station_code);
        strcpy(input_data->str_platform, p_lut_data->str_platform);
        strcpy(input_data->str_from_station, p_lut_data->str_from_station);
        strcpy(input_data->str_to_station, p_lut_data->str_to_station);
        strcpy(input_data->str_direction_code, p_lut_data->str_direction_code);

        input_data->is_platform = p_lut_data->is_platform;
        input_data->direction = p_lut_data->direction;

        /* calculate signed_measured_speed_km_h */
        if ((input_data->str_speed[0] < '0') || (input_data->str_speed[0] > '9'))
        {
          err = throw_err(err_speed_not_valid);
        }
        else
        {
          sscanf(input_data->str_speed, "%lf", &speed_km_h);
          
          if (1 == input_data->direction)
          {
            input_data->signed_measured_speed_km_h = speed_km_h;
          }
          else if (2 == input_data->direction)
          {
            input_data->signed_measured_speed_km_h = 0 - speed_km_h;
          }
          else
          {
            err = throw_err(err_file_format_not_valid);
          }
        } 

        is_matched = true;
      }
    }
  }

  if (1 != list_iterator_stop(&lut_data_list))
  {
    return throw_err(err_list_stop_failed);
  }
  if (0 != err)
  {
    return err;
  }
  if (!is_matched)
  {
    return throw_err(err_lut_match_not_found);
  }
  return 0;
}

/** @brief  read lookup table file and add to lookup table data list
 *  @param  *str_lut_file  lookup table file path 
 *  @return err_list_append_failed
 *          err_file_not_accessible      
 */
int
read_lut_file(char *str_lut_file)
{

  int err = 0;

  char str_data_line[STR_MAX] = "";
  lutData lut_data = {0};
  FILE *p_lut_file = NULL;

  bool b_enabled = true;

  if (NULL == str_lut_file)
  {
    fprintf
    (
      stdout, 
      "[%6s][%s][%s]\n", 
      "ERROR", 
      "Configuration Files Not Defined!",
      str_lut_file
    );
    err = throw_err(err_file_not_accessible); 
    return err;
  }
  
  if (NULL != (p_lut_file = fopen(str_lut_file, "r")))
  {
    while ((NULL != fgets(str_data_line, STR_MAX, p_lut_file)) && (b_enabled))
    {
      /* check if line is csv */
      if (!is_csv_line(str_data_line, k_lut_header_cnt))
      {
        fprintf
        (
          stdout, 
          "[%6s][%s][%s][%s]\n", 
          "ERROR", 
          "Not a valid csv line",
          str_lut_file, 
          str_data_line
        );
        err = throw_err(err_file_format_not_valid);
      }
      else
      {
        /* check if line is header */
        if (is_header(str_data_line, k_lut_header_num_digit))
        {
          fprintf
          (
            stdout, 
            "[%6s][%s][%s][%s]\n", 
            "DEBUG", 
            "line is header", 
            str_lut_file, 
            str_data_line
          );
          
        }
        else
        {
          err = parse_lut_data(&lut_data, str_data_line);
          /* check err */
          if (err < 0)
          {
            fprintf
            (
              stdout, 
              "[%6s][%s][%s][%s][%s]\n", 
              "ERROR", 
              "line is not parsed correctly",
              get_err_description(err),
              str_lut_file, 
              str_data_line
            );
          }
          else
          {
            /* add lut data to lut data list */
            err = add_to_lut_data_list(&lut_data);
            /* check err */
            if (err < 0)
            {
              fprintf
              (
                stdout, 
                "[%6s][%s][%s][%s][%s]\n", 
                "ERROR", 
                "line is not appended correctly",
                get_err_description(err),
                str_lut_file, 
                str_data_line
              );
              b_enabled = false;
            }
          }
        }
      }
    }
    fclose(p_lut_file);
    if (!b_enabled)
    {
      fprintf
      (
        stdout, 
        "[%6s][%s][%s]\n", 
        "ERROR", 
        "Configuration Files Importing Process Interrupted!", 
        str_lut_file
      );
    }
  }
  else
  {
    fprintf(stdout, "[%6s][%s][%s]\n", "ERROR", "Configuration Files Not Found!", str_lut_file);
    err = throw_err(err_file_not_accessible);
  }

  return err;
}

/** @brief  read input data file and add to input data list 
 *  @param  *str_data_file  input data file path 
 *  @return err_list_append_failed 
 *          err_file_not_accessible 
 */
int 
read_input_file(char *str_data_file)
{
  
  int err = 0; 
  
  char str_data_line[STR_MAX] = "";
  inputData input_data = {0};
  FILE * p_data_file = NULL; 

  bool b_enabled = true;

  if (NULL == str_data_file)
  {
    fprintf
    (
      stdout, 
      "[%6s][%s][%s]\n", 
      "ERROR", 
      "Data Files Not Defined!", 
      str_data_file
    );
    err = throw_err(err_file_not_accessible);
    return err; 
  }

  if (NULL != (p_data_file = fopen(str_data_file, "r")))
  {
    while (NULL != (fgets(str_data_line, STR_MAX, p_data_file)) && (0 == err))
    {
      /* check if line is csv */
      if (!is_csv_line(str_data_line, k_input_csv_num_col))
      {
        fprintf
        (
          stdout, 
          "[%6s][%s][%s][%s]\n", 
          "ERROR", 
          "Not a valid csv line",
          str_data_file, 
          str_data_line
        );
        err = throw_err(err_file_format_not_valid);
      }
      else
      {
        /* check if line is header */
        if (is_header(str_data_line, k_header_num_digit))
        {
          fprintf
          (
            stdout, 
            "[%6s][%s][%s][%s]\n", 
            "DEBUG", 
            "line is header", 
            str_data_file, 
            str_data_line
          );
          err = parse_input_header(str_data_line);
          if (err < 0)
          {
            fprintf
            (
              stdout, 
              "[%6s][%s][%s][%s]\n", 
              "ERROR", 
              "header is not parsed correctly", 
              str_data_file, 
              str_data_line
            );
            
          }
        }
        else
        {
          err = parse_input_data(&input_data, str_data_line, str_data_file);
          /* check err */
          if (err < 0)
          {
            fprintf
            (
              stdout, 
              "[%6s][%s][%s][%s][%s]\n", 
              "ERROR", 
              "line is not parsed correctly",
              get_err_description(err),
              str_data_file, 
              str_data_line
            );
          }
          else
          {
            /* add input data to input data list */
            err = add_to_input_data_list(&input_data);
            /* check err */
            if (err < 0)
            {
              fprintf
              (
                stdout, 
                "[%6s][%s][%s][%s][%s]\n", 
                "ERROR", 
                "line is not appended correctly",
                get_err_description(err),
                str_data_file, 
                str_data_line
              );
              b_enabled = false;
            }
          }
        }
      }
    }
    fclose(p_data_file);
    if (!b_enabled)
    {
      fprintf
      (
        stdout, 
        "[%6s][%s][%s]\n", 
        "ERROR", 
        "Data File Importing Process Interrupted!", 
        str_data_file
      );
    }
  }
  else
  {
    fprintf(stdout, "[%6s][%s][%s]\n", "ERROR", "Data Files Not Found!", str_data_file);
    err = throw_err(err_file_not_accessible);
  }

  return err;
}

/** @brief  display program title, version, usage information
 *  @return none
 *      
 */
void 
display_usage(char **argv)
{
  printf("\n");
  printf("************************************************************\n");
  printf("\n");  
  printf("    %s    Version: %s\n", SOFTWARE_TITLE, SOFTWARE_VER);
  printf("\n");
  printf("************************************************************\n");
  printf("\n");
  printf("USAGE: %s ", strip_path(argv[0]));
  printf("[FILE]...\n");
  printf("function description\n");
  printf("\n");
}

/** @brief  expand input data list with lookup table 
 *  @return err_list_iteration_failed 
 *          err_list_retrieval_failed 
 */
int
expand_data_list_use_lut()
{
  inputData *p_input_data = NULL;
  int err = 0;

  bool b_enabled = true;

  /* check err, 0 - not able to start iteration; 1 - ok to iterate */
  if (1 != list_iterator_start(&input_data_list))
  {
    /* iteration not able to start */
    err = throw_err(err_list_iteration_failed);
    fprintf
    (
      stdout, 
      "[%6s][%s][%s]\n", 
      "ERROR", 
      "Input Data Not Available!",
      get_err_description(err)
    );
    return err;
  }
  while (list_iterator_hasnext(&input_data_list) && (b_enabled)) 
  {
    /* check for next element */
    p_input_data = (inputData *)list_iterator_next(&input_data_list);
    if (NULL == p_input_data)
    {
      /* error on retrieving element from list */
      err = throw_err(err_list_retrieval_failed);
      fprintf
      (
        stdout, 
        "[%6s][%s][%s]\n", 
        "ERROR", 
        "Input Data Not Available",
        get_err_description(err)
      );
      b_enabled = false;
    }
    else
    {
      err = expand_data_use_lut(p_input_data);
      if (err < 0)
      {
        fprintf
        (
          stdout, 
          "[%6s][%s][%s][%s]\n", 
          "ERROR", 
          "Input Data Not Valid",
          get_err_description(err),
          input_data_to_string(p_input_data)
        );
      }
      if (err == throw_err(err_lut_match_not_found))
      {
        /* ignore if err is lut match not found */
        err = 0; 
      }
    }
  }

  if (!b_enabled)
  {
    fprintf
    (
      stdout, 
      "[%6s][%s]\n", 
      "ERROR", 
      "Input Data Preprocessing Process Interrupted!"
    );
  }

  if (1 != list_iterator_stop(&input_data_list))
  {
    return throw_err(err_list_stop_failed);
  }
  else
  {
    return err;
  }
}

/** @brief  process input data list, generate output data, and 
 *          add to output data list 
 *  @return err_list_iteration_failed 
 *          err_list_retrieval_failed
 *          err_maximum_number_exceeded
 *  
 */
int
calculate_output_data_list()
{  
  inputData *p_input_data = NULL;
  inputData *p_input_data_prev = NULL;
  outputData output_data = {0};  
  
  time_t time_origin = 0;
  time_t travel_time_origin = 0;
  uint8_t cur_run_cnt = 0;
  
  double displacement_m = 0;
  double accum_displacement_m = 0;
  double accum_travelled_m = 0;

  bool b_departure_complete = false;
  bool b_arrival_complete = false;
  bool b_run_complete = false;
  bool b_run_interrupted = false;
  bool b_stopped = false;
  bool b_departed = false;

  char str_departure_block[STR_SHORT] = "";
  char str_arrival_block[STR_SHORT] = "";

  char *p_temp = NULL;
  int err = 0;
  bool b_enabled = true;

  /* iteration start on input list */
  /* check err, 0 - not able to start iteration; 1 - ok to iterate */
  if (1 != list_iterator_start(&input_data_list))
  {
    /* iteration not able to start */
    fprintf
    (
      stdout, 
      "[%6s][%s][%s]\n", 
      "ERROR", 
      "Input Data Not Available!",
      get_err_description(err_list_iteration_failed)
    );
    return throw_err(err_list_iteration_failed);
  }
  while (list_iterator_hasnext(&input_data_list) && (b_enabled)) 
  { 
    /* check for next element */
    p_input_data = (inputData *)list_iterator_next(&input_data_list);
    if (NULL == p_input_data)
    {
      /* error on retrieving element from list */
      b_enabled = false;
      err = throw_err(err_list_retrieval_failed);
      fprintf
      (
        stdout, 
        "[%6s][%s][%s]\n", 
        "ERROR", 
        "Input Data Not Available",
        get_err_description(err)
      );
    }
    else
    {
      /* process input data constructs output data structure */
      /* generate output list id */
      if (LIST_MAX_SIZE == output_list_id)
      {
        b_enabled = false;
        err = throw_err(err_maximum_number_exceeded);
        fprintf
        (
          stdout, 
          "[%6s][%s][%s][%s]\n", 
          "ERROR", 
          "Input Data Numbers Exceeded Limitation",
          get_err_description(err),
          input_data_to_string(p_input_data)
        );
      }
      else
      {      
        output_data.id = output_list_id++;
        /* copy known data from input data to output data */
        strcpy(output_data.sorting_str, p_input_data->sorting_str);
        strcpy(output_data.input_data_file, p_input_data->input_data_file);
        output_data.input_data_id = p_input_data->id;
        strcpy(output_data.str_timestamp, p_input_data->str_timestamp);
        strcpy(output_data.str_cc_id, p_input_data->str_cc_id);
        strcpy(output_data.segment_id, p_input_data->str_block);
        strcpy(output_data.signal_name_graphing, p_input_data->str_current_driving_mode);
        output_data.measured_speed_km_h = abs(p_input_data->signed_measured_speed_km_h);
        
        output_data.timestamp = p_input_data->timestamp;
        output_data.cc_id = p_input_data->cc_id; 
        strcpy(output_data.str_station_code, p_input_data->str_station_code);
        strcpy(output_data.str_platform, p_input_data->str_platform);
        strcpy(output_data.str_from_station, p_input_data->str_from_station);
        strcpy(output_data.str_to_station, p_input_data->str_to_station);
        strcpy(output_data.str_direction_code, p_input_data->str_direction_code);

        /* populate unneccessary and unknown info with -1 or "NA" */
        output_data.permitted_speed_km_h = -1.0;
        output_data.current_tag_id = -1;
        output_data.ti_tag = -1;
        strcpy(output_data.signal_name, "NA");
        output_data.civil_speed_km_h = -1.0;

        /* check if new run profile 

        * if new file, determine if the run profile is complete
        *   skip station
        *   begin or end is not platform
        first data point */
        /* if new */

        if 
        (
          /* first item */
          (NULL == p_input_data_prev) ||
          /* different cc number */
          (strcmp(p_input_data_prev->str_cc_id, p_input_data->str_cc_id)) ||
          /* delta_t > 3 sec and different track*/
          ( ((p_input_data->timestamp - p_input_data_prev->timestamp) > 3) && 
            !(p_input_data_prev->is_motion) && 
            (strcmp(p_input_data_prev->str_block, p_input_data->str_block))) ||
          /* normal service - first door open event */
          ((p_input_data->is_platform) && 
          ('0' == p_input_data_prev->str_doors_open[0]) &&
          ('1' == p_input_data->str_doors_open[0]) &&
          (strcmp(str_departure_block, p_input_data->str_block))) ||
          /* skip station */
          (('1' == p_input_data_prev->str_skip_stop[0]) && 
          (p_input_data_prev->is_platform) &&
          !(p_input_data->is_platform)) 
        )
        /* new run profile */
        {
          time_origin = p_input_data->timestamp;
          travel_time_origin = p_input_data->timestamp;
          if (MAX_RUN_CNT == run_cnt)
          {
            b_enabled = false;
            err = throw_err(err_maximum_run_number_exceeded);
            fprintf
            (
              stdout, 
              "[%6s][%s][%s][%s]\n", 
              "ERROR", 
              "Speed Profiles Numbers Exceeded Limitation",
              get_err_description(err),
              input_data_to_string(p_input_data)
            );
          }
          else
          {
            cur_run_cnt = run_cnt++;
            output_data.run_cnt = cur_run_cnt;
          }
          accum_displacement_m = 0;
          accum_travelled_m = 0;

          output_data.log_time_s = 0;
          output_data.travel_time_s = 0;
          output_data.distance_travelled_0_m = 0;
          /* reverse distance - need to work on this later on */
          output_data.distance_travelled_1_m = 0;
          output_data.accum_distance_travelled_ft = 0;

          strcpy(str_departure_block, p_input_data->str_block);

          b_departure_complete = false;
          b_arrival_complete = false;
          b_run_complete = false;
          b_run_interrupted = false;
          b_stopped = false;
          b_departed = false;
          strcpy(str_arrival_block, "");
        }
        else
        /* continue the current run profile */
        {
          output_data.run_cnt = cur_run_cnt;
          output_data.log_time_s = 
            (double) 
            (
              strtol( p_input_data->str_timestamp, 
                      &p_temp, 
                      10
                    ) 
              - time_origin
            );

          if (!b_departed)
          {
            if (p_input_data->is_motion)
            {
              b_departed = true;
            }
            else
            {
              travel_time_origin = p_input_data->timestamp;
            }
          }
          output_data.travel_time_s = 
            (double) 
            (
              strtol( p_input_data->str_timestamp, 
                      &p_temp, 
                      10
                    ) 
              - travel_time_origin
            );

          displacement_m = integrate_trapezoidal(
            p_input_data_prev->timestamp,
            1/3.6*p_input_data_prev->signed_measured_speed_km_h,
            p_input_data->timestamp,
            1/3.6*p_input_data->signed_measured_speed_km_h
          );
          output_data.distance_travelled_0_m = displacement_m;
          output_data.distance_travelled_1_m = displacement_m;
          accum_displacement_m += displacement_m;
          if (displacement_m < 0)
          {        
            accum_travelled_m -= displacement_m;
          }
          else
          {
            accum_travelled_m += displacement_m;
          }
          output_data.accum_distance_travelled_ft = 3.28084 * accum_travelled_m;

          if 
          (
            (p_input_data_prev->is_motion) && 
            !(p_input_data->is_motion) &&
            (p_input_data->is_platform) &&
            (strcmp(p_input_data->str_block, str_departure_block)) &&
            (NULL == str_arrival_block)
          )
          {
            b_arrival_complete = true;
            strcpy(str_arrival_block, p_input_data->str_block);
          }
          if (!b_departure_complete)
          {
            if ((p_input_data->is_motion) && (p_input_data->is_platform))
            {
              b_departure_complete = true;
            }            
          }
          if
          (
            (p_input_data_prev->is_motion) && 
            !(p_input_data->is_motion)
          )
          {
            b_stopped = true;
          }
          if
          (
            !(p_input_data_prev->is_motion) && 
            (p_input_data->is_motion) &&
            (b_stopped) 
          )
          {
            b_run_interrupted = true;
          }
          if
          (
            (b_departure_complete) &&
            (b_arrival_complete)
          )
          {
            b_run_complete = true;
          }
        }

        p_input_data_prev = p_input_data;

        /* add output data structure to output list */
        err = add_to_output_data_list(&output_data);
        if (err < 0)
        {
          fprintf
          (
            stdout, 
            "[%6s][%s][%s][%s]\n", 
            "ERROR", 
            "Speed Profile Append Failure!",
            get_err_description(err),
            output_data_to_string(&output_data)
          );
          b_enabled = false;
        }
      }
    }
  }

  if (!b_enabled)
  {
    fprintf
    (
      stdout, 
      "[%6s][%s]\n", 
      "ERROR", 
      "Speed Profile Calculation Process Interrupted!"
    );
  }

  if (1 != list_iterator_stop(&input_data_list))
  {
    return throw_err(err_list_stop_failed);
  }
  else
  {
    return err;  
  }
} 

/** @brief  generate run profile output csv files, 
 *          one file per start-stop per train
 *  @return err_list_iteration_failed 
 *          err_list_retrieval_failed
 *          err_maximum_number_exceeded
 *  
 */
int
export_run_profile_file()
{
  FILE *fp_out = NULL;

  int run_cnt_prev = -1;
  char output_file_full_path[STR_MAX] = "";
  char output_filename[STR_MAX] = "";
  char prev_signal_name_graphing[STR_SHORT] = "";
  char graphing_temp[STR_SHORT] = "";
  char run_profile_description[STR_MAX] = "";
  
  int err = 0;
  outputData *p_data = NULL;

  bool b_enabled = true;

  /* start iteration */
  /* iteration start on output list */
  /* check err, 0 - not able to start iteration; 1 - ok to iterate */
  if (1 != list_iterator_start(&output_data_list))
  {
    /* iteration not able to start */
    fprintf
    (
      stdout, 
      "[%6s][%s][%s]\n", 
      "ERROR", 
      "Speed Profiles Not Available!",
      get_err_description(err_list_iteration_failed)
    );
    return throw_err(err_list_iteration_failed);
  }
  while (list_iterator_hasnext(&output_data_list) && (b_enabled)) 
  { 
    /* check for next element */
    p_data = (outputData *)list_iterator_next(&output_data_list);
    if (NULL == p_data)
    {
      /* error on retrieving element from list */
      b_enabled = false;      
      err = throw_err(err_list_retrieval_failed);
      fprintf
      (
        stdout, 
        "[%6s][%s][%s]\n", 
        "ERROR", 
        "Speed Profiles Not Available",
        get_err_description(err)
      );
    }
    else
    {
      if (run_cnt_prev != p_data->run_cnt)
      {
        /* new file */
        /* close old file if open */
        if (NULL != fp_out)
        {
          fclose(fp_out);
        }

        run_cnt_prev = p_data->run_cnt;
        strcpy(prev_signal_name_graphing, "");
        
        err = get_output_file(output_filename, p_data);
        if (err < 0)
        {
          b_enabled = false;
          fprintf
          (
            stdout, 
            "[%6s][%s][%s]\n", 
            "ERROR", 
            "Output File Not Available",
            get_err_description(err)
          );
        }
        else
        {
          strcpy(output_file_full_path, output_filename);

          /* check if file already exist */
          fp_out = fopen(output_file_full_path, "r");
          if (NULL != fp_out)
          {
            fclose(fp_out);
            b_enabled = false;
            err = throw_err(err_file_already_exist);
            fprintf
            (
              stdout, 
              "[%6s][%s][%s][%s]\n", 
              "ERROR", 
              "Output File Already Exists!",
              get_err_description(err),
              output_file_full_path
            );
          }
          else
          { 
            /* create new file */
            fp_out = fopen(output_file_full_path, "w");
            /* file cannot be created */
            if (NULL == fp_out)
            {
              b_enabled = false;
              err = throw_err(err_file_not_accessible);
              fprintf
              (
                stdout, 
                "[%6s][%d][%s][%s][%s]\n", 
                "ERROR", 
                errno,
                "Output File Cannot Be Created!",
                get_err_description(err),
                output_file_full_path
              );
            }
            else
            {
              /* file created successfully */
              strcpy(p_data->output_data_file, output_filename);
              /* write header to new file */
              
              if (NULL != p_data->str_platform && strcmp(p_data->str_platform, "") > 0)
              {
                snprintf
                (
                  run_profile_description, 
                  STR_MAX, 
                  "%s%s%s%s%s%s%s%s%s",
                  "ATC Speed Profile Departing From ", 
                  p_data->str_from_station, 
                  " Platform ", 
                  p_data->str_platform, 
                  " to ", 
                  p_data->str_to_station,
                  " (", 
                  p_data->str_direction_code, 
                  ")"
                );
              }
              else
              {
                snprintf
                (
                  run_profile_description, 
                  STR_MAX, 
                  "%s%s%s%s%s%s%s",
                  "ATC Speed Profile Departing From ", 
                  p_data->str_from_station,  
                  " to ", 
                  p_data->str_to_station,
                  " (", 
                  p_data->str_direction_code, 
                  ")"
                );
              }
              fprintf
              (
                fp_out, 
                "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", 
                "Log_Time_[s]",
                "Segment_ID",
                "Distance_Travelled_0_[m]",
                "Distance_Travelled_1_[m]",
                "Accum_Distance_Travelled_[ft]",
                "Permitted_Speed_[km/h]",
                "Measured_Speed_[km/h]",
                "Current_Tag_ID",
                "TI_Tag",
                "Signal_Name",
                "Signal_Name_Graphing",
                "Civil_Speed_[km/h]",
                "Travel_Time_[s]",
                run_profile_description, 
                "Distance Travelled from Starting Point [ft]"
              );
            }
          }
        }
      }

      /* if current_driving_mode changes, = new, otherwise = blank */
      if (strcmp(prev_signal_name_graphing, p_data->signal_name_graphing))
      /* not equal to last one, print current signal_name_graphing */
      {
        strcpy(graphing_temp, p_data->signal_name_graphing);
      }
      else 
      /* equal to last one, print "" */
      {
        strcpy(graphing_temp, "");
      }
      /* write the current output data to the output file */

      fprintf
      (
        fp_out, 
        "%f, %s, %f, %f, %f, %f, %f, %d, %d, %s, %s, %f, %f, %s\n", 
        p_data->log_time_s,
        p_data->segment_id,
        p_data->distance_travelled_0_m,
        p_data->distance_travelled_1_m,
        p_data->accum_distance_travelled_ft,
        p_data->permitted_speed_km_h,
        p_data->measured_speed_km_h,
        p_data->current_tag_id,
        p_data->ti_tag,
        p_data->signal_name,
        graphing_temp,
        p_data->civil_speed_km_h,
        p_data->travel_time_s, 
        p_data->str_timestamp
      );
    
      /* copy current to prev */
      strcpy(prev_signal_name_graphing, p_data->signal_name_graphing);
    }
    /* end of current iteration */
  }
  
  if (NULL != fp_out)
  {
    fclose(fp_out);
  }

  if (!b_enabled)
  {
    fprintf
    (
      stdout, 
      "[%6s][%s]\n", 
      "ERROR", 
      "Speed Profiles Export Process Interrupted!"
    );
  }

  if (1 != list_iterator_stop(&output_data_list))
  {
    return throw_err(err_list_stop_failed);
  }
  else
  {
    return err;
  }
}
