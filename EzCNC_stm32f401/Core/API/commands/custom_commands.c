/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "custom_commands.h"
#include "error_codes/error_codes.h"
#include "command_api/command_api.h"
#include "gcode_commands_api.h"
#include "motion_api/motion_api.h"
#include "usbd_cdc_if.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define SINGLE_PARAMETER_BUFFER_SIZE 40u
/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/
typedef struct sCustomCommandParams {
	float x, y, z;
	float f;
} sCustomCommandParams_t;
/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
const char firmware_version[] = "\r\nGrbl 1.1 \r\n";


/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
// compiler throws error that GCODE_COMMANDS_SIZE is not constant(when it obviously is) when used for commands_table_size
static sCommandParseSettings_t g_gcode_commands_settings = {.command_separator = ' '};
/**********************************************************************************************************************
 * Exported variables and references
 *********************************************************************************************************************/
const sCommandFunction_t CUSTOM_COMMANDS_LUT[] = {
		{GEN_COMMAND(&Custom_Command_Version, "version")},
		{GEN_COMMAND(&Custom_Command_Jog, "$J")},
		{GEN_COMMAND(&Custom_Command_HomeAll, "$H")}
};

const size_t CUSTOM_COMMANDS_SIZE = (sizeof(CUSTOM_COMMANDS_LUT)/sizeof(sCommandFunction_t));
const char CUSTOM_COMMANDS_NAME_SEPARATOR = '=';
/**********************************************************************************************************************
 * Prototypes of private functions
 *********************************************************************************************************************/
eErrorCode_t Custom_Commands_PlaceParam(char *param_text, sCustomCommandParams_t *parsed_params);
eErrorCode_t Custom_Commands_ParseParameters(char *text, size_t text_size, sCustomCommandParams_t *parsed_params);
/**********************************************************************************************************************
 * Definitions of private functions
 * typedef struct sCommandParseSettings {
	const sCommandFunction_t *commands_table;
	size_t commands_table_size;
	char *parsable_text;
	size_t parsable_text_size;
	char command_separator;
} sCommandParseSettings_t;
 *********************************************************************************************************************/
eErrorCode_t Custom_Commands_PlaceParam(char *param_text, sCustomCommandParams_t *parsed_params) {
	switch (*param_text) {
		case 'G':
			g_gcode_commands_settings.commands_table = GCODE_COMMANDS_LUT;
			g_gcode_commands_settings.commands_table_size = GCODE_COMMANDS_SIZE;
			g_gcode_commands_settings.parsable_text = param_text;
			g_gcode_commands_settings.parsable_text_size = strlen(param_text) + 1;
			Command_API_Parse(&g_gcode_commands_settings);
			break;
		case 'X':
		case 'x':
			parsed_params->x = atof((param_text+1));
			break;
		case 'Y':
		case 'y':
			parsed_params->y = atof((param_text+1));
			break;
		case 'z':
		case 'Z':
			parsed_params->z = atof((param_text+1));
			break;
		case 'f':
		case 'F':
			parsed_params->f = atof((param_text+1));
			break;
		default:
			return eErrorCode_InvalidArgument;
			break;
	}
	return eErrorCode_Success;
}

eErrorCode_t Custom_Commands_ParseParameters(char *text, size_t text_size, sCustomCommandParams_t *parsed_params) {
	parsed_params->x = 0;
	parsed_params->y = 0;
	parsed_params->z = 0;
	char *param_start = text;
	text++;
	char param_text[SINGLE_PARAMETER_BUFFER_SIZE] = {0};
	while((text_size--)>0) {
		if(*text == '\0') {
			memcpy(param_text, param_start, (text-param_start));
			param_text[(text-param_start)] = '\0';
			Custom_Commands_PlaceParam(param_text, parsed_params);
			return eErrorCode_Success;
		} else if(isalpha((int)*text)) {
			memcpy(param_text, param_start, (text-param_start));
			param_text[(text-param_start)] = '\0';
			Custom_Commands_PlaceParam(param_text, parsed_params);
			param_start = text;
		}
		text++;
	}
	return eErrorCode_InvalidArgument;
}
/**********************************************************************************************************************
 * Definitions of exported functions
 *********************************************************************************************************************/
eErrorCode_t Custom_Command_Version (sCommandParams_t command_params){
	CDC_Transmit_FS((uint8_t *)firmware_version, sizeof(firmware_version)-1);
	return eErrorCode_Success;
}

eErrorCode_t Custom_Command_Jog(sCommandParams_t params) {
	// command name is $J=(3 chars) long
	if(params.f > 0) {
		Motion_API_SetFeedRate(params.f);
	}
	Motion_API_MoveLinear((sPosition2D_t){.pos_x = params.x, .pos_y = params.y});
	return eErrorCode_Success;
}

eErrorCode_t Custom_Command_HomeAll(sCommandParams_t command_params) {
	Motion_API_SetHome();
	return eErrorCode_Success;
}
