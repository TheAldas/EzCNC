/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "priority_commands.h"
#include "error_codes/error_codes.h"
#include "motion_api/motion_api.h"
#include "usbd_cdc_if.h"
#include <string.h>
#include <stdio.h>
/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define STATUS_REPORT_BUFFER_SIZE 200U
/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
const char *g_state_report_text_lut[eMotionState_Last] = {
		[eMotionState_Run] = "Run",
		[eMotionState_Idle] = "Idle",
		[eMotionState_Alarm] = "Alarm",
		[eMotionState_Hold] = "Hold",
		[eMotionState_Home] = "Home",
};
/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported variables and references
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of private functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Definitions of private functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Definitions of exported functions
 *********************************************************************************************************************/
eErrorCode_t Priority_Command_SoftReset (void) {
	return eErrorCode_Success;
}

eErrorCode_t Priority_Command_StatusReportQuery (void) {
	char realtime_status_report[STATUS_REPORT_BUFFER_SIZE];
	sMotionData_t motion_data = Motion_API_GetPositionData();
	eMotionState_t curr_state = Motion_API_GetState();
	size_t text_size = snprintf(realtime_status_report, STATUS_REPORT_BUFFER_SIZE,
			"<%s|WPos:%.2f,%.2f,%.2f|FS:0,0|WCO:0.000,0.000,0.000>\r\n",g_state_report_text_lut[curr_state],
			motion_data.position.pos_x, motion_data.position.pos_y, motion_data.position.pos_z);
	CDC_Transmit_FS((uint8_t *)realtime_status_report, text_size);
	return eErrorCode_Success;
}

eErrorCode_t Priority_Command_CycleStartResume (void) {
	return eErrorCode_UnknownError;
}
eErrorCode_t Priority_Command_FeedHold (void) {
	return eErrorCode_UnknownError;
}
