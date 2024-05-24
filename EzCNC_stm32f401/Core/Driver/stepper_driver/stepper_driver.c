/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "stepper_driver.h"
#include "timer_driver/timer_driver.h"
#include "gpio_driver/gpio_driver.h"
#include "error_codes/error_codes.h"
#include <stdint.h>
/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define DEFAULT_SPEED_STEPS_PER_SECOND 10000u
// Wait cycles in stepper step wait()
#define WAIT_CYCLES_PER_STEP 2u
#define MIN_STEPS_PER_MIN 60u
#define CUSTOM_ABS(x) ((x) < 0 ? -(x) : (x))
/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
static uint32_t g_steps_per_second = DEFAULT_SPEED_STEPS_PER_SECOND;
/**********************************************************************************************************************
 * Exported variables and references
 *********************************************************************************************************************/
static const sTimerParams_t g_timer_init_config = {
		.timer = eTimerDriverPeriph_StepperTimer,
		.timer_enable = false,
		.timer_freq_hz = DEFAULT_SPEED_STEPS_PER_SECOND * WAIT_CYCLES_PER_STEP };

/**********************************************************************************************************************
 * Prototypes of private functions
 *********************************************************************************************************************/
void move_stepper_x(int64_t direction);
void move_stepper_y(int64_t direction);
static void stepper_step_wait(void);
/**********************************************************************************************************************
 * Definitions of private functions
 *********************************************************************************************************************/
void move_stepper_y(int64_t direction) {
	if (direction == 0) {
		return;
	}
	if (direction >= 1) {
		GPIO_Driver_WritePin(eGpioDriverPin_y_axis_driver_dir0, eGpioDriverPinState_High);
		GPIO_Driver_WritePin(eGpioDriverPin_y_axis_driver_dir1, eGpioDriverPinState_Low);
	} else {
		GPIO_Driver_WritePin(eGpioDriverPin_y_axis_driver_dir0, eGpioDriverPinState_Low);
		GPIO_Driver_WritePin(eGpioDriverPin_y_axis_driver_dir1, eGpioDriverPinState_High);
	}
	GPIO_Driver_WritePin(eGpioDriverPin_y_axis_driver_step, eGpioDriverPinState_High);
}

void move_stepper_x(int64_t direction) {
	if (direction == 0) {
		return;
	}
	if (direction >= 1) {
		GPIO_Driver_WritePin(eGpioDriverPin_x_axis_driver_dir, eGpioDriverPinState_High);
	} else {
		GPIO_Driver_WritePin(eGpioDriverPin_x_axis_driver_dir, eGpioDriverPinState_Low);
	}
	GPIO_Driver_WritePin(eGpioDriverPin_x_axis_driver_step, eGpioDriverPinState_High);
}

static void stepper_step_wait(void) {
	Timer_Driver_WaitForTimerCycle(eTimerDriverPeriph_StepperTimer);
	GPIO_Driver_WritePin(eGpioDriverPin_x_axis_driver_step, eGpioDriverPinState_Low);
	GPIO_Driver_WritePin(eGpioDriverPin_y_axis_driver_step, eGpioDriverPinState_Low);
	Timer_Driver_WaitForTimerCycle(eTimerDriverPeriph_StepperTimer);
}

/**********************************************************************************************************************
 * Definitions of exported functions
 *********************************************************************************************************************/
eErrorCode_t Stepper_Driver_Init(void) {
	Timer_Driver_InitTimer(g_timer_init_config);
	GPIO_Driver_Init(eGpioDriverPin_y_axis_driver_dir0);
	GPIO_Driver_Init(eGpioDriverPin_y_axis_driver_dir1);
	GPIO_Driver_Init(eGpioDriverPin_y_axis_driver_step);
	GPIO_Driver_Init(eGpioDriverPin_x_axis_driver_dir);
	GPIO_Driver_Init(eGpioDriverPin_x_axis_driver_step);
	GPIO_Driver_Init(eGpioDriverPin_z_axis_driver_step);
	GPIO_Driver_Init(eGpioDriverPin_z_axis_driver_dir);
	GPIO_Driver_Init(eGpioDriverPin_stepper_enable);

	Stepper_Driver_Enable();
	return eErrorCode_Success;
}

eErrorCode_t Stepper_Driver_Disable(void) {
	GPIO_Driver_WritePin(eGpioDriverPin_stepper_enable, eGpioDriverPinState_Low);
	return eErrorCode_Success;
}

eErrorCode_t Stepper_Driver_Enable(void) {
	GPIO_Driver_WritePin(eGpioDriverPin_stepper_enable, eGpioDriverPinState_High);
	return eErrorCode_Success;

}

eErrorCode_t Stepper_Driver_SetStepsPerMinute(uint64_t steps_per_min) {
	if (steps_per_min < MIN_STEPS_PER_MIN) {
		return eErrorCode_InvalidArgument;
	}

	// Divide steps per min by 60 to get steps per second
	steps_per_min = steps_per_min/60u;

	g_steps_per_second = steps_per_min;
	if (Timer_Driver_SetFrequencyHz(g_timer_init_config.timer,
			g_steps_per_second * WAIT_CYCLES_PER_STEP)) {
		return eErrorCode_UnknownError;
	}

	return eErrorCode_Success;

}


eErrorCode_t Stepper_Driver_MoveLinear(int64_t x_steps, int64_t y_steps) {
	int64_t dx = CUSTOM_ABS(x_steps);
	int64_t dy = -CUSTOM_ABS(y_steps);
	int64_t sx = (0 < x_steps) ? 1 : -1;
	int64_t sy = (0 < y_steps) ? 1 : -1;
	int64_t err = dx + dy;

	int64_t current_x = 0;
	int64_t current_y = 0;


	while(true) {
		if (current_x == x_steps && current_y == y_steps) {
			break;
		}
		int64_t e2 = 2 * err;
		if (e2 >= dy) {
			if (current_x != x_steps) {
				err += dy;
				current_x += sx;
				move_stepper_x(sx);
			}
		}
		if (e2 <= dx) {
			if (current_y != y_steps) {
				err += dx;
				current_y += sy;
				move_stepper_y(sy);
			}
		}
		stepper_step_wait();
	}
	return eErrorCode_Success;


}
