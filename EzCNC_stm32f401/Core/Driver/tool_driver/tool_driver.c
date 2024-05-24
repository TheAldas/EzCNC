/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "tool_driver.h"
#include "gpio_driver/gpio_driver.h"
#include "timer_driver/timer_driver.h"
#include "error_codes/error_codes.h"
#include "stm32f4xx.h"
/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define DEFAULT_TOOL_CONTROL_FREQUENCY_HZ 20000u
#define DEFAULT_TOOL_DUTY_CYCLE 0.f
/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
// @formatter:off
static const eGpioDriverPin_t g_tool_control_pin = eGpioDriverPin_tool_power;
static const eTimerDriverPeriph_t g_tool_control_timer = eTimerDriverPeriph_ToolControlTimer;
static const eTimerDriverChannel_t g_tool_control_timer_channel = eTimerDriverChannel_MainToolControl;

static const sTimerParams_t g_timer_init_config = {
		.timer = eTimerDriverPeriph_ToolControlTimer,
		.timer_enable = false,
		.timer_freq_hz = DEFAULT_TOOL_CONTROL_FREQUENCY_HZ };

static const sTimerChannelParams_t g_timer_channel_init_config = {
		.channel_enable = false,
		.channel_mode = eTimerCompareMode_PWM1,
		.duty_cycle = DEFAULT_TOOL_DUTY_CYCLE,
		.timer_channel = g_tool_control_timer_channel };
// @formatter:on

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
static float g_tool_duty_cycle = DEFAULT_TOOL_DUTY_CYCLE;
static uint32_t g_tool_control_frequency = DEFAULT_TOOL_CONTROL_FREQUENCY_HZ;

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
eErrorCode_t Tool_Driver_Init(void) {
	if (GPIO_Driver_Init(g_tool_control_pin) != eErrorCode_Success) {
		return eErrorCode_UnknownError;
	}

	if (Timer_Driver_InitTimer(g_timer_init_config) != eErrorCode_Success) {
		return eErrorCode_UnknownError;
	}

	if (Timer_Driver_InitChannel(g_timer_channel_init_config) != eErrorCode_Success) {
		return eErrorCode_UnknownError;
	}

	return eErrorCode_Success;
}
eErrorCode_t Tool_Driver_EnablePower(bool enable) {
	Timer_Driver_EnableTimer(g_tool_control_timer, enable);
	Timer_Driver_EnableChannel(g_tool_control_timer_channel, enable);
	return eErrorCode_Success;
}

eErrorCode_t Tool_Driver_SetDutyCycle(float duty_cycle) {
	if (Timer_Driver_SetDutyCycle(g_tool_control_timer_channel, duty_cycle) != eErrorCode_Success) {
		return eErrorCode_UnknownError;
	}
	g_tool_duty_cycle = duty_cycle;

	return eErrorCode_Success;
}

eErrorCode_t Tool_Driver_SetFrequencyHz(uint32_t frequency_hz) {
	if (Timer_Driver_SetFrequencyHz(g_tool_control_timer, frequency_hz) != eErrorCode_Success) {
		return eErrorCode_UnknownError;
	}

	g_tool_control_frequency = frequency_hz;

	return Timer_Driver_SetDutyCycle(g_tool_control_timer_channel, g_tool_duty_cycle);
}
