/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "mcode_commands_api.h"
#include "error_codes/error_codes.h"
#include "tool_driver/tool_driver.h"
#include "command_api/command_api.h"
#include <string.h>
/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported variables and references
 *********************************************************************************************************************/
const sCommandFunction_t MCODE_COMMANDS_LUT[] = {
		{GEN_COMMAND(&MCODE_COMMAND_M4, "M4")},
		{GEN_COMMAND(&MCODE_COMMAND_M5, "M5")}
};
const size_t MCODE_COMMANDS_SIZE = (sizeof(MCODE_COMMANDS_LUT)/sizeof(sCommandFunction_t));
const char MCODE_COMMAND_NAME_SEPARATOR = ' ';
/**********************************************************************************************************************
 * Prototypes of private functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Definitions of private functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Definitions of exported functions
 *********************************************************************************************************************/
eErrorCode_t MCODE_COMMAND_M4 (sCommandParams_t command_params) {
	if (command_params.s < 0) {
		return eErrorCode_InvalidArgument;
	}
	Tool_Driver_SetDutyCycle((command_params.s/10));
	Tool_Driver_EnablePower(true);

	return eErrorCode_Success;
}


eErrorCode_t MCODE_COMMAND_M5 (sCommandParams_t command_params) {
	Tool_Driver_EnablePower(false);
	return eErrorCode_Success;
}

