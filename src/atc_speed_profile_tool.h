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

#ifndef ATC_SPEED_PROFILE_H
#define ATC_SPEED_PROFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <inttypes.h>
#include "common_util.h"

/*
** Type Definitions
** -----------------------------------------------------
*/


/*
** Constants
** -----------------------------------------------------
*/

#define LIST_MAX_SIZE UINT_MAX

/* software title */
#define SOFTWARE_TITLE            "ATC Speed Profile Tool"
/* software version */
#define SOFTWARE_VER              "1.0"

/* lookup table file */
#define BLOCK_LUT_FILE            "config\\atc_block_lut.csv"
/* maximum run profile count */
#define MAX_RUN_CNT               65535
/* maximum file count */
#define MAX_FILE_CNT              99
/* folder store output ECD format logs */                  
#define RUN_PROFILE_PATH          "run_profiles\\"
/* folder store output ECD format logs */ 
#define RUN_PROFILE_PREFIX        "DEPARTING_FROM_PLATFORM_"
/* Look up table header (data column) count */
#define LUT_HEADER_CNT            11
/* Input file list maximum length */
#define FILE_LIST_MAX_LENGTH      99


/*
** Structures
** -----------------------------------------------------
*/

/* input data structure */
typedef struct input_data_t 
{
  /* unique id, range [0, 4,294,967,295] */
  uint64_t id;
  /* sorting value "nnnYYYYMMDDHHMMSS" */
  /* combination of str_cc_id, str_timestamp */
  char sorting_str[STR_MEDIUM];
  char input_data_file[STR_MAX];

  /* YYYYMMDDHHMMSS converted from str_time */
  char str_timestamp[STR_MEDIUM]; 

  char str_time[STR_MEDIUM];
  char str_location[STR_MEDIUM];
  char str_block[STR_SHORT];
  char str_run_number[STR_MIN];
  char str_direction[STR_MIN];
  char str_destination_code[STR_MIN];
  char str_origination_code[STR_MIN];
  char str_schedule_class[STR_MIN];
  char str_cc_id[STR_MIN];
  char str_current_driving_mode[STR_MIN];
  char str_selected_driving_mode[STR_MIN];
  char str_talkative[STR_MIN];
  char str_motion[STR_MIN];
  char str_skip_stop[STR_MIN];
  char str_doors_open[STR_MIN];
  char str_door_fault[STR_MIN];
  char str_alarm[STR_MIN];
  char str_emergency_brake[STR_MIN];
  char str_speed[STR_MIN];

  char str_from_station[STR_MEDIUM];
  char str_to_station[STR_MEDIUM];
  char str_direction_code[STR_MIN];

  time_t timestamp;
  int cc_id;

  int direction;
  double signed_measured_speed_km_h;
  char str_station_code[STR_MIN];
  char str_platform[STR_MIN];
  bool is_platform;
  bool is_motion;

} inputData;

/* output data structure */
typedef struct output_data_t 
{
  /* unique id, range [0, 4,294,967,295] */
  uint64_t id;
  /* sorting value "nnnYYYYMMDDHHMMSS" */
  /* combination of str_cc_id, str_timestamp */
  char sorting_str[STR_MEDIUM];
  char input_data_file[STR_MAX];
  char output_data_file[STR_MAX];

  uint64_t input_data_id;

  /* YYYYMMDDHHMMSS converted from str_time */
  char str_timestamp[STR_MEDIUM];
  char str_cc_id[STR_MIN];

  double log_time_s;
  char segment_id[STR_SHORT];
  double distance_travelled_0_m;
  double distance_travelled_1_m;
  double accum_distance_travelled_ft;
  double permitted_speed_km_h;
  double measured_speed_km_h;
  int current_tag_id;
  int ti_tag;
  char signal_name[STR_SHORT];
  char signal_name_graphing[STR_SHORT];
  double civil_speed_km_h;
  double travel_time_s;

  char str_from_station[STR_MEDIUM];
  char str_to_station[STR_MEDIUM];
  char str_direction_code[STR_MIN];

  time_t timestamp;
  int cc_id;
  char str_station_code[STR_MIN];
  char str_platform[STR_MIN];
  char str_direction_num[STR_MIN];
  uint8_t run_cnt;

} outputData;

/* lookup table data structure */
typedef struct lut_data_t 
{
  /* unique id, range [0, 4,294,967,295] */
  uint64_t id;
  /* sorting value */
  /* combination of str_block, str_direction */
  char sorting_str[STR_MEDIUM];

  char str_id[STR_MIN];
  char str_location[STR_MEDIUM];
  char str_block[STR_SHORT];
  char str_direction[STR_MIN];
  char str_direction_code[STR_MIN];
  char str_direction_num[STR_MIN];  
  char str_platform[STR_MIN];
  char str_station_code[STR_MIN];
  char str_block_length[STR_SHORT];

  char str_from_station[STR_MEDIUM];
  char str_to_station[STR_MEDIUM];

  int direction;
  double block_length;
  bool is_platform;

} lutData;

/* input data header structure */
typedef struct input_header_t 
{
  int col_time;
  int col_location;
  int col_block;
  int col_run_number;
  int col_direction;
  int col_destination_code;
  int col_origination_code;
  int col_schedule_class;
  int col_cc_id;
  int col_current_driving_mode;
  int col_selected_driving_mode;
  int col_talkative;
  int col_motion;
  int col_skip_stop;
  int col_doors_open;
  int col_door_fault;
  int col_alarm;
  int col_emergency_brake;
  int col_speed;
} inputHeader;

/*
** Variables
** -----------------------------------------------------
*/

/* number of columns in input csv low limit */
static const int k_input_csv_num_col = 19;
/* number of digits in header high limit */
static const int k_header_num_digit = 1;
/* number of digits in lut header high limit */
static const int k_lut_header_num_digit = 4;
/* number of columns in lookup table csv */
static const int k_lut_header_cnt = LUT_HEADER_CNT;
/* static string to support _to_string function */
static char str_static_data[STR_EXTRA];

/* input data header index */
enum header_index
{
  k_header_id_time = 0,
  k_header_id_location,
  k_header_id_block,
  k_header_id_run_number,
  k_header_id_direction,
  k_header_id_destination_code,
  k_header_id_origination_code,
  k_header_id_schedule_class,
  k_header_id_cc_id,
  k_header_id_current_driving_mode,
  k_header_id_selected_driving_mode,
  k_header_id_talkative,
  k_header_id_motion,
  k_header_id_skip_stop,
  k_header_id_doors_open,
  k_header_id_door_fault,
  k_header_id_alarm,
  k_header_id_emergency_brake,
  k_header_id_speed,

  /* input data header columns count */
  k_header_cnt
};

/* input data header column names */
static const char *k_header_names[k_header_cnt] = 
{
  "TIME",
  "LOCATION",
  "BLOCK",
  "RUN NUMBER",
  "DIRECTION",
  "DESTINATION CODE",
  "ORIGINATION CODE", 
  "SCHEDULE CLASS",
  "CC ID", 
  "CURRENT DRIVING MODE", 
  "SELECTED DRIVING MODE",
  "TALKATIVE", 
  "MOTION", 
  "SKIP STOP", 
  "DOORS OPEN",
  "DOOR FAULT", 
  "ALARM", 
  "EMERGENCY BRAKE", 
  "SPEED"
};

/* input data header column index */
static inputHeader input_header = 
{
  .col_time = 0,
  .col_location = 1,
  .col_block = 2,
  .col_run_number = 3,
  .col_direction = 4,
  .col_destination_code = 5,
  .col_origination_code = 6,
  .col_schedule_class = 7,
  .col_cc_id = 8,
  .col_current_driving_mode = 9,
  .col_selected_driving_mode = 10,
  .col_talkative = 11,
  .col_motion = 12,
  .col_skip_stop = 13,
  .col_doors_open = 14,
  .col_door_fault = 15,
  .col_alarm = 16,
  .col_emergency_brake = 17,
  .col_speed = 18
};

/*
** Function Prototypes
** -----------------------------------------------------
*/

/** @brief  convert inputHeader and format into string 
 *  @param  *data  inputData to be converted 
 *  @return NULL      input is not valid
 *          converted static string             
 */
char *
input_header_to_string();

/** @brief  convert inputData and format into string 
 *  @param  *data  inputData to be converted 
 *  @return NULL      input is not valid
 *          converted static string  
 *             
 */
char *
input_data_to_string(inputData *data);

/** @brief  convert lutData and format into string 
 *  @param  *data  lutData to be converted 
 *  @return NULL      input is not valid
 *          converted static string  
 *             
 */
char *
lut_data_to_string(lutData *data);

/** @brief  convert outputData and format into string 
 *  @param  *data  outputData to be converted 
 *  @return NULL      input is not valid
 *          converted static string  
 *             
 */
char *
output_data_to_string(outputData *data);

/** @brief  print block look up table on the screen
 *  @param  none
 *  @return errorCode
 *             
 */
int 
display_lut_data_list();

/** @brief  print input data list on the screen
 *  @param  none
 *  @return none
 *             
 */
int 
display_input_data_list();

/** @brief  print output data list on the screen
 *  @param  none
 *  @return none
 *             
 */
int 
display_output_data_list();

/** @brief  initialize lookup list
 *  @param  none
 *  @return none
 *             
 */
void 
init_lut_data_list();

/** @brief  initialize input data list
 *  @param  none
 *  @return none
 *             
 */
void 
init_input_data_list();

/** @brief  initialize output data list
 *  @param  none
 *  @return none
 *             
 */
void 
init_output_data_list();

/** @brief  sort input data list
 *  @param  none
 *  @return none
 *             
 */
int 
sort_input_data_list();

/** @brief  clear and free lookup table list
 *  @param  none
 *  @return none
 *             
 */
void 
free_lut_data_list();

/** @brief  clear and free input data list
 *  @param  none
 *  @return none
 *             
 */
void 
free_input_data_list();

/** @brief  clear and free output data list
 *  @param  none
 *  @return none
 *             
 */
void 
free_output_data_list();

/** @brief  read lookup table file and add to lookup table data list
 *  @param  *str_lut_file  lookup table file path 
 *  @return err_list_append_failed
 *          err_file_not_accessible      
 */
int
read_lut_file(char *str_lut_file);

/** @brief  read input data file and add to input data list 
 *  @param  *str_lut_file  input data file path 
 *  @return err_list_append_failed 
 *          err_file_not_accessible 
 */
int 
read_input_file(char *str_data_file);

/** @brief  display program title, version, usage information
 *  @return none
 *      
 */
void 
display_usage(char **argv);

/** @brief  expand input data list with lookup table 
 *  @return err_list_iteration_failed 
 *          err_list_retrieval_failed 
 */
int
expand_data_list_use_lut();

/** @brief  process input data list, generate output data, and 
 *          add to output data list 
 *  @return err_list_iteration_failed 
 *          err_list_retrieval_failed
 *          err_maximum_number_exceeded
 *  
 */
int
calculate_output_data_list();

/** @brief  generate run profile output csv files, 
 *          one file per start-stop per train
 *  @return err_list_iteration_failed 
 *          err_list_retrieval_failed
 *          err_maximum_number_exceeded
 *  
 */
int
export_run_profile_file();

#endif
