#ifndef UTILITY_ERROR_CODES_H_
#define UTILITY_ERROR_CODES_H_
/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/
typedef enum eErrorCode {
    eErrorCode_First = 0,
    eErrorCode_Success = eErrorCode_First,
	eErrorCode_UnknownError,
	eErrorCode_CommandNotFound,
	eErrorCode_InvalidArgument,
	eErrorCode_NotInitialized,
	eErrorCode_NoData,
	eErrorCode_BufferLimitReached,
	eErrorCode_ExecutionCancelled,
    eErrorCode_Last
} eErrorCode_t;
/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/

#endif /* UTILITY_ERROR_CODES_H_ */
