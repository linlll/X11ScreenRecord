#include <opencv2/opencv.hpp>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>

int main(int argc, char const *argv[]) {
  Display *display = XOpenDisplay(NULL);
  Window root = XDefaultRootWindow(display);
  XWindowAttributes attr;
  XGetWindowAttributes(display, root, &attr);
  int x = attr.x, y = attr.y, h = attr.height, w = attr.width;
  XImage *image = XGetImage(display, root, attr.x, attr.y, attr.width, attr.height, AllPlanes, ZPixmap);
  cv::Mat img(h, w, CV_8UC3);

  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      uint64_t pixel = XGetPixel(image, j, i);
      uint8_t blue = pixel & image->blue_mask;
      uint8_t green = (pixel & image->green_mask) >> 8;
      uint8_t red = (pixel & image->red_mask) >> 16;
      img.at<cv::Vec3b>(i, j) = cv::Vec3b(blue, green, red);
    }
  }
  cv::imwrite("img.jpg", img);

  XDestroyImage(image);
  return 0;
}
