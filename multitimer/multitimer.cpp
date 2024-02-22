#include "multitimer.h"
#include <pthread.h>
#include <stdint.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

#define MAX_NUM_TIMER 10
#define TICK_TIME 1000 // 1000us=1ms

typedef struct multiTimer {
  uint8_t timer_id;
  uint32_t count;
  uint32_t intval;
  bool loop;
  timerHandler timer_handler;
  void *timer_handler_params;
  multiTimer *next = nullptr;
} multiTimer;

static uint8_t timer_id_count = 0;
static multiTimer *timerListHead = nullptr; // head, meaningless
static sigset_t old_mask;
static itimerval old_timerval;
static pthread_t t;

static void destroyMultiTimer(uint8_t id) {
  multiTimer *prev = timerListHead;
  while (prev != nullptr && prev->next->timer_id != id)
    prev = prev->next;
  multiTimer *tmp = prev->next;
  prev = prev->next->next;
  free(tmp);
}

static void tickHandler(int sig) {
  if (sig != SIGALRM) return;
  if (timerListHead == nullptr) return;
  multiTimer *p = timerListHead->next;
  while (p != nullptr) {
    if (++(p->count) == p->intval) {
      p->count = 0;
      p->timer_handler(p->timer_handler_params);
      if (!(p->loop))
        destroyMultiTimer(p->timer_id);
    }
    p = p->next;
  }
}

static void *timerThreadWork(void *arg) {
  signal(SIGALRM, tickHandler);
  itimerval t;
  memset(&t, 0, sizeof(itimerval));
  t.it_value.tv_sec = 0;
  t.it_value.tv_usec = TICK_TIME;
  t.it_interval.tv_sec = 0;
  t.it_interval.tv_usec = TICK_TIME;
  setitimer(ITIMER_REAL, &t, &old_timerval);
  while (1) pause();
  return nullptr;
}

void initMultiTimer() {
  timerListHead = static_cast<multiTimer*>(malloc(sizeof(multiTimer)));
  timerListHead->timer_id = 0xff;

  pthread_create(&t, NULL, timerThreadWork, NULL);

  // main thread block the SIGALRM
  sigset_t new_mask;
  sigemptyset(&new_mask);
  sigaddset(&new_mask, SIGALRM);
  pthread_sigmask(SIG_SETMASK, &new_mask, &old_mask);
}

long createMultiTimer(long long intval, timerHandler timer_handler, void* arg,
                      bool loop) {
  multiTimer *prev = timerListHead;
  while (prev->next != nullptr)
    prev = prev->next;
  
  multiTimer *mt = static_cast<multiTimer*>(malloc(sizeof(multiTimer)));
  mt->timer_id = timer_id_count++;
  mt->count = 0;
  mt->intval = intval;
  mt->loop = loop;
  mt->timer_handler = timer_handler;
  mt->timer_handler_params = arg;

  prev->next = mt;
  return mt->timer_id;
}

void destroyMultiTimer() {
  setitimer(ITIMER_REAL, &old_timerval, nullptr);
  usleep(100000); // 100ms
  multiTimer *prev = timerListHead;
  while (prev != nullptr) {
    multiTimer *tmp = prev->next;
    free(prev);
    prev = tmp;
  }

  pthread_sigmask(SIG_SETMASK, &old_mask, nullptr);
}
