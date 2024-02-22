#include "screen_record.h"

int main(int argc, char const *argv[]) {
  initScreenRecord();
  startScreenRecord("output.mp4", 60 /*60fps*/, 60000 /*60000ms=60s*/); // press 'q' to break
  destroyScreenRecord();
  return 0;
}
