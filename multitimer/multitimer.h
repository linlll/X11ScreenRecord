#ifndef _MULTITIMER_H_
#define _MULTITIMER_H_

typedef void (*timerHandler)(void *arg);

void initMultiTimer();
long createMultiTimer(long long intval, timerHandler timer_handler, void *arg, bool loop);
void destroyMultiTimer();

#endif // _MULTITIMER_H_
