/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "ring_buffer/ring_buffer.h"
#include "error_codes/error_codes.h"
#include "command_api/command_api.h"
#include "commands/gcode_commands_api.h"
#include "commands/mcode_commands_api.h"
#include "commands/priority_commands.h"
#include "commands/custom_commands.h"
#include "motion_api/motion_api.h"
#include "cmsis_os2.h"
#include "usbd_cdc_if.h"
#include <stdint.h>
#include <stdbool.h>
#include <text_parser_api/text_parser_api.h>
/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define COMMAND_MESSAGE_OK "ok\r\n"
#define COMMAND_MESSAGE_ERROR "Error command\r\n"

#define PRIORITY_COMMAND_SOFT_RESET 0x18
#define PRIORITY_COMMAND_STATUS_REPORT_QUERY 0x3F // '?'
#define PRIORITY_COMMAND_CYCLE_START_RESUME 0x7E // '~'
#define PRIORITY_COMMAND_FEED_HOLD  0x21 // '!'

#define LINE_TO_PARSE_BUFFER_SIZE 2000U
#define UNPARSED_TEXT_BUFFER_SIZE 30000U
/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/
typedef enum eParserState {
	eParserState_First = 0,
	eParserState_Collect = eParserState_First,

	eParserState_Last
} eParserState_t;
/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
const osThreadAttr_t g_parser_task_attr = {
  .name = "textParserTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

const size_t g_text_buffer_size = UNPARSED_TEXT_BUFFER_SIZE;

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
static sRingBufferHandle_t g_text_buffer = NULL;
static sCommandParseSettings_t g_command_parser_settings = {.command_separator = ' '};
static char line_buffer[LINE_TO_PARSE_BUFFER_SIZE] = { 0 };

static osThreadId_t g_parser_task_handle = NULL;
/**********************************************************************************************************************
 * Exported variables and references
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of private functions
 *********************************************************************************************************************/
static void TextParser_API_Parser(void *args);
static size_t TextParser_API_CollectLine(void);

eErrorCode_t TextParser_API_ChooseCommandsLUT(void);
eErrorCode_t TextParser_API_SelectParser(size_t text_line_size);

void TextParser_API_RemoveCommentsFromBuffer(void);
bool TextParser_API_CheckPriorityCommand(char command_char);
/**********************************************************************************************************************
 * Definitions of private functions
 *********************************************************************************************************************/
static void TextParser_API_Parser(void *args) {
	size_t line_size;
	while(1) {
		line_size = TextParser_API_CollectLine();
		if (line_size <= 1) {
			 continue;
		}
		if (TextParser_API_SelectParser(line_size) == eErrorCode_Success) {
			CDC_Transmit_FS((uint8_t *)COMMAND_MESSAGE_OK, sizeof(COMMAND_MESSAGE_OK)-1);
		} else {
			CDC_Transmit_FS((uint8_t *)COMMAND_MESSAGE_ERROR, sizeof(COMMAND_MESSAGE_ERROR)-1);
		}
	}
}

static size_t TextParser_API_CollectLine(void) {
	size_t size = 0;
	bool found_eol = false;
	while((found_eol == false) && (size < LINE_TO_PARSE_BUFFER_SIZE)) {
		if (RingBuffer_GetByte(g_text_buffer,
				(uint8_t*) &(line_buffer[size])) != true) {
			continue;
		}
		switch (line_buffer[size]) {
		case '\r':
		case ';':
			TextParser_API_RemoveCommentsFromBuffer();
		case '\n':
			found_eol = true;
			break;
		default:
			break;
		}
		size++;
	}
	// switch last received character with a null terminator that is a separator anyway
	line_buffer[size - 1] = '\0';
	return size;
}

eErrorCode_t TextParser_API_ChooseCommandsLUT(void) {
	static char prev_char = '\0';
	if (prev_char == line_buffer[0]) {
		return eErrorCode_Success;
	}
	prev_char = line_buffer[0];
	switch (line_buffer[0]) {
		case 'G':
			g_command_parser_settings.commands_table = GCODE_COMMANDS_LUT;
			g_command_parser_settings.commands_table_size = GCODE_COMMANDS_SIZE;
			g_command_parser_settings.command_separator = GCODE_COMMAND_NAME_SEPARATOR;
			break;
		case 'M':
			g_command_parser_settings.commands_table = MCODE_COMMANDS_LUT;
			g_command_parser_settings.commands_table_size = MCODE_COMMANDS_SIZE;
			g_command_parser_settings.command_separator = MCODE_COMMAND_NAME_SEPARATOR;
			break;
		case '$':
		default:
			g_command_parser_settings.commands_table = CUSTOM_COMMANDS_LUT;
			g_command_parser_settings.commands_table_size = CUSTOM_COMMANDS_SIZE;
			g_command_parser_settings.command_separator = CUSTOM_COMMANDS_NAME_SEPARATOR;
			break;
	}
	return eErrorCode_Success;
}
eErrorCode_t TextParser_API_SelectParser(size_t text_line_size) {
	if (text_line_size == 0) {
		return eErrorCode_NoData;
	}
	if (TextParser_API_ChooseCommandsLUT() != eErrorCode_Success) {
		return eErrorCode_CommandNotFound;
	}

	g_command_parser_settings.parsable_text = line_buffer;
	g_command_parser_settings.parsable_text_size = text_line_size;
	return Command_API_Parse(&g_command_parser_settings);
}

void TextParser_API_RemoveCommentsFromBuffer(void) {
	char temp = 't';
	while (temp != '\n') {
		if (RingBuffer_GetByte(g_text_buffer, (uint8_t*) &temp) == false) {
			return;
		}
	}
}

bool TextParser_API_CheckPriorityCommand(char command_char) {
	switch (command_char) {
		case PRIORITY_COMMAND_SOFT_RESET:
			Priority_Command_SoftReset();
			break;
		case PRIORITY_COMMAND_STATUS_REPORT_QUERY:
			Priority_Command_StatusReportQuery();
			break;
		case PRIORITY_COMMAND_CYCLE_START_RESUME:
			Priority_Command_CycleStartResume();
			break;
		case PRIORITY_COMMAND_FEED_HOLD:
			Priority_Command_FeedHold();
			break;
		default:
			return true;
			break;
	}

	return false;
}

/**********************************************************************************************************************
 * Definitions of exported functions
 *********************************************************************************************************************/
eErrorCode_t TextParser_API_Init(void) {
	g_text_buffer = RingBuffer_Init(g_text_buffer_size, eRingBufferMode_FIFO);
	if (g_text_buffer == NULL) {
		return eErrorCode_NotInitialized;
	}
	g_parser_task_handle = osThreadNew(TextParser_API_Parser, NULL, &g_parser_task_attr);
	if (g_parser_task_handle == NULL) {
		return eErrorCode_UnknownError;
	}
	return eErrorCode_Success;
}


eErrorCode_t TextParser_API_AddText(char *text, size_t text_size) {
	if ((text == NULL) || (g_text_buffer == NULL) || (text_size == 0)) {
		return eErrorCode_NotInitialized;
	}

	for (size_t i = 0; i < text_size; i++) {
		if (TextParser_API_CheckPriorityCommand(text[i]) == true) {
			if (RingBuffer_PutByte(g_text_buffer, (uint8_t)text[i]) == false) {
				return eErrorCode_UnknownError;
			}
		}
	}


	return eErrorCode_Success;
}
