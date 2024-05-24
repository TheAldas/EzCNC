#ifndef SOURCE_UTILITY_RING_BUFFER_H_
#define SOURCE_UTILITY_RING_BUFFER_H_
/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/
// In a circular mode, unread data is overwritten if trying to write to a full buffer
// In  a FIFO mode, writing is blocked if trying to write to a full buffer
typedef enum eRingBufferMode {
    eRingBufferMode_First = 0,
    eRingBufferMode_Circular = eRingBufferMode_First,
    eRingBufferMode_FIFO,
    eRingBufferMode_Last
} eRingBufferMode_t;
/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
typedef struct sRingBuffer *sRingBufferHandle_t;
/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
sRingBufferHandle_t RingBuffer_Init (size_t size,  eRingBufferMode_t rb_mode);
bool RingBuffer_DeInit (sRingBufferHandle_t rb);
bool RingBuffer_GetCount (sRingBufferHandle_t rb, size_t *count);
bool RingBuffer_Get (sRingBufferHandle_t rb, uint8_t *data, size_t size);
bool RingBuffer_GetByte (sRingBufferHandle_t rb, uint8_t *byte);
bool RingBuffer_Put (sRingBufferHandle_t rb, uint8_t *data, size_t size);
bool RingBuffer_PutByte (sRingBufferHandle_t rb, uint8_t byte);
#endif /* SOURCE_UTILITY_RING_BUFFER_H_ */
