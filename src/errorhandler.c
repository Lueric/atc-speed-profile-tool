/*------------------------------------------------------
**
** File: errorhandler.c
** Author: Eric Lu
** Created: 2018-10-25
**
** Copyright ©2012 Toronto Transit Commission
** 
**
** Function Description
** -----------------------------------------------------
** 
** provide common error handler functions and variables definitions.
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

#include "errorhandler.h"

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

/*
** Function Prototypes
** -----------------------------------------------------
*/

/*
** Source Code
** -----------------------------------------------------
*/

/** @brief  get error description by the error number
 *  @param  err error code enum as defined in errorCode
 *  @return error description
 */
const char *
get_err_description(int err)
{
  if (err < 0 )
  {
    err = 0 - err;
  }
  if (err > err_no_max)
  {
    err = err_no_max;
  }
  return kErrorDescription[err];
}

/** @brief  make errorCode negative since the errorCode enum is 
 *          always non-negative
 *  @param  err error code enum as defined in errorCode
 *  @return negative of err
 */
int 
throw_err(int err)
{
  if (err < 0)
  {
    return err;
  }
  return (0 - err);
}
