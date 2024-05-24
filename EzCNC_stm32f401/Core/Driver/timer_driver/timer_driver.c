/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "error_codes/error_codes.h"
#include "timer_driver.h"
#include "stm32f4xx.h"
#include <stdbool.h>
/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define TIMER_ARR_MAX_VALUE 65535
/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/
typedef struct sTimerInitPeriphData {
	TIM_TypeDef *timer;
	volatile uint32_t *timer_rcc_enr;
	uint8_t timer_rcc_enr_pos;
} sTimerPeriphData_t;

typedef struct sTimerChannelData {
	eTimerDriverPeriph_t timer_periph;
	uint8_t channel_number;
} sTimerChannelData_t;
/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
// @formatter:off
static const sTimerPeriphData_t g_tim_conf_lut[eTimerDriverPeriph_Last] =
{
		[eTimerDriverPeriph_StepperTimer] =
			{.timer = TIM2, .timer_rcc_enr = &RCC->APB1ENR, .timer_rcc_enr_pos = RCC_APB1ENR_TIM2EN_Pos},
		[eTimerDriverPeriph_ToolControlTimer] =
			{.timer = TIM3, .timer_rcc_enr = &RCC->APB1ENR, .timer_rcc_enr_pos = RCC_APB1ENR_TIM3EN_Pos}
};

static const sTimerChannelData_t g_tim_channel_conf_lut[eTimerDriverChannel_Last] = {
		[eTimerDriverChannel_MainToolControl] =
			{.timer_periph = eTimerDriverPeriph_ToolControlTimer, .channel_number = 1}
};

// @formatter:on
/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported variables and references
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of private functions
 *********************************************************************************************************************/
static eErrorCode_t Timer_Driver_SetChannelMode(eTimerDriverChannel_t channel, eTimerCompareMode_t channel_mode);
/**********************************************************************************************************************
 * Definitions of private functions
 *********************************************************************************************************************/
//typedef enum eTimerDriverPeriph {
//	eTimerDriverPeriph_First = 0,
//	eTimerDriverPeriph_TIM1 = eTimerDriverPeriph_First,
//	eTimerDriverPeriph_TIM2,
//	eTimerDriverPeriph_TIM3,
//	eTimerDriverPeriph_TIM4,
//	eTimerDriverPeriph_TIM5,
//	eTimerDriverPeriph_Last
//} eTimerDriverPeriph_t;

/* Used for setting channel duty cycle */
static eErrorCode_t Timer_Driver_SetChannelMode(eTimerDriverChannel_t channel, eTimerCompareMode_t channel_mode) {
	if (channel_mode >= eTimerCompareMode_Last) {
		return eErrorCode_InvalidArgument;
	}

	TIM_TypeDef *channel_timer = g_tim_conf_lut[g_tim_channel_conf_lut[channel].timer_periph].timer;

	switch (channel_mode) {
		case eTimerCompareMode_PWM1:
			channel_timer->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
			break;
		default:
			return eErrorCode_InvalidArgument;
			break;
	}

	return eErrorCode_Success;
}

/**********************************************************************************************************************
 * Definitions of exported functions
 *********************************************************************************************************************/


/* init functions are used for initialization and reconfiguration */
eErrorCode_t Timer_Driver_InitTimer(sTimerParams_t timer_config) {
	if (timer_config.timer >= eTimerDriverPeriph_Last) {
		return eErrorCode_InvalidArgument;
	}
	Timer_Driver_EnableTimer(timer_config.timer, false);
	*g_tim_conf_lut[timer_config.timer].timer_rcc_enr |= (1u << g_tim_conf_lut[timer_config.timer].timer_rcc_enr_pos);

	Timer_Driver_SetFrequencyHz(timer_config.timer, timer_config.timer_freq_hz);

	// only call enable function to enable when necessary
	if (timer_config.timer_enable) {
		Timer_Driver_EnableTimer(timer_config.timer, timer_config.timer_enable);
	}
	return eErrorCode_Success;
}

eErrorCode_t Timer_Driver_InitChannel(sTimerChannelParams_t channel_config) {
	if ((channel_config.timer_channel >= eTimerDriverChannel_Last)) {
			return eErrorCode_InvalidArgument;
		}
	Timer_Driver_EnableChannel(channel_config.timer_channel, false);
	Timer_Driver_SetChannelMode(channel_config.timer_channel, channel_config.channel_mode);
	Timer_Driver_SetDutyCycle(channel_config.timer_channel, channel_config.duty_cycle);

	// only call enable function to enable when necessary
	if (channel_config.timer_channel == true) {
		Timer_Driver_EnableChannel(channel_config.timer_channel, true);
	}

	return eErrorCode_Success;


}
/* Restarts timer and waits for the update waits for the update event */
eErrorCode_t Timer_Driver_WaitForTimerCycle(eTimerDriverPeriph_t timer) {

	if (timer >= eTimerDriverPeriph_Last) {
			return eErrorCode_InvalidArgument;
		}
		g_tim_conf_lut[timer].timer->CR1 &= ~TIM_CR1_CEN;
		g_tim_conf_lut[timer].timer->CNT = 0;
		g_tim_conf_lut[timer].timer->SR &= ~TIM_SR_UIF;
		g_tim_conf_lut[timer].timer->CR1 |= TIM_CR1_CEN;
		while (!(g_tim_conf_lut[timer].timer->SR & TIM_SR_UIF));
		g_tim_conf_lut[timer].timer->CR1 &= ~TIM_CR1_CEN;
		return eErrorCode_Success;
}
eErrorCode_t Timer_Driver_SetFrequencyHz(eTimerDriverPeriph_t timer, uint32_t frequency_hz) {
	if ((timer >= eTimerDriverPeriph_Last) ||
			(frequency_hz == 0) ||
			(frequency_hz > SystemCoreClock)) {
			return eErrorCode_InvalidArgument;
		}

	Timer_Driver_EnableTimer(timer, false);

	uint32_t core_cycles_per_cycle = SystemCoreClock/frequency_hz;
	uint32_t psc = core_cycles_per_cycle/TIMER_ARR_MAX_VALUE;
	uint32_t arr = core_cycles_per_cycle/(psc + 1) - 1;

	/* Might add disabling all timer channels or automatically
	 * scale all timer channels ccr values depending on arr change for safety,
	 * for now outside duty cycle reconfiguring and enabling is required */
	g_tim_conf_lut[timer].timer->PSC = psc;
	g_tim_conf_lut[timer].timer->ARR = arr;


	return eErrorCode_Success;
}

/* Used for setting channel duty cycle */
eErrorCode_t Timer_Driver_SetDutyCycle(eTimerDriverChannel_t channel, float duty_cycle) {
	if ((channel >= eTimerDriverChannel_Last) ||
			(duty_cycle > 100) || (duty_cycle < 0)) {
		return eErrorCode_InvalidArgument;
	}

	uint32_t timer_arr = g_tim_conf_lut[g_tim_channel_conf_lut[channel].timer_periph].timer->ARR;
	uint32_t ccr_value;
	if (duty_cycle == 0) {
		ccr_value = 0;
	} else {
		ccr_value = (uint32_t)(timer_arr * (duty_cycle / 100));
	}
	volatile uint32_t *tim_ccr = &(g_tim_conf_lut[g_tim_channel_conf_lut[channel].timer_periph].timer->CCR1);
	tim_ccr[g_tim_channel_conf_lut[channel].channel_number-1] = ccr_value;

	return eErrorCode_Success;
}

/* Used for enabling/disabling timer*/
eErrorCode_t Timer_Driver_EnableTimer(eTimerDriverPeriph_t timer, bool enable) {
	if (timer >= eTimerDriverPeriph_Last) {
		return eErrorCode_InvalidArgument;
	}

	if (enable == false) {
		g_tim_conf_lut[timer].timer->CR1 &= ~TIM_CR1_CEN_Msk;
		return eErrorCode_Success;
	}
	g_tim_conf_lut[timer].timer->CR1 |= TIM_CR1_CEN_Msk;

	return eErrorCode_Success;
}

eErrorCode_t Timer_Driver_GenerateUpdate(eTimerDriverPeriph_t timer) {
	if (timer >= eTimerDriverPeriph_Last) {
		return eErrorCode_InvalidArgument;
	}

	g_tim_conf_lut[timer].timer->EGR |= TIM_EGR_UG_Msk;

	return eErrorCode_Success;
}

/* Used for enabling/disabling timer channel*/
eErrorCode_t Timer_Driver_EnableChannel(eTimerDriverChannel_t channel, bool enable) {
	if (channel >= eTimerDriverChannel_Last) {
		return eErrorCode_InvalidArgument;
	}

	uint32_t ccer_pos = (g_tim_channel_conf_lut[channel].channel_number-1)
						* (TIM_CCER_CC2E_Pos - TIM_CCER_CC1E_Pos);

	if (enable == false) {
		g_tim_conf_lut[g_tim_channel_conf_lut[channel].timer_periph].timer->CCER &= ~(1u << ccer_pos);
		return eErrorCode_Success;
	}

	g_tim_conf_lut[g_tim_channel_conf_lut[channel].timer_periph].timer->CCER |= 1u << ccer_pos;

	return eErrorCode_Success;
}


