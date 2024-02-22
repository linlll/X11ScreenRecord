#include <stdio.h>
#include <unistd.h>

#include "multitimer.h"

void timerHandler1(void *arg) { printf("timerHandler1\n"); }

void timerHandler2(void *arg) { printf("timerHandler2\n"); }

void timerHandler3(void *arg) { printf("timerHandler3\n"); }

int main(int argc, char const *argv[]) {
  initMultiTimer();

  createMultiTimer(1000 /*1s*/, timerHandler1, NULL, true);
  createMultiTimer(2000 /*2s*/, timerHandler2, NULL, true);
  createMultiTimer(3000 /*3s*/, timerHandler3, NULL, false);

  while (1) {
    sleep(1);
    printf("---------------------------\n");
  }

  destroyMultiTimer();
  return 0;
}
