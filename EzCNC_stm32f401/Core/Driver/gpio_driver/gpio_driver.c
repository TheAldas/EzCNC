/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "gpio_driver.h"
#include "error_codes/error_codes.h"
#include "stm32f4xx.h"
/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/
typedef enum eGpioMode {
	eGpioMode_First = 0,
	eGpioMode_Input = eGpioMode_First,
	eGpioMode_Output,
	eGpioMode_Alternate,
	eGpioMode_Analog,
	eGpioMode_Last
} eGpioMode_t;

typedef enum eGpioPin {
	eGpioPin_First = 0,
	eGpioPin_0 = eGpioPin_First,
	eGpioPin_1,
	eGpioPin_2,
	eGpioPin_3,
	eGpioPin_4,
	eGpioPin_5,
	eGpioPin_6,
	eGpioPin_7,
	eGpioPin_8,
	eGpioPin_9,
	eGpioPin_10,
	eGpioPin_11,
	eGpioPin_12,
	eGpioPin_13,
	eGpioPin_14,
	eGpioPin_15,
	eGpioPin_Last
} eGpioPin_t;

typedef enum eGpioAF {
	eGpioAF_First = 0,
	eGpioAF_0 = eGpioAF_First,
	eGpioAF_1,
	eGpioAF_2,
	eGpioAF_3,
	eGpioAF_4,
	eGpioAF_5,
	eGpioAF_6,
	eGpioAF_7,
	eGpioAF_8,
	eGpioAF_9,
	eGpioAF_10,
	eGpioAF_11,
	eGpioAF_12,
	eGpioAF_13,
	eGpioAF_14,
	eGpioAF_15,
	eGpioAF_Last
} eGpioAF_t;

typedef enum eGpioType {
	eGpioType_First = 0,
	eGpioType_PushPull = eGpioType_First,
	eGpioType_OpenDrain,
	eGpioType_Last
} eGpioType_t;

typedef enum eGpioPull {
	eGpioPull_First = 0,
	eGpioPull_None = eGpioPull_First,
	eGpioPull_PullUp,
	eGpioPull_PullDown,
	eGpioPull_Last
} eGpioPull_t;

typedef enum eGpioSpeed {
	eGpioSpeed_First = 0,
	eGpioSpeed_Low = eGpioSpeed_First,
	eGpioSpeed_Medium,
	eGpioSpeed_High,
	eGpioSpeed_VeryHigh,
	eGpioSpeed_Last
} eGpioSpeed_t;

typedef struct sGpioDesc {
	GPIO_TypeDef *gpio_port;
	uint8_t port_ahb1enr_pos;
	eGpioPin_t gpio_pin;
	eGpioMode_t gpio_mode;
	eGpioType_t gpio_type;
	eGpioPull_t gpio_pull;
	eGpioSpeed_t gpio_speed;
	eGpioAF_t alternate_function;
} sGpioDesc_t;

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
/*
 *
 *  = 0,
	 = eGpioDriverPin_First,
	,
	,
	,
	,
	eGpioDriverPin_z_axis_driver_step,
	eGpioDriverPin_z_axis_driver_dir,
	,
	,
	eGpioDriverPin_status_led,
	eGpioDriverPin_test_led_A15,
	eGpioDriverPin_Last
 *
 */

static const sGpioDesc_t g_gpio_desc_lut[eGpioDriverPin_Last] = {
		[eGpioDriverPin_y_axis_driver_step] =
			{ .gpio_port = GPIOB, .port_ahb1enr_pos = 1U, .gpio_pin = eGpioPin_12,
				.gpio_mode = eGpioMode_Output, .gpio_type = eGpioType_PushPull,
				.gpio_pull = eGpioPull_None, .gpio_speed = eGpioSpeed_VeryHigh },
		[eGpioDriverPin_y_axis_driver_dir0] =
			{ .gpio_port = GPIOB, .port_ahb1enr_pos = 1U, .gpio_pin = eGpioPin_13,
				.gpio_mode = eGpioMode_Output, .gpio_type = eGpioType_PushPull,
				.gpio_pull = eGpioPull_None, .gpio_speed = eGpioSpeed_VeryHigh},
		[eGpioDriverPin_y_axis_driver_dir1] =
			{ .gpio_port = GPIOB, .port_ahb1enr_pos = 1U, .gpio_pin = eGpioPin_14,
				.gpio_mode = eGpioMode_Output, .gpio_type = eGpioType_PushPull,
				.gpio_pull = eGpioPull_None, .gpio_speed = eGpioSpeed_VeryHigh},
		[eGpioDriverPin_x_axis_driver_step] =
			{ .gpio_port = GPIOB, .port_ahb1enr_pos = 1U, .gpio_pin = eGpioPin_15,
			.gpio_mode = eGpioMode_Output, .gpio_type = eGpioType_PushPull,
			.gpio_pull = eGpioPull_None, .gpio_speed = eGpioSpeed_VeryHigh},
		[eGpioDriverPin_x_axis_driver_dir] =
			{ .gpio_port = GPIOA, .port_ahb1enr_pos = 0U, .gpio_pin = eGpioPin_8,
			.gpio_mode = eGpioMode_Output, .gpio_type = eGpioType_PushPull,
			.gpio_pull = eGpioPull_None, .gpio_speed = eGpioSpeed_VeryHigh},
		[eGpioDriverPin_stepper_enable] =
			{ .gpio_port = GPIOA, .port_ahb1enr_pos = 0U, .gpio_pin = eGpioPin_9,
			.gpio_mode = eGpioMode_Output, .gpio_type = eGpioType_PushPull,
			.gpio_pull = eGpioPull_None, .gpio_speed = eGpioSpeed_VeryHigh},
		[eGpioDriverPin_z_axis_driver_step] =
			{ .gpio_port = GPIOA, .port_ahb1enr_pos = 0U, .gpio_pin = eGpioPin_10,
			.gpio_mode = eGpioMode_Output, .gpio_type = eGpioType_PushPull,
			.gpio_pull = eGpioPull_None, .gpio_speed = eGpioSpeed_VeryHigh},
		[eGpioDriverPin_z_axis_driver_dir] =
			{ .gpio_port = GPIOA, .port_ahb1enr_pos = 0U, .gpio_pin = eGpioPin_11,
			.gpio_mode = eGpioMode_Output, .gpio_type = eGpioType_PushPull,
			.gpio_pull = eGpioPull_None, .gpio_speed = eGpioSpeed_VeryHigh},
		[eGpioDriverPin_tool_power] =
			{ .gpio_port = GPIOB, .port_ahb1enr_pos = RCC_AHB1ENR_GPIOBEN_Pos, .gpio_pin = eGpioPin_4,
			.gpio_mode = eGpioMode_Alternate, .gpio_type = eGpioType_PushPull, .gpio_pull = eGpioPull_None,
			.gpio_speed = eGpioSpeed_High, .alternate_function = eGpioAF_2},
		[eGpioDriverPin_status_led] =
			{ .gpio_port = GPIOC, .port_ahb1enr_pos = 2U, .gpio_pin =
				eGpioPin_13, .gpio_mode = eGpioMode_Output, .gpio_type =
				eGpioType_PushPull, .gpio_pull = eGpioPull_None, .gpio_speed = eGpioSpeed_Medium },
		[eGpioDriverPin_test_led_A15] = { .gpio_port = GPIOA, .port_ahb1enr_pos = 0,
				.gpio_pin = eGpioPin_15, .gpio_mode = eGpioMode_Alternate,
				.gpio_type = eGpioType_PushPull, .gpio_pull = eGpioPull_None,
				.gpio_speed = eGpioSpeed_Medium, .alternate_function = eGpioAF_1} };


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
void GPIO_Driver_SetMode (eGpioDriverPin_t pin) {
	g_gpio_desc_lut[pin].gpio_port->MODER &= ~((uint32_t)0b11 << (g_gpio_desc_lut[pin].gpio_pin << 1));
	g_gpio_desc_lut[pin].gpio_port->MODER |= (uint32_t)g_gpio_desc_lut[pin].gpio_mode << (g_gpio_desc_lut[pin].gpio_pin << 1);
}

void GPIO_Driver_SetSpeed (eGpioDriverPin_t pin) {
	g_gpio_desc_lut[pin].gpio_port->OSPEEDR &= ~((uint32_t)0b11 << (g_gpio_desc_lut[pin].gpio_pin << 1));
	g_gpio_desc_lut[pin].gpio_port->OSPEEDR |= (uint32_t)g_gpio_desc_lut[pin].gpio_speed << (g_gpio_desc_lut[pin].gpio_pin << 1);
}

void GPIO_Driver_SetPull (eGpioDriverPin_t pin) {
	g_gpio_desc_lut[pin].gpio_port->PUPDR &= ~((uint32_t)0b11 << (g_gpio_desc_lut[pin].gpio_pin << 1));
	g_gpio_desc_lut[pin].gpio_port->PUPDR |= (uint32_t)g_gpio_desc_lut[pin].gpio_pull << (g_gpio_desc_lut[pin].gpio_pin << 1);
}

void GPIO_Driver_SetType (eGpioDriverPin_t pin) {
	g_gpio_desc_lut[pin].gpio_port->OTYPER &= ~((uint32_t)0b1 << g_gpio_desc_lut[pin].gpio_pin);
	g_gpio_desc_lut[pin].gpio_port->OTYPER |= (uint32_t)g_gpio_desc_lut[pin].gpio_type << g_gpio_desc_lut[pin].gpio_pin;
}

void GPIO_Driver_SetAlternateFunction (eGpioDriverPin_t pin) {
	uint8_t afr_index = g_gpio_desc_lut[pin].gpio_pin >> 3;
	uint8_t bitfield_pos = (g_gpio_desc_lut[pin].gpio_pin % eGpioPin_8) << 2;
	g_gpio_desc_lut[pin].gpio_port->AFR[afr_index] &= ~((uint32_t)0b1111 << bitfield_pos);
	g_gpio_desc_lut[pin].gpio_port->AFR[afr_index] |=
			((uint32_t)g_gpio_desc_lut[pin].alternate_function << bitfield_pos);
}


/**********************************************************************************************************************
 * Definitions of exported functions
 *********************************************************************************************************************/
eErrorCode_t GPIO_Driver_Init(eGpioDriverPin_t pin) {
	if (pin >= eGpioDriverPin_Last) {
		return eErrorCode_InvalidArgument;
	}
	RCC->AHB1ENR |= (1u << g_gpio_desc_lut[pin].port_ahb1enr_pos);
	switch (g_gpio_desc_lut[pin].gpio_mode) {
		case eGpioMode_Input:
			GPIO_Driver_SetMode(pin);
			GPIO_Driver_SetPull(pin);
			break;
		case eGpioMode_Output:
			GPIO_Driver_SetMode(pin);
			GPIO_Driver_SetType(pin);
			GPIO_Driver_SetSpeed(pin);
			GPIO_Driver_SetPull(pin);
			break;
		case eGpioMode_Alternate:
			GPIO_Driver_SetMode(pin);
			GPIO_Driver_SetType(pin);
			GPIO_Driver_SetSpeed(pin);
			GPIO_Driver_SetPull(pin);
			GPIO_Driver_SetAlternateFunction(pin);
		case eGpioMode_Analog:
			GPIO_Driver_SetMode(pin);
			g_gpio_desc_lut[pin].gpio_port->PUPDR &= ~((uint32_t)0b11 << (g_gpio_desc_lut[pin].gpio_pin << 1));
			break;
		default:
			break;
	}
	return eErrorCode_Success;
}

eErrorCode_t GPIO_Driver_TogglePin(eGpioDriverPin_t pin) {
	if (pin >= eGpioDriverPin_Last) {
		return eErrorCode_InvalidArgument;
	}
	uint32_t bsrr_shift = (1u << g_gpio_desc_lut[pin].gpio_pin);
	// shift by 16 more if output pin is high
	if ((g_gpio_desc_lut[pin].gpio_port->ODR
			& (1u << g_gpio_desc_lut[pin].gpio_pin)) > 0) {
		bsrr_shift <<= 16;
	}
	g_gpio_desc_lut[pin].gpio_port->BSRR = bsrr_shift;
	return eErrorCode_Success;
}

eErrorCode_t GPIO_Driver_WritePin(eGpioDriverPin_t pin,
		eGpioDriverPinState_t pin_state) {
	if ((pin >= eGpioDriverPin_Last)
			|| (pin_state >= eGpioDriverPinState_Last)) {
		return eErrorCode_InvalidArgument;
	}
	switch (pin_state) {
	case eGpioDriverPinState_Low:
		g_gpio_desc_lut[pin].gpio_port->BSRR = ((1u << 16u) << g_gpio_desc_lut[pin].gpio_pin);
		break;
	case eGpioDriverPinState_High:
		g_gpio_desc_lut[pin].gpio_port->BSRR = (1u << g_gpio_desc_lut[pin].gpio_pin);
		break;
	default:
		return eErrorCode_UnknownError;
		break;
	}
	return eErrorCode_Success;
}

eErrorCode_t GPIO_Driver_ReadPin(eGpioDriverPin_t pin,
		eGpioDriverPinState_t *pin_state) {
	if ((pin >= eGpioDriverPin_Last) || (pin_state == NULL)) {
		return eErrorCode_InvalidArgument;
	}

	*pin_state = (g_gpio_desc_lut[pin].gpio_port->IDR
					& (1u << g_gpio_desc_lut[pin].gpio_pin)) > 0 ?
					eGpioDriverPinState_High : eGpioDriverPinState_Low;
	return eErrorCode_Success;
}
