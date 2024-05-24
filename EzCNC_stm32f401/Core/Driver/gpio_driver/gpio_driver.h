#ifndef DRIVER_GPIO_H_
#define DRIVER_GPIO_H_

/**********************************************************************************************************************
 * Includes
**********************************************************************************************************************/
#include "error_codes/error_codes.h"
/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
typedef enum eGpioDriverPin {
	eGpioDriverPin_First = 0,
	eGpioDriverPin_y_axis_driver_step = eGpioDriverPin_First,
	eGpioDriverPin_y_axis_driver_dir0,
	eGpioDriverPin_y_axis_driver_dir1,
	eGpioDriverPin_x_axis_driver_step,
	eGpioDriverPin_x_axis_driver_dir,
	eGpioDriverPin_z_axis_driver_step,
	eGpioDriverPin_z_axis_driver_dir,
	eGpioDriverPin_stepper_enable,
	eGpioDriverPin_tool_power,
	eGpioDriverPin_status_led,
	eGpioDriverPin_test_led_A15,
	eGpioDriverPin_Last
} eGpioDriverPin_t;

typedef enum eGpioDriverPinState {
	eGpioDriverPinState_First = 0,
	eGpioDriverPinState_Low = eGpioDriverPinState_First,
	eGpioDriverPinState_High,
	eGpioDriverPinState_Last
} eGpioDriverPinState_t;
/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
eErrorCode_t GPIO_Driver_Init (eGpioDriverPin_t pin);
eErrorCode_t GPIO_Driver_TogglePin (eGpioDriverPin_t pin);
eErrorCode_t GPIO_Driver_WritePin (eGpioDriverPin_t pin, eGpioDriverPinState_t pin_state);
eErrorCode_t GPIO_Driver_ReadPin (eGpioDriverPin_t pin, eGpioDriverPinState_t *pin_state);


#endif /* DRIVER_GPIO_H_ */
