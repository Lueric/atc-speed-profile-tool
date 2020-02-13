/*------------------------------------------------------
**
** File: errorhandler.h
** Author: Eric Lu
** Created: 2018-10-25
**
** Copyright ©2012 Toronto Transit Commission
** 
**
** Function Description
** -----------------------------------------------------
** 
** provide common error handler functions and variables declerations.
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

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H


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

/* collection of possible error codes */
typedef enum errorCode
{
  err_no_error = 0,
  err_insufficient_buffer_size,
  err_file_not_accessible,
  err_file_format_not_valid,
  err_end_of_file_reached,
  err_out_of_range_file_access,
  err_file_already_exist,
  err_null_string,
  err_date_not_valid,
  err_maximum_number_exceeded,
  err_maximum_run_number_exceeded,
  err_list_append_failed,
  err_list_iteration_failed,
  err_list_stop_failed,
  err_list_retrieval_failed,
  err_list_is_empty,
  err_missing_header,
  err_lut_is_empty,
  err_lut_match_not_found,
  err_cc_not_valid,
  err_speed_not_valid,
  err_motion_not_valid,

  /* insert more errors above */
  err_no_max
} errorCode;

/*
** Variables
** -----------------------------------------------------
*/

/* collection of possible error descriptions */
static const char *kErrorDescription[err_no_max+1] = 
{
  "OPERATION_SUCCEEDED",
  "INSUFFICIENT_BUFFER_SIZE",
  "FILE_NOT_ACCESSIBLE",
  "FILE_FORMAT_NOT_VALID",
  "END_OF_FILE_REACHED",
  "OUT_OF_RANGE_FILE_ACCESS", 
  "FILE_ALREADY_EXIST",
  "NULL_STRING",
  "DATE_NOT_VALID",    
  "MAXIMUM_NUMBER_EXCEEDED",
  "MAXIMUM_RUN_NUMBER_EXCEEDED",
  "LIST_APPEND_FAILED",
  "LIST_ITERATION_FAILED",
  "LIST_STOP_FAILED",
  "LIST_RETRIEVAL_FAILED",
  "LIST_IS_EMPTY",
  "MISSING_HEADER",
  "LUT_IS_EMPTY",
  "LUT_MATCH_NOT_FOUND",
  "CC_IS_NOT_VALID",
  "SPEED_IS_NOT_VALID",
  "MOTION_IS_NOT_VALID",

  /* insert more errors above */
  "UNKNOWN_ERROR"    
};

/*
** Function Prototypes
** -----------------------------------------------------
*/

/** @brief  get error description by the error number
 *  @param  err error code enum as defined in errorCode
 *  @return error description
 */
const char *
get_err_description(int err);

/** @brief  make errorCode negative since the errorCode enum is always not
 *  @param  err error code enum as defined in errorCode
 *  @return negative of err
 */
int 
throw_err(int err);


#endif