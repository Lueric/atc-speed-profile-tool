#include "simclist.h"
#include "errorhandler.h"
#include "common_util.h"


void 
display_usage();
time_t
str_to_seconds(const char *str_date_time);
bool 
is_header(const char *line, int num_digit_hi);
bool
is_csv_line(const char *line, int num_col_lo);
errorCode
parse_input_header(const char *line);
errorCode 
parse_input_data(inputData *input_data, char *str_data_line, char *str_data_file);
errorCode 
parse_lut_data(lutData *input_data, char *str_data_line);

errorCode
expand_data_use_lut(inputData *input_data);


errorCode
get_output_file(char *filename, outputData *data);


char *
input_data_to_string(inputData *data);
char *
lut_data_to_string(lutData *data);
char *
output_data_to_string(outputData *data);


errorCode 
add_to_input_data_list(inputData *data);
errorCode 
add_to_output_data_list(outputData *data);
errorCode 
add_to_lut_data_list(lutData *data);


errorCode
read_lut_file(char *str_lut_file);
errorCode 
read_input_file(char *str_data_file);
errorCode
expand_data_list_use_lut();

errorCode
calculate_output_data_list();
errorCode
export_run_profile_file();



fprintf(stdout, "[%6s][%s]\n", "INFO", "");

read_lut_file(char *str_lut_file);  
fprintf(stdout, "[%6s][%s][%s]\n", "INFO", "Import Configuration Files...", str_lut_file);

read_input_file(char *str_data_file);
fprintf(stdout, "[%6s][%s][%s]\n", "INFO", "Import Data Files...", str_data_file);

expand_data_list_use_lut();
fprintf(stdout, "[%6s][%s]\n", "INFO", "Preprocess Data Files...");

calculate_output_data_list();
fprintf(stdout, "[%6s][%s]\n", "INFO", "Calculate Speed Profiles...");

export_run_profile_file();
fprintf(stdout, "[%6s][%s]\n", "INFO", "Export Speed Profile Files...");


int main(int argc, char *argv[]);


display_lut_data_list()







data structure

inputData

data->id,
data->sorting_str,
data->input_data_file,
data->str_timestamp, 
data->str_time, 
data->str_location, 
data->str_block, 
data->str_run_number, 
data->str_direction, 
data->str_destination_code, 
data->str_origination_code, 
data->str_schedule_class, 
data->str_cc_id, 
data->str_current_driving_mode, 
data->str_selected_driving_mode, 
data->str_talkative, 
data->str_motion, 
data->str_skip_stop, 
data->str_doors_open, 
data->str_door_fault, 
data->str_alarm, 
data->str_emergency_brake, 
data->str_speed, 
data->str_from_station, 
data->str_to_station, 
data->str_direction_code,
data->timestamp, 
data->cc_id, 
data->direction, 
data->signed_measured_speed_km_h, 
data->str_station_code, 
data->str_platform, 
data->is_platform, 
data->is_motion



lutData

data->id,
data->sorting_str,
data->input_data_file,
data->str_timestamp, 
data->str_time, 
data->str_location, 
data->str_block, 
data->str_run_number, 
data->str_direction, 
data->str_destination_code, 
data->str_origination_code, 
data->str_schedule_class, 
data->str_cc_id, 
data->str_current_driving_mode, 
data->str_selected_driving_mode, 
data->str_talkative, 
data->str_motion, 
data->str_skip_stop, 
data->str_doors_open, 
data->str_door_fault, 
data->str_alarm, 
data->str_emergency_brake, 
data->str_speed, 
data->str_from_station, 
data->str_to_station, 
data->str_direction_code,

data->timestamp, 
data->cc_id, 
data->direction, 
data->signed_measured_speed_km_h, 

data->str_station_code, 
data->str_platform, 

data->is_platform, 
data->is_motion


outputData

data->id, 
data->sorting_str, 
data->input_data_file, 
data->output_data_file, 
data->input_data_id, 
data->str_timestamp, 
data->str_cc_id, 
data->log_time_s, 
data->segment_id, 
data->distance_travelled_0_m, 
data->distance_travelled_1_m, 
data->accum_distance_travelled_ft, 
data->permitted_speed_km_h, 
data->measured_speed_km_h, 
data->current_tag_id, 
data->ti_tag, 
data->signal_name, 
data->signal_name_graphing, 
data->civil_speed_km_h, 
data->travel_time_s, 
data->str_from_station, 
data->str_to_station, 
data->str_direction_code, 
data->timestamp, 
data->cc_id, 
data->str_station_code, 
data->str_platform, 
data->str_direction_num, 
data->run_cnt