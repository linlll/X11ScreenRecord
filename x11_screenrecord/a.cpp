#include <opencv2/opencv.hpp>

#include "screen_record.h"
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "multi_timer.h"

typedef struct ScreenRecord {
  Display *display;
  Window root;
  XWindowAttributes attr;
  cv::VideoWriter video;
} ScreenRecord;

static ScreenRecord screen_record;
static struct termios old_settings;

static void videoReleaseHandler(void *arg) {
  ScreenRecord *sr_ptr = reinterpret_cast<ScreenRecord *>(arg);
  sr_ptr->video.release();
}

static void frameDealHandler(void *arg) { // TODO this function is not lightweight, the running time is about 22ms in my machine, too long
  ScreenRecord *sr_ptr = reinterpret_cast<ScreenRecord *>(arg);
  clock_t start = clock();
  XImage *image =
      XGetImage(sr_ptr->display, sr_ptr->root, sr_ptr->attr.x, sr_ptr->attr.y,
                sr_ptr->attr.width, sr_ptr->attr.height, AllPlanes, ZPixmap);
  cv::Mat frame(sr_ptr->attr.height, sr_ptr->attr.width, CV_8UC3);
  for (int x = 0; x < sr_ptr->attr.width; x++) {
    for (int y = 0; y < sr_ptr->attr.height; y++) {
      uint64_t pixel = XGetPixel(image, x, y);
      uint8_t red = pixel & image->blue_mask;
      uint8_t green = (pixel & image->green_mask) >> 8;
      uint8_t blue = (pixel & image->red_mask) >> 16;
      frame.at<cv::Vec3b>(y, x) = cv::Vec3b(red, green, blue);
    }
  }
  sr_ptr->video.write(frame);
  XDestroyImage(image);
}

void initScreenRecord() {
  screen_record.display = XOpenDisplay(nullptr);
  screen_record.root = DefaultRootWindow(screen_record.display);
  XGetWindowAttributes(screen_record.display, screen_record.root,
                       &(screen_record.attr));
  initMultiTimer();
}

void startScreenRecord(std::string filename, float fps, int release_time) {
  screen_record.video.open(
      filename, cv::VideoWriter::fourcc('m', 'p', '4', 'v'), fps,
      cv::Size(screen_record.attr.width, screen_record.attr.height));
  createMultiTimer(static_cast<long>(1000.0 / fps), frameDealHandler,
                   static_cast<void *>(&screen_record), true);
  createMultiTimer(release_time, videoReleaseHandler,
                   static_cast<void *>(&screen_record), true);

  tcgetattr(STDIN_FILENO, &old_settings);

  struct termios new_settings = old_settings;
  new_settings.c_lflag &= ~ICANON;
  new_settings.c_cc[VMIN] = 0;
  new_settings.c_cc[VTIME] = 1;

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_settings);

  char c;
  while (1) {
    if (read(STDIN_FILENO, &c, sizeof(char)) == -1) {
      perror("Error reading from stdin");
      exit(-1);
    } else if (c == 'q') {
      break;
    }
  }
}

void destroyScreenRecord() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_settings);
  screen_record.video.release();
  XCloseDisplay(screen_record.display);
}
