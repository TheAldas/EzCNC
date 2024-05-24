/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "command_api.h"
#include "error_codes/error_codes.h"
#include "commands/gcode_commands_api.h"
#include "cmsis_os2.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define COMMAND_QUEUE_MESAGE_NUM 10
#define COMMAND_QUEUE_NAME "CommandsQueue"
#define SINGLE_PARAMETER_BUFFER_SIZE 50
/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
static const osMessageQueueAttr_t g_command_queue_attr = {
		.name = COMMAND_QUEUE_NAME
};

const osThreadAttr_t g_commands_queue_handler_attr= {
  .name = "cmdQueueHandlerTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
static sCommandParseSettings_t g_gcode_commands_settings = {.command_separator = ' ', .commands_table = GCODE_COMMANDS_LUT};
static osMessageQueueId_t g_command_queue_handle = NULL;
static osThreadId_t g_commands_queue_handler_handle = NULL;
/**********************************************************************************************************************
 * Exported variables and references
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of private functions
 *********************************************************************************************************************/
static void Command_API_CommandsQueueHandler(void *args);
static void *Command_API_GetFunction(sCommandParseSettings_t *settings);
static size_t Command_API_GetFunctionNameLength(sCommandParseSettings_t *settings);

static eErrorCode_t Command_API_PlaceParam(char *param_text, sCommandParams_t *parsed_params);
static eErrorCode_t Command_API_ParseParameters(char *text, size_t text_size, sCommandParams_t *parsed_params);
/**********************************************************************************************************************
 * Definitions of private functions
 *********************************************************************************************************************/
static void Command_API_CommandsQueueHandler(void *args) {
	sCommandParams_t command_params;
	eErrorCode_t (*last_command_function)(sCommandParams_t);
	while (1) {
		// to do: add ret status check and msg for it if status isn't right
		osMessageQueueGet(g_command_queue_handle, &command_params, NULL, osWaitForever);
		if (command_params.command_function == NULL) {
			command_params.command_function = last_command_function;
		}
		last_command_function = command_params.command_function;
		// to do: add check for ret error
		command_params.command_function(command_params);
	}
}

static size_t Command_API_GetFunctionNameLength(sCommandParseSettings_t *settings) {
	char *temp = memchr(settings->parsable_text,
				settings->command_separator,
				strnlen(settings->parsable_text, settings->parsable_text_size));
	return (temp == NULL) ?
			strnlen(settings->parsable_text, settings->parsable_text_size)
			: (size_t)(temp - settings->parsable_text);
}

static void *Command_API_GetFunction(sCommandParseSettings_t *settings) {

	size_t command_name_size = Command_API_GetFunctionNameLength(settings);

	for (size_t i = 0; i < settings->commands_table_size; i++) {
		if (settings->commands_table[i].command_name_size == command_name_size) {
			if (memcmp(settings->commands_table[i].command_name,
					settings->parsable_text, command_name_size) == 0) {
				return (settings->commands_table[i].command_function);
			}
		}
	}
	return NULL;
}

static eErrorCode_t Command_API_PlaceParam(char *param_text, sCommandParams_t *parsed_params) {
	switch (*param_text) {
		case 'G':
			g_gcode_commands_settings.commands_table_size = GCODE_COMMANDS_SIZE;
			g_gcode_commands_settings.parsable_text = param_text;
			g_gcode_commands_settings.parsable_text_size = strlen(param_text) + 1;
			parsed_params->extra_function = Command_API_GetFunction(&g_gcode_commands_settings);
			parsed_params->num_params++;
			break;
		case 'x':
		case 'X':
			parsed_params->x = atof((param_text+1));
			parsed_params->params_found.x = 1;
			parsed_params->num_params++;
			break;
		case 'Y':
		case 'y':
			parsed_params->y = atof((param_text+1));
			parsed_params->params_found.y = 1;
			parsed_params->num_params++;
			break;
		case 'z':
		case 'Z':
			parsed_params->z = atof((param_text+1));
			parsed_params->params_found.z = 1;
			parsed_params->num_params++;
			break;
		case 'f':
		case 'F':
			parsed_params->f = atof((param_text+1));
			parsed_params->params_found.f = 1;
			parsed_params->num_params++;
			break;
		case 's':
		case 'S':
			parsed_params->s = atof((param_text+1));
			parsed_params->params_found.s = 1;
			parsed_params->num_params++;
			break;
		default:
			return eErrorCode_InvalidArgument;
			break;
	}
	return eErrorCode_Success;
}

static eErrorCode_t Command_API_ParseParameters(char *text, size_t text_size, sCommandParams_t *parsed_params) {
	// at text size of 1, only null terminator will be present anyway
	if (text_size <= 1) {
		return eErrorCode_Success;
	}
	char *param_start = text;
	text++;
	char param_text[SINGLE_PARAMETER_BUFFER_SIZE] = {0};
	while((text_size--)>0) {
		if(*text == '\0') {
			memcpy(param_text, param_start, (text-param_start));
			param_text[(text-param_start)] = '\0';
			Command_API_PlaceParam(param_text, parsed_params);
			return eErrorCode_Success;
		} else if(isalpha((int)*text)) {
			memcpy(param_text, param_start, (text-param_start));
			param_text[(text-param_start)] = '\0';
			Command_API_PlaceParam(param_text, parsed_params);
			param_start = text;
		}
		text++;
	}
	return eErrorCode_InvalidArgument;
}
/**********************************************************************************************************************
 * Definitions of exported functions
 *********************************************************************************************************************/
eErrorCode_t Command_API_Init(void) {
	g_command_queue_handle = osMessageQueueNew(COMMAND_QUEUE_MESAGE_NUM, sizeof(sCommandParams_t), &g_command_queue_attr);
	if (g_command_queue_handle == NULL) {
		return eErrorCode_UnknownError;
	}

	g_commands_queue_handler_handle = osThreadNew
			(Command_API_CommandsQueueHandler, NULL, &g_commands_queue_handler_attr);
	if (g_commands_queue_handler_handle == NULL) {
		return eErrorCode_UnknownError;
	}
	return eErrorCode_Success;
}

eErrorCode_t Command_API_Parse(sCommandParseSettings_t *settings) {
	if (settings == NULL) {
		return eErrorCode_InvalidArgument;
	}
	if ((settings->parsable_text == NULL) || (settings->parsable_text_size <= 0) ||
		(settings->commands_table == NULL) || (settings->commands_table_size <= 0)) {
		return eErrorCode_InvalidArgument;
	}
	if (settings->parsable_text[settings->parsable_text_size-1] != '\0') {
		return eErrorCode_InvalidArgument;
	}

	sCommandParams_t parsed_params = {0};
	parsed_params.command_function = Command_API_GetFunction(settings);
	if (parsed_params.command_function != NULL) {
		size_t command_name_size = Command_API_GetFunctionNameLength(settings);
		// advance by the command name and a delimiter length
		settings->parsable_text += command_name_size;
		settings->parsable_text_size -= command_name_size;
		if (*settings->parsable_text != '\0') {
			settings->parsable_text++;
			settings->parsable_text_size--;
		}
	}
	if (Command_API_ParseParameters(settings->parsable_text, settings->parsable_text_size, &parsed_params) != eErrorCode_Success) {
		return eErrorCode_UnknownError;
	}

	osMessageQueuePut(g_command_queue_handle, &parsed_params, 0, osWaitForever);

	return eErrorCode_Success;
}



