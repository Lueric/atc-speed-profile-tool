#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <inttypes.h>
#include <stdbool.h>
#include <limits.h>
#include "common_util.h"
#include "simclist.h"

#define LIST_MAX_SIZE     UINT_MAX
#define RUN_PROFILES      "."
#define SUMMARY_FILE_NAME "output//travel_time_summary.csv"

/*
** Structures
** -----------------------------------------------------
*/

/* input data structure */
typedef struct station_travel_time_t 
{
  /* unique id, range [0, 4,294,967,295] */
  uint64_t id;
  /* sorting value "XXXXnnnNN" */
  /* combination of str_station_code, str_platform, str_cc_id, str_file_cnt */
  char sorting_str[STR_MEDIUM];
  /* YYYYMMDDHHMMSS converted from str_time */
  char str_timestamp[STR_MEDIUM]; 

  char filename[STR_MAX];
  char str_station_code[STR_MIN];
  char str_platform[STR_MIN];
  char str_cc_id[STR_MIN];
  char str_file_cnt[STR_MIN];
  char str_travel_time[STR_MIN];

  int cc_id;
  int file_cnt;
  double travel_time_s;

} station_travel_time;

/*
** Variables
** -----------------------------------------------------
*/

/* system operation errno */
extern int errno;
/* linked list to store input data */
static list_t station_travel_time_list;
static uint64_t list_id = 0;
/* static string to support _to_string function */
static char str_static_data[STR_EXTRA];

/* 
** Supporting Functions (If any)
*/

/* required functions to manipulate the linked list */
/* comparator function required for list_sort function */
/* using the event sorting_str to sort the list */
int 
station_travel_time_data_comparator(const void *a, const void *b) 
{
  const station_travel_time *A = (station_travel_time *) a;
  const station_travel_time *B = (station_travel_time *) b;

  return strcmp(A->sorting_str, B->sorting_str);
}

/* meter function required for the list_append function */
/* return size of the inputData structure */
size_t 
station_travel_time_data_meter(const void *el) 
{
  return sizeof(station_travel_time);
}

/** @brief  initialize input data list
 *  @param  none
 *  @return none
 *             
 */
void 
init_station_travel_time_list()
{
  /* list initialization */
  list_init(&station_travel_time_list);
  list_attributes_copy(&station_travel_time_list, station_travel_time_data_meter, 1);
  list_attributes_comparator(&station_travel_time_list, station_travel_time_data_comparator);
}

/** @brief  sort input data list
 *  @param  none
 *  @return none
 *             
 */
int 
sort_station_travel_time_list()
{
  if (list_empty(&station_travel_time_list))
  {
    return throw_err(err_list_is_empty);    
  }
  else
  {
    list_sort(&station_travel_time_list, -1);
    return 0;
  }
}

/** @brief  clear and free input data list
 *  @param  none
 *  @return none
 *             
 */
void 
free_station_travel_time_list()
{
  list_destroy(&station_travel_time_list);
}

/** @brief  add input data structure to the static input data list
 *  @param  *data  pointer to the new input data structure 
 *  @return err_list_append_failed
 *      
 */
int 
add_to_station_travel_time_list(station_travel_time *data) 
{
  
  if (1 != list_append(&station_travel_time_list, data)) 
  {
      return throw_err(err_list_append_failed);
  }  

  return 0;
}

/** @brief  convert inputData and format into string 
 *  @param  *data  inputData to be converted 
 *  @return NULL      input is not valid
 *          converted static string  
 *             
 */
char *
station_travel_time_to_string(station_travel_time *data)
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
  if (NULL != data->filename)
  {    
    strcat(str_static_data, data->filename);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_timestamp)
  {    
    strcat(str_static_data, data->str_timestamp);
  } 
  strcat(str_static_data, ",");
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
  if (NULL != data->str_cc_id)
  {    
    strcat(str_static_data, data->str_cc_id);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_file_cnt)
  {    
    strcat(str_static_data, data->str_file_cnt);
  }
  strcat(str_static_data, ",");
  if (NULL != data->str_travel_time)
  {    
    strcat(str_static_data, data->str_travel_time);
  }
  strcat(str_static_data, ",");

  snprintf(tmp, STR_EXTRA, "%03d,%02d,% .1f", 
           data->cc_id, 
           data->file_cnt, 
           data->travel_time_s);
  strcat(str_static_data, tmp);

  return str_static_data;
}

/** @brief  read input data file and add to input data list 
 *  @param  *str_lut_file  input data file path 
 *  @return err_list_append_failed 
 *          err_file_not_accessible 
 */
int 
read_input_file(char *str_data_file)
{
  
  int err = 0; 
  int i = 0;
  
  char str_data_line[STR_MAX] = "";
  char tmp_line[STR_MAX] = "";
  char *str_tmp = NULL;
  char *p_tmp = NULL;
  station_travel_time input_data = {0};
  FILE * p_data_file = NULL; 

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
    /* skip through to last line */
    while (NULL != (fgets(str_data_line, STR_MAX, p_data_file)) && (0 == err))
    {
      strncpy(tmp_line, str_data_line, STR_MAX);
    }
    /* tmp_line is the last line */
    fclose(p_data_file);

    str_tmp = strtok(tmp_line, ",");
    while (i < 12)
    {
      str_tmp = strtok(NULL, ",");
      i++;
    }
    strncpy(input_data.str_travel_time, trim(str_tmp), STR_MIN);
    input_data.travel_time_s = (double) (strtol(str_tmp, &p_tmp, 10));
    
    strncpy(input_data.filename, str_data_file, STR_MAX);
    str_tmp = strtok(str_data_file, "_");
    i=0;
    while (i <= 6)
    {
      str_tmp = strtok(NULL, "_");
      if (2==i)
      {
        if (str_tmp[0] >= '0' && str_tmp[0] <= '9')
        {
          /* station not known */
          strncpy(input_data.str_station_code, "___", STR_MIN);
          strncpy(input_data.str_platform, "_", STR_MIN);

          /* token is file cnt */
          strncpy(input_data.str_file_cnt, str_tmp, STR_MIN);
          input_data.file_cnt = (int) strtol(str_tmp, (char **)NULL, 10);

          /* skip next iteration */
          i++;
        }
        else if (str_tmp[0] >= 'A' && str_tmp[0] <= 'Z')
        {
          strncpy(input_data.str_station_code, str_tmp, STR_MIN);
          input_data.str_station_code[3] = '\0';
          input_data.str_platform[0] = str_tmp[3];
          input_data.str_platform[1] = '\0';
        }        
      }

      else if (3==i)
      {
        strncpy(input_data.str_file_cnt, str_tmp, STR_MIN);
        input_data.file_cnt = (int) strtol(str_tmp, (char **)NULL, 10);
      }
      else if (4==i)
      {
        input_data.str_cc_id[0] = str_tmp[2];
        input_data.str_cc_id[1] = str_tmp[3];
        input_data.str_cc_id[2] = str_tmp[4];
        input_data.str_cc_id[3] = '\0';

        input_data.cc_id = (int) strtol(input_data.str_cc_id, (char **)NULL, 10);
      }

      i++;
    }
    if (LIST_MAX_SIZE == list_id)
    {
      /* reach the maximum number of input list id */
      return throw_err(err_maximum_number_exceeded);
    }
    else
    {
      input_data.id = list_id;
      list_id++;

      snprintf
      (
        input_data.sorting_str, 
        STR_MEDIUM,
        "%s%s%s%s", 
        input_data.str_station_code, 
        input_data.str_platform, 
        input_data.str_cc_id, 
        input_data.str_file_cnt
      );

      /* add input data to input data list */
      err = add_to_station_travel_time_list(&input_data);
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
        
      }
    }
  }
  else
  {
    fprintf(stdout, "[%6s][%s][%s]\n", "ERROR", "Data Files Not Found!", str_data_file);
    err = throw_err(err_file_not_accessible);
  }

  return err;
}

/** @brief  generate run profile output csv files, 
 *          one file per start-stop per train
 *  @return err_list_iteration_failed 
 *          err_list_retrieval_failed
 *          err_maximum_number_exceeded
 *  
 */
int
export_summary_file()
{
  FILE *fp_out = NULL;

  char output_file_full_path[STR_MAX] = "";
  
  int err = 0;
  station_travel_time *p_data = NULL;

  bool b_enabled = true;

  /* start iteration */
  /* iteration start on output list */
  /* check err, 0 - not able to start iteration; 1 - ok to iterate */
  if (1 != list_iterator_start(&station_travel_time_list))
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

  strcpy(output_file_full_path, SUMMARY_FILE_NAME);
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
      /* write header to new file */      
      fprintf
      (
        fp_out, 
        "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", 
        "id",
        "sorting_str",
        "filename",
        "str_timestamp",
        "str_station_code",
        "str_platform",
        "str_cc_id",
        "str_file_cnt",
        "str_travel_time",
        "cc_id",
        "file_cnt",
        "travel_time_s"
      );

      while (list_iterator_hasnext(&station_travel_time_list) && (b_enabled)) 
      { 
        /* check for next element */
        p_data = (station_travel_time *)list_iterator_next(&station_travel_time_list);
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
          /* write the current output data to the output file */
          fprintf
          (
            fp_out, 
            "%s\n", 
            station_travel_time_to_string(p_data)
          );

        }
        /* end of current iteration */
      } /* end of while loop */

      if (NULL != fp_out)
      {
        fclose(fp_out);
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
      "Speed Profiles Export Process Interrupted!"
    );
  }

  if (1 != list_iterator_stop(&station_travel_time_list))
  {
    return throw_err(err_list_stop_failed);
  }
  else
  {
    return err;
  }
}

int main()
{	
	
	char current_dir[STR_MAX]; //Variable to store current working directory
  char temp_csv_file[STR_MAX]; //Temp variable to store csv name (to be broken into tokens)
	char csv_file[STR_MAX]; //Variable to store csv name

  int err = 0;

	WIN32_FIND_DATA fdFile; //WIN32_FIND_DATA is a struct that contains many file attributes.
	HANDLE hfile = NULL; //Pointer to a struct for windows.

  strcpy(current_dir, RUN_PROFILES); //Retrieve the current working directory.
	strcat(current_dir, "\\*.csv"); //Concatenate "\*.csv" to indicate extension to look for.		
	
	hfile = FindFirstFile(current_dir, &fdFile); //Find the first .csv file and save its info to data
	
  /* initiate the list */


	/*
	** If hfile returns an INVALID_HANDLE_VALUE, print the appropriate error message
	** and terminate the program.
	*/
	if (hfile == INVALID_HANDLE_VALUE) 
  {
    printf ("Unable to find any *.csv files. Terminating.\n");

    return 0;
  }
	else
	{
		printf("Searching for proper .csv files...\n");	

    init_station_travel_time_list();

    do
    {
      //Find first file will always return "."
      //    and ".." as the first two directories.
      if
      (
        strcmp(fdFile.cFileName, ".") != 0 && 
        strcmp(fdFile.cFileName, "..") != 0 &&
        !(fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
      )
      {
        /* file found */
        strcpy(temp_csv_file, fdFile.cFileName);
        strcpy(csv_file, fdFile.cFileName);

        /* create a new node */
        err = read_input_file(csv_file);
        if (err < 0)
        {
          fprintf
          (
            stdout, 
            "[%6s][%s][%s][%s]\n", 
            "INFO", 
            "Data Files Import Failed!", 
            get_err_description(err), 
            csv_file
          );
        }
      
      }
    }
    while(FindNextFile(hfile, &fdFile)); //Find the next file.

    FindClose(hfile); //Always, Always, clean things up!

    /* output the list to file */
    err = export_summary_file();
    if (err < 0)
    {
      fprintf
      (
        stdout, 
        "[%6s][%s][%s][%s]\n", 
        "INFO", 
        "Summary File Export Failed!", 
        get_err_description(err), 
        "Terminating..."
      );
    }

  }

  free_station_travel_time_list();

  system("PAUSE");
  
  
  return EXIT_SUCCESS;

}

