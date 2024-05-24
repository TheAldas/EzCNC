/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "motion_api.h"
#include "error_codes/error_codes.h"
#include "system_config.h"
#include "stepper_driver/stepper_driver.h"
#include <stdint.h>
#include <string.h>
/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define MOTION_API_MAX_FEED_RATE_MM 100000
/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
//const static g_x_axis_steps_per_mm

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
static sPosition3D_t g_machine_position = {0, 0, 0 };
static float g_feed_rate = 1000;
static ePositionStatus_t g_position_status = ePositionStatus_Unknown;
static ePositioningType_t g_positioning_type = ePositioningType_Local;
static ePositioningUnits_t g_positioning_units = ePositioningUnits_mm;
static eMotionState_t g_machine_state = eMotionState_Alarm;


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
eErrorCode_t Motion_API_Init(void) {
	g_position_status = ePositionStatus_Unknown;
	if (Stepper_Driver_Init() != eErrorCode_Success) {
		return eErrorCode_UnknownError;
	}
	return eErrorCode_Success;
}

eErrorCode_t Motion_API_SetPositioningType(ePositioningType_t positioning_type) {
	if (positioning_type >= ePositioningType_Last) {
		return eErrorCode_InvalidArgument;
	}
	g_positioning_type = positioning_type;
	return eErrorCode_Success;
}

eErrorCode_t Motion_API_SetPositioningUnits(ePositioningUnits_t units) {
	if (units >= ePositioningUnits_Last) {
		return eErrorCode_InvalidArgument;
	}
	g_positioning_units = units;
	return eErrorCode_Success;
}

eErrorCode_t Motion_API_SetHome(void) {
	g_machine_position.pos_x = 0;
	g_machine_position.pos_y = 0;
	g_machine_position.pos_z  = 0;
	g_position_status = ePositionStatus_Known;
	g_machine_state = eMotionState_Idle;
	return eErrorCode_Success;
}

sMotionData_t Motion_API_GetPositionData(void) {
	return (sMotionData_t){.position_status = g_position_status,
		.position = g_machine_position};
}

eMotionState_t Motion_API_GetState(void) {
	return g_machine_state;
}

eMotionState_t Motion_API_SetState(eMotionState_t new_state) {
	if (new_state >= eMotionState_Last) {
		return eErrorCode_InvalidArgument;
	}
	g_machine_state = new_state;
	return eErrorCode_Success;
}

eErrorCode_t Motion_API_SetFeedRate(float feed_rate) {
	if ((feed_rate > MOTION_API_MAX_FEED_RATE_MM) || (feed_rate <= 0)) {
		return eErrorCode_InvalidArgument;
	}
	g_feed_rate = feed_rate;
	Stepper_Driver_SetStepsPerMinute(feed_rate * CONFIG_X_AXIS_STEPS_PER_MM);
	return eErrorCode_Success;
}

eErrorCode_t Motion_API_MoveLinear(sPosition2D_t destination_pos) {
	if ((g_position_status == ePositionStatus_Unknown)
			&& (g_positioning_type == ePositioningType_Global)) {
		return eErrorCode_NotInitialized;
	}
	if (g_positioning_units == ePositioningUnits_inch) {
		destination_pos.pos_x  *= 2.54;
		destination_pos.pos_y  *= 2.54;
	}

	if (g_positioning_type == ePositioningType_Global) {
		if (destination_pos.go_flags.go_pos_x == 1u) {
			destination_pos.pos_x = destination_pos.pos_x - g_machine_position.pos_x;
		}
		if (destination_pos.go_flags.go_pos_y == 1u) {
			destination_pos.pos_y = destination_pos.pos_y - g_machine_position.pos_y;
		}
	}
	if (Stepper_Driver_MoveLinear(
			(int64_t) (destination_pos.pos_x * CONFIG_X_AXIS_STEPS_PER_MM),
			(int64_t) (destination_pos.pos_y * CONFIG_Y_AXIS_STEPS_PER_MM)) != eErrorCode_Success) {
		g_position_status = ePositionStatus_Unknown;

		return eErrorCode_UnknownError;
	}
	g_machine_position.pos_x += destination_pos.pos_x;
	g_machine_position.pos_y += destination_pos.pos_y;
	return eErrorCode_Success;
}

