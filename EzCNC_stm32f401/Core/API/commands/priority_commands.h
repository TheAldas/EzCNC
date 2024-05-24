#ifndef API_PRIORITY_COMMANDS_H_
#define API_PRIORITY_COMMANDS_H_

/**********************************************************************************************************************
 * Includes
**********************************************************************************************************************/
#include "error_codes/error_codes.h"
#include "command_api/command_api.h"
/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
//eErrorCode_t GCODE_COMMAND_G0 (char *command_text, size_t command_text_size);

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/
eErrorCode_t Priority_Command_SoftReset (void);
eErrorCode_t Priority_Command_StatusReportQuery (void);
eErrorCode_t Priority_Command_CycleStartResume (void);
eErrorCode_t Priority_Command_FeedHold (void);


#endif /* API_PRIORITY_COMMANDS_H_ */