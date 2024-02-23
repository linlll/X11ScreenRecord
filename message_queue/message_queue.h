#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_

#include <stdint.h>

void initImageQueue(int size=100);
void pushImageQueue(uint8_t *src);
void popImageQueue(uint8_t **dst);
void destroyImageQueue();

#endif // _MESSAGE_QUEUE_H_
