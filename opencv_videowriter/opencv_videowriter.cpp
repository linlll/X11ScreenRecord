#include <opencv2/opencv.hpp>
#include <time.h>
#include <fcntl.h>

int main(int argc, char const *argv[]) {
  const int seconds = 10;
  const int fps = 60;

  cv::Mat img = cv::imread("../../beauty.jpg");
  cv::resize(img, img, cv::Size(img.cols/6, img.rows/6));

  cv::VideoWriter writer("./output.mp4", cv::VideoWriter::fourcc('m','p','4','v'), fps, cv::Size(img.cols, img.rows));

  clock_t start, end;
  float average_write_opencv = 0.0, average_write_file = 0.0;
  FILE *f = fopen("./output.bin", "w");
  if (f == nullptr) {
    fprintf(stderr, "error\n");
    exit(-1);
  }

  for (int i = 0; i < fps*seconds; i++) {
    start = clock();
    writer.write(img);
    end = clock();
    average_write_opencv += ((float)(end-start))/CLOCKS_PER_SEC*1000;

    start = clock();
    fwrite(img.data, img.elemSize(), img.total(), f);
    end = clock();
    average_write_file += ((float)(end-start))/CLOCKS_PER_SEC*1000;
  }
  average_write_opencv /= fps*seconds;
  average_write_file /= fps*seconds;

  printf("average_write_opencv=%.3fms, average_write_file=%.3fms\n", average_write_opencv, average_write_file);

  writer.release();
  fclose(f);

  return 0;
}
