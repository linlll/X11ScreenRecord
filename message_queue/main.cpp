#include "message_queue.h"
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

void *pushHandler(void *arg) {
  while (1) {
    uint8_t *data = new uint8_t[1920*1080];
    bzero(data, 1920*1080);
    pushImageQueue(data);
    sleep(1);
  }
}

void *popHandler(void *arg) {
  while (1) {
    uint8_t *data;
    popImageQueue(&data);
    delete [] data;
    sleep(2);
  }
}

int main(int argc, char const *argv[]) {
  initImageQueue(100);
  
  pthread_t push_thread, pop_thread;
  pthread_create(&push_thread, NULL, pushHandler, NULL);
  pthread_create(&push_thread, NULL, popHandler, NULL);

  while (1) {
    sleep(5);
  }

  destroyImageQueue();
  return 0;
}
