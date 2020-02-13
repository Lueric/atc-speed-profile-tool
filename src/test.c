#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
//#include "include/cmocka.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "atc_speed_profile_tool.h"


void 
test_display_usage()
{
  char *str[2] = {"atc-speed-profile", "123.csv"};
  display_usage(str);
}

void
test_str_to_seconds()
{
  long tt = 0;
  char buffer[STR_MAX] = "";

  tt = str_to_seconds(NULL);
  printf("[str_to_seconds][NULL][%ld]\n", tt);
  tt = str_to_seconds("");
  printf("[str_to_seconds][\"\"][%ld]\n", tt);
  tt = str_to_seconds("2018/08/27 20:00:00");
  printf("[str_to_seconds][\"2018/08/27 20:00:00\"][%ld]\n", tt);
  strftime(buffer, STR_MAX, "%Y-%m-%d %H:%M:%S", localtime(&tt));
  printf("[str_to_seconds][\"2018/08/27 20:00:00\"][%s]\n", buffer);
  tt = str_to_seconds("1018/08/27 20:00:00");
  printf("[str_to_seconds][\"1018/08/27 20:00:00\"][%ld]\n", tt);
  tt = str_to_seconds("2018/00/27 20:00:00");
  printf("[str_to_seconds][\"2018/00/27 20:00:00\"][%ld]\n", tt);
  tt = str_to_seconds("2018/13/27 20:00:00");
  printf("[str_to_seconds][\"2018/13/27 20:00:00\"][%ld]\n", tt);
  tt = str_to_seconds("2018/08/00 20:00:00");
  printf("[str_to_seconds][\"2018/08/00 20:00:00\"][%ld]\n", tt);
  tt = str_to_seconds("2018/08/40 20:00:00");
  printf("[str_to_seconds][\"2018/08/40 20:00:00\"][%ld]\n", tt);
  tt = str_to_seconds("2018/08/27 -3:00:00");
  printf("[str_to_seconds][\"2018/08/27 -3:00:00\"][%ld]\n", tt);
  tt = str_to_seconds("2018/08/27 25:00:00");
  printf("[str_to_seconds][\"2018/08/27 25:00:00\"][%ld]\n", tt);
  tt = str_to_seconds("2018/08/27 20:-2:00");
  printf("[str_to_seconds][\"2018/08/27 20:-2:00\"][%ld]\n", tt);
  tt = str_to_seconds("2018/08/27 20:70:00");
  printf("[str_to_seconds][\"2018/08/27 20:70:00\"][%ld]\n", tt);
  tt = str_to_seconds("2018/08/27 20:00:-5");
  printf("[str_to_seconds][\"2018/08/27 20:00:-5\"][%ld]\n", tt);
  tt = str_to_seconds("2018/08/27 20:00:60");
  printf("[str_to_seconds][\"2018/08/27 20:00:60\"][%ld]\n", tt);

}

void
test_is_header()
{
  bool b_res = false;
  char line[STR_MAX] = "TIME,LOCATION,BLOCK,RUN NUMBER,DIRECTION,DESTINATION CODE,ORIGINATION CODE, SCHEDULE CLASS,CC ID, CURRENT DRIVING MODE, SELECTED DRIVING MODE,TALKATIVE, MOTION, SKIP STOP, DOORS OPEN,DOOR FAULT, ALARM, EMERGENCY BRAKE, SPEED";
  int num_digit_hi = k_header_num_digit;
  b_res = is_header(line, num_digit_hi);
  printf("[is_header][HEADER][%d]\n", b_res);
  strcpy(line, "2018/08/27 20:00:00,Vaughan Metro Centre,IVB_542,K150,L,K,H,X,27,U,U,1,0,0,0,0,1,0,0");
  b_res = is_header(line, num_digit_hi);
  printf("[is_header][  ROW1][%d]\n", b_res);
  strcpy(line, "2018/08/27 20:00:24,Finch West,IVB_462,H011,L,H,K,R,9,A,A,1,1,0,0,0,0,0,56");
  b_res = is_header(line, num_digit_hi);
  printf("[is_header][  ROW2][%d]\n", b_res);
  strcpy(line, "");
  b_res = is_header(line, num_digit_hi);
  printf("[is_header][    \"\"][%d]\n", b_res);
  b_res = is_header(NULL, num_digit_hi);
  printf("[is_header][  NULL][%d]\n", b_res);
}

void
test_is_csv_line()
{
  bool b_res = false;
  char line[STR_MAX] = "TIME,LOCATION,BLOCK,RUN NUMBER,DIRECTION,DESTINATION CODE,ORIGINATION CODE, SCHEDULE CLASS,CC ID, CURRENT DRIVING MODE, SELECTED DRIVING MODE,TALKATIVE, MOTION, SKIP STOP, DOORS OPEN,DOOR FAULT, ALARM, EMERGENCY BRAKE, SPEED";
  int num_col_lo = k_lut_header_cnt;
  b_res = is_csv_line(line, num_col_lo);
  printf("[is_csv_line][ NORM CSV][%d]\n", b_res);
  strcpy(line, "Sheppard West,IVB_504,K104,R,K,H,R,58,C,C,1,1,1,0,0,1,");
  b_res = is_header(line, num_col_lo);
  printf("[is_csv_line][FEWER COL][%d]\n", b_res);
  strcpy(line, "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22,");
  b_res = is_header(line, num_col_lo);
  printf("[is_csv_line][FEWER COL][%d]\n", b_res);
  strcpy(line, "");
  b_res = is_header(line, num_col_lo);
  printf("[is_csv_line][       \"\"][%d]\n", b_res);
  b_res = is_header(NULL, num_col_lo);
  printf("[is_csv_line][     NULL][%d]\n", b_res);

}

void
test_parse_input_header()
{
  int err = 0;
  char line[STR_MAX] = "TIME,LOCATION,BLOCK,RUN NUMBER,DIRECTION,DESTINATION CODE,ORIGINATION CODE, SCHEDULE CLASS,CC ID, CURRENT DRIVING MODE, SELECTED DRIVING MODE,TALKATIVE, MOTION, SKIP STOP, DOORS OPEN,DOOR FAULT, ALARM, EMERGENCY BRAKE, SPEED";
  err = parse_input_header(line);
  printf("[parse_input_header][err = %d][%s]\n", err, get_err_description(err));
  strcpy(line, "TIME,LOCATION,BLOCK,RUN NUMBER,DIRECTION,DESTINATION CODE,ORIGINATION CODE, SCHEDULE CLASS,CC ID, CURRENT DRIVING MODE, SELECTED DRIVING MODE,TALKATIVE, MOTION, SKIP STOP, DOORS OPEN,DOOR FAULT, ALARM, SPEED, EMERGENCY BRAKE");
  err = parse_input_header(line);
  printf("[parse_input_header][err = %d][%s]\n", err, get_err_description(err));
  strcpy(line, "TIME,LOCATION,BLOCK,RUN NUMBER,DIRECTION,DESTINATION CODE,ORIGINATION CODE, SCHEDULE CLASS,CC ID, CURRENT DRIVING MODE, SELECTED DRIVING MODE,TALKATIVE, MOTION, SKIP STOP, DOORS OPEN,DOOR FAULT, ALARM, EMERGENCY BRAKE, SPEE");
  err = parse_input_header(line);
  printf("[parse_input_header][err = %d][%s]\n", err, get_err_description(err));
  strcpy(line, "TIME,LOCATION,BLOCK,RUN NUMBER,DIRECTION,DESTINATION CODE,ORIGINATION CODE, SCHEDULE CLASS,CC ID, CURRENT DRIVING MODE, SELECTED DRIVING MODE,TALKATIVE, MOTION, SKIP STOP, DOORS OPEN,DOOR FAULT, ALARM, EMERGENCY BRAKE");
  err = parse_input_header(line);
  printf("[parse_input_header][err = %d][%s]\n", err, get_err_description(err));
  strcpy(line, "TIME,LOCATION,BLOCK,RUN NUMBER,DIRECTION,DESTINATION CODE,ORIGINATION CODE, SCHEDULE CLASS,CC ID, CURRENT DRIVING MODE, SELECTED DRIVING MODE,TALKATIVE, MOTION, SKIP STOP, DOORS OPEN,DOOR FAULT, ALARM, EMERGENCY BRAKE, SPEED, TIME");
  err = parse_input_header(line);
  printf("[parse_input_header][err = %d][%s]\n", err, get_err_description(err));
  strcpy(line, "TIME,LOCATION,BLOCK,RUN NUMBER,DIRECTION,DESTINATION CODE,ORIGINATION CODE, SCHEDULE CLASS,CC ID, CURRENT DRIVING MODE, SELECTED DRIVING MODE,TALKATIVE, MOTION, SKIP STOP, DOORS OPEN,DOOR FAULT, ALARM, EMERGENCY BRAKE, SPEED");
  err = parse_input_header(line);
  printf("[parse_input_header][err = %d][%s]\n", err, get_err_description(err));
}

void 
test_parse_input_data()
{
  int err = 0;
  inputData data = {0};
  char str_data_line[STR_MAX] = "";
  char str_data_file[STR_MAX] = "1234.csv";
  /* date not valid */
  strcpy(str_data_line, "H011,L,H,K,R,9,A,A,1,1,0,0,0,0,0,55");
  err = parse_input_data(&data, str_data_line, str_data_file);
  printf("[parse_input_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_input_data][%s]\n", input_data_to_string(&data));
  /* OK line */
  strcpy(str_data_line, "2018/08/27 20:00:00,Sheppard West,IVB_504,K171,R,K,H,R,20,A,A,1,1,0,0,0,1,0,4");
  err = parse_input_data(&data, str_data_line, str_data_file);
  printf("[parse_input_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_input_data][%s]\n", input_data_to_string(&data));
  /* cc_num <= 0 */
  strcpy(str_data_line, "2018/08/27 20:00:00,Sheppard West,IVB_504,K171,R,K,H,R,-20,A,A,1,1,0,0,0,1,0,4");
  err = parse_input_data(&data, str_data_line, str_data_file);
  printf("[parse_input_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_input_data][%s]\n", input_data_to_string(&data));
  /* cc_num > 999 */
  strcpy(str_data_line, "2018/08/27 20:00:00,Sheppard West,IVB_504,K171,R,K,H,R,1200,A,A,1,1,0,0,0,1,0,4");
  err = parse_input_data(&data, str_data_line, str_data_file);
  printf("[parse_input_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_input_data][%s]\n", input_data_to_string(&data));
  /* motion null */
  strcpy(str_data_line, "2018/08/27 20:00:00,Sheppard West,IVB_504,K171,R,K,H,R,20,A,A,1, ,0,0,0,1,0,4");
  err = parse_input_data(&data, str_data_line, str_data_file);
  printf("[parse_input_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_input_data][%s]\n", input_data_to_string(&data));
  /* motion format */
  strcpy(str_data_line, "2018/08/27 20:00:00,Sheppard West,IVB_504,K171,R,K,H,R,20,A,A,1,12,0,0,0,1,0,4");
  err = parse_input_data(&data, str_data_line, str_data_file);
  printf("[parse_input_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_input_data][%s]\n", input_data_to_string(&data));
  /* motion not 1 or 0 */
  strcpy(str_data_line, "2018/08/27 20:00:00,Sheppard West,IVB_504,K171,R,K,H,R,20,A,A,1,2,0,0,0,1,0,4");
  err = parse_input_data(&data, str_data_line, str_data_file);
  printf("[parse_input_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_input_data][%s]\n", input_data_to_string(&data));

  /* seg fault line */
  strcpy(str_data_line, "2018/08/27 20:59:28,Highway 407,IVB_483,H131,L,H,K,R,64,A,A,1,1,0,0,0,1,0,5");
  err = parse_input_data(&data, str_data_line, str_data_file);
  printf("[parse_input_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_input_data][%s]\n", input_data_to_string(&data));
  

}

void 
test_parse_lut_data()
{
  int err = 0;
  lutData data = {0};
  char str_data_line[STR_MAX] = "";
  /* OK lut line */
  strcpy(str_data_line, "1,Sheppard West Portal,IVB_443,L,N,2,,WIL,200.5,Wilson,Sheppard West Portal");
  err = parse_lut_data(&data, str_data_line);
  printf("[parse_lut_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_lut_data][%s]\n", lut_data_to_string(&data));
  /* direction num null */
  strcpy(str_data_line, "1,Sheppard West Portal,IVB_443,L,N,,,WIL,200.5,Wilson,Sheppard West Portal");
  err = parse_lut_data(&data, str_data_line);
  printf("[parse_lut_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_lut_data][%s]\n", lut_data_to_string(&data));
  /* direction num len > 1 */
  strcpy(str_data_line, "1,Sheppard West Portal,IVB_443,L,N,22,,WIL,200.5,Wilson,Sheppard West Portal");
  err = parse_lut_data(&data, str_data_line);
  printf("[parse_lut_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_lut_data][%s]\n", lut_data_to_string(&data));
  /* direction num = 0 */
  strcpy(str_data_line, "1,Sheppard West Portal,IVB_443,L,N,0,,WIL,200.5,Wilson,Sheppard West Portal");
  err = parse_lut_data(&data, str_data_line);
  printf("[parse_lut_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_lut_data][%s]\n", lut_data_to_string(&data));
  /* direction num = 3 */
  strcpy(str_data_line, "1,Sheppard West Portal,IVB_443,L,N,3,,WIL,200.5,Wilson,Sheppard West Portal");
  err = parse_lut_data(&data, str_data_line);
  printf("[parse_lut_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_lut_data][%s]\n", lut_data_to_string(&data));
  /* str_platform null */
  strcpy(str_data_line, "1,Sheppard West Portal,IVB_443,L,N,2,,WIL,200.5,Wilson,Sheppard West Portal");
  err = parse_lut_data(&data, str_data_line);
  printf("[parse_lut_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_lut_data][%s]\n", lut_data_to_string(&data));
  /* str_platform len > 1 */
  strcpy(str_data_line, "1,Sheppard West Portal,IVB_443,L,N,2,12,WIL,200.5,Wilson,Sheppard West Portal");
  err = parse_lut_data(&data, str_data_line);
  printf("[parse_lut_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_lut_data][%s]\n", lut_data_to_string(&data));
  /* str_block null */
  strcpy(str_data_line, "1,Sheppard West Portal,,L,N,2,,WIL,200.5,Wilson,Sheppard West Portal");
  err = parse_lut_data(&data, str_data_line);
  printf("[parse_lut_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_lut_data][%s]\n", lut_data_to_string(&data));
  /* str_block no _ */
  strcpy(str_data_line, "1,Sheppard West Portal,IVB443,L,N,2,,WIL,200.5,Wilson,Sheppard West Portal");
  err = parse_lut_data(&data, str_data_line);
  printf("[parse_lut_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_lut_data][%s]\n", lut_data_to_string(&data));
  /* str_block not a number */
  strcpy(str_data_line, "1,Sheppard West Portal,IVB_yui,L,N,2,,WIL,200.5,Wilson,Sheppard West Portal");
  err = parse_lut_data(&data, str_data_line);
  printf("[parse_lut_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_lut_data][%s]\n", lut_data_to_string(&data));
  /* str_direction null */
  strcpy(str_data_line, "1,Sheppard West Portal,IVB_443,,N,2,,WIL,200.5,Wilson,Sheppard West Portal");
  err = parse_lut_data(&data, str_data_line);
  printf("[parse_lut_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_lut_data][%s]\n", lut_data_to_string(&data));
  /* str_direction len > 1 */
  strcpy(str_data_line, "1,Sheppard West Portal,IVB_443,L9,N,2,,WIL,200.5,Wilson,Sheppard West Portal");
  err = parse_lut_data(&data, str_data_line);
  printf("[parse_lut_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_lut_data][%s]\n", lut_data_to_string(&data));
  /* str_direction 3 */
  strcpy(str_data_line, "1,Sheppard West Portal,IVB_443,3,N,2,,WIL,200.5,Wilson,Sheppard West Portal");
  err = parse_lut_data(&data, str_data_line);
  printf("[parse_lut_data][err = %d][%s]\n", err, get_err_description(err));
  printf("[parse_lut_data][%s]\n", lut_data_to_string(&data));

}

void 
test_get_output_file()
{
  int err = 0;
  outputData data = {0};
  char filename[STR_MAX] = "";
  char str_data_line[STR_MAX] = "";
  /* OK lut line */
  strcpy(str_data_line, "1,Sheppard West Portal,IVB_443,L,N,2,,WIL,200.5,Wilson,Sheppard West Portal");
  err = get_output_file(filename, &data);
  printf("[get_output_file][err = %d][%s]\n", err, get_err_description(err));
  printf("[get_output_file][%s]\n", filename);

}

void 
test_read_lut_file()
{
  int err = 0;
  lutData *p_lut_data = NULL;
  char str_lut_file[STR_MAX] = "";

  /* lut file name invalid */
  err = read_lut_file(str_lut_file);
  printf("[read_lut_file][%d][err = %d][%s]\n", __LINE__, err, get_err_description(err));
  /* lut file name valid */
  strcpy(str_lut_file, "test\\atc_block_lut.csv");
  err = read_lut_file(str_lut_file);
  printf("[read_lut_file][%d][err = %d][%s]\n", __LINE__, err, get_err_description(err));

  display_lut_data_list(); 

}

void
test_read_input_file()
{
  int err = 0;
  inputData *p_data = NULL;
  char str_file[STR_MAX] = "";

  /* input file name invalid */
  err = read_input_file(str_file);
  printf("[read_input_file][%d][err = %d][%s]\n", __LINE__, err, get_err_description(err));
  /* input file name valid */
  strcpy(str_file, "test\\traindata-test-simple.csv");
  err = read_input_file(str_file);
  printf("[read_input_file][%d][err = %d][%s]\n", __LINE__, err, get_err_description(err));

  //display_input_data_list(); 

}

void
test_expand_data_use_lut()
{
  int err = 0;
  inputData *p_data = NULL;
  
  err = expand_data_list_use_lut();
  printf("[expand_data_use_lut][%d][err = %d][%s]\n", __LINE__, err, get_err_description(err));

  
}

void
test_calculate_output_data_list()
{
  int err = 0;
  err = calculate_output_data_list();
  printf("[calculate_output_data_list][%d][err = %d][%s]\n", __LINE__, err, get_err_description(err));
}

void
test_export_run_profile_file()
{
  int err = 0;
  err = export_run_profile_file();
  printf("[export_run_profile_file][%d][err = %d][%s]\n", __LINE__, err, get_err_description(err));
}


int 
main(int argc, char *argv[])
{
  int err = 0;

  /*
  test_display_usage();
  test_str_to_seconds();
  test_is_header();
  test_is_csv_line();
  test_parse_input_header();
  test_parse_input_data();
  test_parse_lut_data();
  test_get_output_file();

  /* initialize lists */
  init_lut_data_list();
  init_input_data_list();
  init_output_data_list(); 
  /* continue test */

  
  test_read_lut_file();
  test_read_input_file();

  /* sort input data list */
  err = sort_input_data_list();
  printf("[sort_input_data_list][%d][err = %d][%s]\n", __LINE__, err, get_err_description(err));
  
  //display_input_data_list(); 

  /* expand data with lookup table */
  test_expand_data_use_lut();

  display_input_data_list(); 

  test_calculate_output_data_list();
  
  display_output_data_list(); 

  test_export_run_profile_file();

  printf("[  INFO] TEST DONE\n");
  printf("[  INFO] Clean up ...");
  free_lut_data_list();
  free_input_data_list();
  free_output_data_list();
  printf("DONE\n");

  //system("PAUSE");
  return 0;
}

