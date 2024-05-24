#ifndef API_MOTION_H_
#define API_MOTION_H_

/**********************************************************************************************************************
 * Includes
**********************************************************************************************************************/
#include "error_codes/error_codes.h"
#include <stdint.h>
/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/
typedef enum eMotionState {
	eMotionState_First = 0,
	eMotionState_Alarm = eMotionState_First,
	eMotionState_Home,
	eMotionState_Hold,
	eMotionState_Idle,
	eMotionState_Run,
	eMotionState_Last
} eMotionState_t;


typedef enum ePositioningType {
	ePositioningType_First = 0,
	ePositioningType_Global = ePositioningType_First,
	ePositioningType_Local,
	ePositioningType_Last
} ePositioningType_t;

typedef enum ePositioningUnits {
	ePositioningUnits_First = 0,
	ePositioningUnits_mm = ePositioningUnits_First,
	ePositioningUnits_inch,
	ePositioningUnits_Last
} ePositioningUnits_t;

typedef enum ePositionStatus {
	ePositionStatus_First = 0,
	ePositionStatus_Known = ePositionStatus_First,
	ePositionStatus_Unknown,
	ePositionStatus_Last
} ePositionStatus_t;

typedef struct sPosition3D {
	float pos_x;
	float pos_y;
	float pos_z;
} sPosition3D_t;

typedef struct sPosition2D {
	float pos_x;
	float pos_y;
	struct {
		unsigned int go_pos_x : 1;
		unsigned int go_pos_y : 1;
	} go_flags;
} sPosition2D_t;

typedef struct sMotionData {
	sPosition3D_t position;
	ePositionStatus_t position_status;
	uint64_t feed_rate;
} sMotionData_t;


/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
eErrorCode_t Motion_API_Init(void);
eErrorCode_t Motion_API_SetPositioningType(ePositioningType_t positioning_type);
eErrorCode_t Motion_API_SetPositioningUnits(ePositioningUnits_t units);
eErrorCode_t Motion_API_SetHome(void);
sMotionData_t Motion_API_GetPositionData(void);
eMotionState_t Motion_API_SetState(eMotionState_t new_state);
eMotionState_t Motion_API_GetState(void);
eErrorCode_t Motion_API_SetFeedRate(float feed_rate);
eErrorCode_t Motion_API_MoveLinear(sPosition2D_t destination_pos);



#endif /* API_MOTION_H_ */
