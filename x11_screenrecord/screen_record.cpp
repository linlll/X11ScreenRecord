#include "screen_record.h"
#include <opencv2/opencv.hpp>

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>

#include "multitimer.h"

#define DEBUG_HANDLE_TIME 1

typedef struct ScreenRecordInfo {
  Display *display = nullptr;
  Window root;
  int x, y, w, h;
  XShmSegmentInfo *shm_seg_info = nullptr;
  XImage *shm_image = nullptr;
  cv::VideoWriter vw;
} ScreenRecordInfo;

static ScreenRecordInfo screen_record_info;
static struct termios old_settings;

static void videoReleaseHandler(void *arg) {
  screen_record_info.vw.release();
}

static void frameHandler(void *arg) {
#if DEBUG_HANDLE_TIME
  clock_t start = clock();
#endif

  XShmGetImage(screen_record_info.display, screen_record_info.root, screen_record_info.shm_image, 0, 0, AllPlanes);
  cv::Mat img(screen_record_info.h, screen_record_info.w, CV_8UC4);
  img.data = (uchar*)(screen_record_info.shm_image->data);
  cv::cvtColor(img, img, cv::COLOR_BGRA2BGR);
  screen_record_info.vw.write(img);

#if DEBUG_HANDLE_TIME
  clock_t end = clock();
  printf("%.3fms\n", ((float)(end-start))/CLOCKS_PER_SEC*1000);
#endif
}

void initScreenRecord() {
  screen_record_info.display = XOpenDisplay(NULL);
  screen_record_info.root = XDefaultRootWindow(screen_record_info.display);
  XWindowAttributes attr;
  XGetWindowAttributes(screen_record_info.display, screen_record_info.root, &attr);
  screen_record_info.shm_seg_info = new XShmSegmentInfo();
  screen_record_info.shm_image = XShmCreateImage(
      screen_record_info.display, DefaultVisual(screen_record_info.display, 0),
      attr.depth, ZPixmap, NULL, screen_record_info.shm_seg_info, attr.width,
      attr.height);
  screen_record_info.shm_seg_info->shmid = shmget(IPC_PRIVATE, screen_record_info.shm_image->bytes_per_line*screen_record_info.shm_image->height, IPC_CREAT|0777);
  screen_record_info.shm_seg_info->shmaddr = screen_record_info.shm_image->data = (char*)shmat(screen_record_info.shm_seg_info->shmid, NULL, 0);
  XShmAttach(screen_record_info.display, screen_record_info.shm_seg_info);
  screen_record_info.x = attr.x;
  screen_record_info.y = attr.y;
  screen_record_info.w = attr.width;
  screen_record_info.h = attr.height;
  initMultiTimer();
}

void startScreenRecord(std::string filename, float fps, int release_time) {
  screen_record_info.vw.open(filename, cv::VideoWriter::fourcc('m','p','4','v'), fps, cv::Size(screen_record_info.w, screen_record_info.h));
  createMultiTimer((long long)(1000.0/fps), frameHandler, NULL, true);
  createMultiTimer(release_time, videoReleaseHandler, NULL, true);

  tcgetattr(STDIN_FILENO, &old_settings);
  struct termios new_settings = old_settings;
  new_settings.c_lflag &= ~ICANON;
  new_settings.c_cc[VMIN] = 0;
  new_settings.c_cc[VTIME] = 1;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_settings);

  char c;
  while (1) {
    if (read(STDIN_FILENO, &c, sizeof(char)) == -1) {
      fprintf(stderr, "Error reading from stdin");
      exit(-1);
    } else if (c == 'q') {
      break;
    }
  }
}

void destroyScreenRecord() {
  destroyMultiTimer();
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_settings);
  screen_record_info.vw.release();
  XShmDetach(screen_record_info.display, screen_record_info.shm_seg_info);
  shmdt(screen_record_info.shm_seg_info->shmaddr);
  shmctl(screen_record_info.shm_seg_info->shmid, IPC_RMID, NULL);
  delete screen_record_info.shm_seg_info;
  XCloseDisplay(screen_record_info.display);
}
