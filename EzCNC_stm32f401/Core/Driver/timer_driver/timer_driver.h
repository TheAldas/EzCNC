#ifndef DRIVER_TIMER_H_
#define DRIVER_TIMER_H_

/**********************************************************************************************************************
 * Includes
**********************************************************************************************************************/
#include "error_codes/error_codes.h"
#include <stdint.h>
#include <stdbool.h>
/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
typedef enum eTimerDriverPeriph {
	eTimerDriverPeriph_First = 0,
	eTimerDriverPeriph_StepperTimer = eTimerDriverPeriph_First,
	eTimerDriverPeriph_ToolControlTimer,
	eTimerDriverPeriph_Last
} eTimerDriverPeriph_t;


typedef enum eTimerDriverChannel {
	eTimerDriverChannel_First = 0,
	eTimerDriverChannel_MainToolControl = eTimerDriverChannel_First,
	eTimerDriverChannel_Last
} eTimerDriverChannel_t;

typedef enum eTimerCompareMode {
	eTimerCompareMode_First = 0,
	eTimerCompareMode_Frozen = eTimerCompareMode_First,
	eTimerCompareMode_ActiveOnMatch,
	eTimerCompareMode_InactiveOnMatch,
	eTimerCompareMode_Toggle,
	eTimerCompareMode_ForceInactive,
	eTimerCompareMode_ForceActive,
	eTimerCompareMode_PWM1,
	eTimerCompareMode_PWM2,
	eTimerCompareMode_Last
} eTimerCompareMode_t;


typedef struct sTimerParams {
	eTimerDriverPeriph_t timer;
	uint32_t timer_freq_hz;
	bool timer_enable;
} sTimerParams_t;

typedef struct sTimerChannelParams {
	eTimerDriverChannel_t timer_channel;
	bool channel_enable;
	eTimerCompareMode_t channel_mode;
//	bool output_compare_preload_enable;
	float duty_cycle;
} sTimerChannelParams_t;


/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/
/* init functions are used for initialization and reconfiguration */
eErrorCode_t Timer_Driver_InitTimer(sTimerParams_t timer_config);
eErrorCode_t Timer_Driver_InitChannel(sTimerChannelParams_t channel_config);
/* Restarts timer and waits for the update waits for the update event */
eErrorCode_t Timer_Driver_WaitForTimerCycle(eTimerDriverPeriph_t timer);
/* Set Timer update(cycle) frequency */
eErrorCode_t Timer_Driver_SetFrequencyHz(eTimerDriverPeriph_t timer, uint32_t frequency_hz);
/* Used for setting channel duty cycle */
eErrorCode_t Timer_Driver_SetDutyCycle(eTimerDriverChannel_t channel , float duty_cycle);
/* Used for enabling/disabling timer*/
eErrorCode_t Timer_Driver_EnableTimer(eTimerDriverPeriph_t timer, bool new_status);
/* Used for enabling/disabling timer channel*/
eErrorCode_t Timer_Driver_EnableChannel(eTimerDriverChannel_t channel, bool enable);
eErrorCode_t Timer_Driver_GenerateUpdate(eTimerDriverPeriph_t timer);

#endif /* DRIVER_TIMER_H_ */


