#ifndef API_MCODE_COMMANDS_H_
#define API_MCODE_COMMANDS_H_

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
eErrorCode_t MCODE_COMMAND_M4 (sCommandParams_t);
eErrorCode_t MCODE_COMMAND_M5 (sCommandParams_t);

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/
extern const sCommandFunction_t MCODE_COMMANDS_LUT[];
extern const size_t MCODE_COMMANDS_SIZE;
extern const char MCODE_COMMAND_NAME_SEPARATOR;
#endif /* API_MCODE_COMMANDS_H_ */