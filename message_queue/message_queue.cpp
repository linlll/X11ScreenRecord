#include "message_queue.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <queue>

typedef struct MessagePool {
  std::queue<uint8_t*> buffer;
  int size;
  bool shutdown;
} MessagePool;

static MessagePool pool;
static pthread_mutex_t pool_lock;
static pthread_cond_t pool_push_cond;
static pthread_cond_t pool_pop_cond;

void initImageQueue(int size) {
  pool.size = size;
  pthread_mutex_init(&pool_lock, NULL);
  pthread_cond_init(&pool_push_cond, NULL);
  pthread_cond_init(&pool_pop_cond, NULL);
}

void pushImageQueue(uint8_t *src) {
  pthread_mutex_lock(&pool_lock);
  while (pool.buffer.size() >= pool.size) {
    pthread_cond_wait(&pool_push_cond, &pool_lock);
  }

  pool.buffer.push(src);

  printf("pushImageQueue: size=%ld\n", pool.buffer.size());
  pthread_cond_signal(&pool_push_cond);
  pthread_mutex_unlock(&pool_lock);
}

void popImageQueue(uint8_t **dst) {
  pthread_mutex_lock(&pool_lock);
  while (pool.buffer.size() == 0) {
    pthread_cond_wait(&pool_push_cond, &pool_lock);
  }

  *dst = pool.buffer.front();
  pool.buffer.pop();

  printf("popImageQueue: size=%ld\n", pool.buffer.size());
  pthread_cond_signal(&pool_push_cond);
  pthread_mutex_unlock(&pool_lock);
}

void destroyImageQueue() {
  while (!pool.buffer.empty()) {
    uint8_t *ptr = pool.buffer.front();
    delete ptr;
    pool.buffer.pop();
  }
  // pthread_mutex_destroy(&image_buffer_lock);
  // pthread_cond_destroy(&image_buffer_push_cond);
  // pthread_cond_destroy(&image_buffer_pop_cond);
}
