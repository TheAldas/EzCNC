/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "ring_buffer.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/
typedef struct sRingBuffer {
    uint8_t *buffer;
    eRingBufferMode_t buffer_mode;
    size_t head;
    size_t tail;
    size_t size;
} sRingBuffer_t;
/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

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

/**********************************************************************************************************************
 * Definitions of exported functions
 *********************************************************************************************************************/
sRingBufferHandle_t RingBuffer_Init (size_t size, eRingBufferMode_t rb_mode) {
    sRingBufferHandle_t rb = (sRingBufferHandle_t) calloc(1, sizeof(sRingBuffer_t));
    if (rb == NULL) {
        return NULL;
    }
    rb->buffer = (typeof(rb->buffer)) calloc(1, size);
    if (rb->buffer == NULL) {
        free(rb);
        return NULL;
    }
    rb->tail = 0;
    rb->head = 0;
    rb->buffer_mode = rb_mode;
    rb->size = size;
    return rb;
}

bool RingBuffer_DeInit (sRingBufferHandle_t rb) {
    if (rb == NULL) {
        return false;
    }
    if (rb->buffer == NULL) {
        return false;
    }
    free(rb->buffer);
    free(rb);
    return true;
}

bool RingBuffer_GetCount (sRingBufferHandle_t rb, size_t *count) {
    if ((rb == NULL) || (count == NULL)) {
        return false;
    }
    *count = (rb->head >= rb->tail) ? (rb->head - rb->tail) : (rb->size-rb->tail + rb->head);
    return true;
}

bool RingBuffer_Get (sRingBufferHandle_t rb, uint8_t *data, size_t size) {
    if (rb == NULL) {
        return false;
    }
    static size_t count = 0;
    RingBuffer_GetCount(rb, &count);
    if ((rb->buffer == NULL) || (size > count)) {
        return false;
    }

    while ((size--) > 0) {
        if (RingBuffer_GetByte(rb, data++) == false) {
            return false;
        }
    }
    return true;
}

bool RingBuffer_GetByte (sRingBufferHandle_t rb, uint8_t *byte) {
    if (rb == NULL) {
        return false;
    }
    static size_t count = 0;
    RingBuffer_GetCount(rb, &count);
    if ((rb->buffer == NULL) || (count == 0)) {
        return false;
    }

    *byte = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->size;
    return true;
}

bool RingBuffer_Put (sRingBufferHandle_t rb, uint8_t *data, size_t size) {
    if (rb == NULL) {
        return false;
    }
    static size_t count = 0;
    RingBuffer_GetCount(rb, &count);
    if ((rb->buffer == NULL) && ((rb->size - count) < size)) {
        return false;
    }

    for (size_t curr_byte = 0; curr_byte < size; curr_byte++) {
        if (RingBuffer_PutByte(rb, data[curr_byte]) == false) {
            return false;
        }
    }
    return true;
}

bool RingBuffer_PutByte (sRingBufferHandle_t rb, uint8_t byte) {
    if (rb == NULL) {
        return false;
    }
    if (rb->buffer == NULL) {
        return false;
    }
    static size_t count = 0;
    RingBuffer_GetCount(rb, &count);
    bool is_buffer_full = (count == rb->size);
    if ((is_buffer_full == true) && (rb->buffer_mode != eRingBufferMode_Circular)) {
        return false;
    }
    rb->buffer[rb->head] = byte;
    rb->head = (rb->head + 1) % rb->size;
    if (is_buffer_full == true) {
        rb->tail = (rb->tail + 1) % rb->size;
    }
    return true;
}
