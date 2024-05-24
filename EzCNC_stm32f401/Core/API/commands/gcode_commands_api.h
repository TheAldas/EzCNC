#ifndef API_GCODE_COMMANDS_H_
#define API_GCODE_COMMANDS_H_

/**********************************************************************************************************************
 * Includes
**********************************************************************************************************************/
#include "error_codes/error_codes.h"
#include "command_api/command_api.h"
#include <stddef.h>
/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
eErrorCode_t GCODE_COMMAND_G0 (sCommandParams_t);
eErrorCode_t GCODE_COMMAND_G1 (sCommandParams_t);
eErrorCode_t GCODE_COMMAND_G10 (sCommandParams_t);
eErrorCode_t GCODE_COMMAND_G20 (sCommandParams_t) ;
eErrorCode_t GCODE_COMMAND_G21 (sCommandParams_t);
eErrorCode_t GCODE_COMMAND_G90 (sCommandParams_t);
eErrorCode_t GCODE_COMMAND_G91 (sCommandParams_t);
/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/
extern const sCommandFunction_t GCODE_COMMANDS_LUT[];
extern const size_t GCODE_COMMANDS_SIZE;
extern const char GCODE_COMMAND_NAME_SEPARATOR;
#endif /* API_GCODE_COMMANDS_H_ */
