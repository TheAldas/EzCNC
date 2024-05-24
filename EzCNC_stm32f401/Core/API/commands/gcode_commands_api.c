/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "gcode_commands_api.h"
#include "command_api/command_api.h"
#include "error_codes/error_codes.h"
#include "motion_api/motion_api.h"
#include "tool_driver/tool_driver.h"
#include <string.h>
#include <ctype.h>
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
const sCommandFunction_t GCODE_COMMANDS_LUT[] = {
		{GEN_COMMAND(&GCODE_COMMAND_G0, "G0")},
		{GEN_COMMAND(&GCODE_COMMAND_G1, "G1")},
		{GEN_COMMAND(&GCODE_COMMAND_G10, "G10")},
		{GEN_COMMAND(&GCODE_COMMAND_G20, "G20")},
		{GEN_COMMAND(&GCODE_COMMAND_G21, "G21")},
		{GEN_COMMAND(&GCODE_COMMAND_G90, "G90")},
		{GEN_COMMAND(&GCODE_COMMAND_G91, "G91")},
};
const size_t GCODE_COMMANDS_SIZE = (sizeof(GCODE_COMMANDS_LUT)/sizeof(sCommandFunction_t));
const char GCODE_COMMAND_NAME_SEPARATOR = ' ';
/**********************************************************************************************************************
 * Prototypes of private functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Definitions of private functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Definitions of exported functions
 *********************************************************************************************************************/
eErrorCode_t GCODE_COMMAND_G0 (sCommandParams_t command_params) {
	if (command_params.f > 0) {
		Motion_API_SetFeedRate(command_params.f);
	}
	Motion_API_MoveLinear((sPosition2D_t){.pos_x = command_params.x,
				.pos_y = command_params.y,
				.go_flags.go_pos_x = command_params.params_found.x,
				.go_flags.go_pos_y = command_params.params_found.y});
	return eErrorCode_Success;
}

eErrorCode_t GCODE_COMMAND_G1 (sCommandParams_t command_params) {
	if (command_params.f > 0) {
		Motion_API_SetFeedRate(command_params.f);
	}
	if (command_params.s > 0) {
		Tool_Driver_SetDutyCycle((command_params.s/10));
	}
	Tool_Driver_EnablePower(true);
	Motion_API_MoveLinear((sPosition2D_t){.pos_x = command_params.x,
			.pos_y = command_params.y,
			.go_flags.go_pos_x = command_params.params_found.x,
			.go_flags.go_pos_y = command_params.params_found.y});
	Tool_Driver_EnablePower(false);
	return eErrorCode_Success;
}


eErrorCode_t GCODE_COMMAND_G20 (sCommandParams_t command_params) {
	Motion_API_SetPositioningUnits(ePositioningUnits_inch);
	return eErrorCode_Success;
}

eErrorCode_t GCODE_COMMAND_G21 (sCommandParams_t command_params) {
	Motion_API_SetPositioningUnits(ePositioningUnits_mm);
	return eErrorCode_Success;
}

eErrorCode_t GCODE_COMMAND_G10 (sCommandParams_t command_params) {
	Motion_API_SetHome();
	return eErrorCode_Success;
}

eErrorCode_t GCODE_COMMAND_G90 (sCommandParams_t command_params) {
	return Motion_API_SetPositioningType(ePositioningType_Global);
}

eErrorCode_t GCODE_COMMAND_G91 (sCommandParams_t command_params) {
	return Motion_API_SetPositioningType(ePositioningType_Local);
}
