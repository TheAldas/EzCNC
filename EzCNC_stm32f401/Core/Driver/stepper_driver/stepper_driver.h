#ifndef DRIVER_STEPPER_H_
#define DRIVER_STEPPER_H_

/**********************************************************************************************************************
 * Includes
**********************************************************************************************************************/
#include "error_codes/error_codes.h"
#include <stdint.h>
/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
typedef struct sStepperDriverLinearMoveParams {
	int64_t steps_x;
	int64_t steps_y;
	int64_t steps_z;
	uint64_t total_time_microseconds;
} sStepperDriverLinearMoveParams;
/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/
eErrorCode_t Stepper_Driver_Init(void);
eErrorCode_t Stepper_Driver_Disable(void);
eErrorCode_t Stepper_Driver_Enable(void);
eErrorCode_t Stepper_Driver_SetStepsPerMinute(uint64_t steps_per_min);
eErrorCode_t Stepper_Driver_MoveLinear(int64_t x_steps, int64_t y_steps);



#endif /* DRIVER_STEPPER_H_ */
