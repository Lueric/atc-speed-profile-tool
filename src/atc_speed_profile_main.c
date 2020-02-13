# ifndef _WIN32
#   include <unistd.h>
# else
#   include <windows.h> 
# endif 
#include "atc_speed_profile_tool.h"

/* 
** Main Program Code
*/
int 
main(int argc, char *argv[])
{
  int len_data_file_name = 0; 
  char str_data_file[STR_MAX] = "";
  char str_input_file_list[FILE_LIST_MAX_LENGTH][STR_MAX] = {""};

  int err = 0; 
  bool b_enabled = true;

  int i = 0;
  int input_file_cnt = 0;

  /* display usage */
  display_usage(argv);

  /* check command line format */
  if (argc > FILE_LIST_MAX_LENGTH)
  {
    fprintf
    (
      stdout, 
      "[%6s][%s]\n", 
      "ERROR", 
      "Command Line Arguments Too Many!"
    );
    fprintf
    (
      stdout, 
      "[%6s][%s%d]\n", 
      "ERROR", 
      "Maximum number of input files is ",
      FILE_LIST_MAX_LENGTH
    );
    return EXIT_FAILURE;
  }
  else if (argc < 2)
  {
    fprintf
    (
      stdout, 
      "[%6s][%s]\n", 
      "ERROR", 
      "CSV Data File Not Defined!"
    );
    return EXIT_FAILURE;
  }
  else
  {
    for (i = 1; i < argc; i++)
    {
      if (NULL == argv[i])
      {
        fprintf
        (
          stdout, 
          "[%6s][%s]\n", 
          "ERROR", 
          "CSV Data File Not Defined!"
        );
        return EXIT_FAILURE;
      }
      else if (strlen(argv[i]) >= STR_MAX)
      {
        fprintf
        (
          stdout, 
          "[%6s][%s]\n", 
          "ERROR", 
          "CSV Data File Name Too Long!"
        );
        return EXIT_FAILURE;
      }
      strcpy(str_data_file, argv[i]);

      /* check extension is csv */
      if 
      (
        ((len_data_file_name = strlen(str_data_file)) <= 4) ||
        (strcmp(str_data_file+(len_data_file_name-4), ".csv") != 0)
      )
      {
        fprintf
        (
          stdout, 
          "[%6s][%s]\n", 
          "ERROR", 
          "Data File Must Be CSV!"
        );
        return EXIT_FAILURE;
      }
      else
      {
        strcpy(str_input_file_list[input_file_cnt], argv[i]);
        input_file_cnt++;
      }
    }    
  }

  if (input_file_cnt < 1 || input_file_cnt > FILE_LIST_MAX_LENGTH)
  {
    fprintf
    (
      stdout, 
      "[%6s][%s]\n", 
      "ERROR", 
      "No Valid Input Files!"
    );
    return EXIT_FAILURE;
  }
  
  init_lut_data_list();
  init_input_data_list();
  init_output_data_list(); 

  /* read lookup file */
  fprintf
  (
    stdout, 
    "[%6s][%s][%s]\n", 
    "INFO", 
    "Import Configuration Files...", 
    BLOCK_LUT_FILE
  );
  err = read_lut_file(BLOCK_LUT_FILE);
  if (err < 0)
  {
    fprintf
    (
      stdout, 
      "[%6s][%s][%s][%s]\n", 
      "INFO", 
      "Configuration Files Import Failed!", 
      get_err_description(err), 
      BLOCK_LUT_FILE
    );
    b_enabled = false;
  }
  else
  {
    fprintf
    (
      stdout, 
      "[%6s][%s]\n", 
      "INFO", 
      "Configuration Files Imported Successfully!" 
    );
  }

  /* read input data file */
  for (i = 0; i < input_file_cnt; i++)
  {
    strcpy(str_data_file, str_input_file_list[i]);
    if (b_enabled)
    {
      fprintf
      (
        stdout, 
        "[%6s][%s][%s]\n", 
        "INFO", 
        "Import Data Files...", 
        str_data_file
      );
      err = read_input_file(str_data_file);
      if (err < 0)
      {
        fprintf
        (
          stdout, 
          "[%6s][%s][%s][%s]\n", 
          "INFO", 
          "Data Files Import Failed!", 
          get_err_description(err), 
          str_data_file
        );
        b_enabled = false;
      }
      else
      {
        fprintf
        (
          stdout, 
          "[%6s][%s]\n", 
          "INFO", 
          "Data Files Imported Successfully!" 
        );
      }
    }
  }

  /* sort the input data list */
  if (b_enabled)
  {
    fprintf
    (
      stdout, 
      "[%6s][%s]\n", 
      "INFO", 
      "Sort Input Data..."
    );    
    err = sort_input_data_list();
    if (err < 0)
    {
      fprintf
      (
        stdout, 
        "[%6s][%s][%s]\n", 
        "INFO", 
        "Input Data Sort Failed!", 
        get_err_description(err)
      );
      b_enabled = false;
    }
    else
    {
      fprintf
      (
        stdout, 
        "[%6s][%s]\n", 
        "INFO", 
        "Input Data Sorted Successfully!" 
      );
    }
  }  

  /* Input Data Preprocessing with lookup table */
  if (b_enabled)
  {
    fprintf
    (
      stdout, 
      "[%6s][%s]\n", 
      "INFO", 
      "Preprocess Input Data..."
    );
    err = expand_data_list_use_lut();
    if (err < 0)
    {
      fprintf
      (
        stdout, 
        "[%6s][%s][%s]\n", 
        "INFO", 
        "Input Data Preprocess Failed!", 
        get_err_description(err)
      );
      b_enabled = false;
    }
    else
    {
      fprintf
      (
        stdout, 
        "[%6s][%s]\n", 
        "INFO", 
        "Input Data Preprocessed Successfully!" 
      );
    }
  }  
  
  /* process input data list and generate output data list */
  if (b_enabled)
  {
    fprintf
    (
      stdout, 
      "[%6s][%s]\n", 
      "INFO", 
      "Calculate Speed Profiles..."
    );
    err = calculate_output_data_list();
    if (err < 0)
    {
      fprintf
      (
        stdout, 
        "[%6s][%s][%s]\n", 
        "INFO", 
        "Speed Profiles Calculation Failed!", 
        get_err_description(err) 
      );
      b_enabled = false;
    }
    else
    {
      fprintf
      (
        stdout, 
        "[%6s][%s]\n", 
        "INFO", 
        "Speed Profiles Calculated Successfully!" 
      );
    }
  } 

  /* output to several csv file, one file per start-stop per train */
  if (b_enabled)
  {
    fprintf
    (
      stdout, 
      "[%6s][%s]\n", 
      "INFO", 
      "Export Speed Profiles..." 
    );
    err = export_run_profile_file();
    if (err < 0)
    {
      fprintf
      (
        stdout, 
        "[%6s][%s][%s]\n", 
        "INFO", 
        "Speed Profiles Export Failed!", 
        get_err_description(err) 
      );
      b_enabled = false;
    }
    else
    {
      fprintf
      (
        stdout, 
        "[%6s][%s]\n", 
        "INFO", 
        "Speed Profiles Exported Successfully!" 
      );
    }
  }  
  
  /* list clean up */
  free_lut_data_list();
  free_input_data_list();
  free_output_data_list();
  /* wait several seconds before exit */
  Sleep(10);
  return EXIT_SUCCESS;
}
