#ifndef API_COMMAND_H_
#define API_COMMAND_H_

/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "error_codes/error_codes.h"
#include <stddef.h>
#include <stdint.h>
/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/
#define GEN_COMMAND(function_ptr, command_name) (function_ptr), (sizeof((command_name))-1), (command_name)


/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
typedef struct sCommandParams sCommandParams_t;

typedef struct sCommandFunction {
    eErrorCode_t (*command_function)(sCommandParams_t);
    size_t command_name_size;
    char *command_name;
} sCommandFunction_t;

typedef struct sCommandParams {
    eErrorCode_t (*command_function)(sCommandParams_t);
    uint32_t num_params;
    eErrorCode_t (*extra_function)(sCommandParams_t);
    float x, y, z;
    float i, j, k;
    float f, s;
    struct {
        unsigned int x : 1;
        unsigned int y : 1;
        unsigned int z : 1;
        unsigned int i : 1;
        unsigned int j : 1;
        unsigned int k : 1;
        unsigned int f : 1;
        unsigned int s : 1;
    } params_found;
} sCommandParams_t;

typedef struct sCommandParseSettings {
	const sCommandFunction_t *commands_table;
	size_t commands_table_size;
	char *parsable_text;
	size_t parsable_text_size;
	char command_separator;
} sCommandParseSettings_t;


/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
eErrorCode_t Command_API_Init(void);
eErrorCode_t Command_API_Parse(sCommandParseSettings_t *settings);

#endif /* API_COMMAND_H_ */
