#ifndef _SCREEN_RECORD_H_
#define _SCREEN_RECORD_H_

#include <string>

void initScreenRecord();
void startScreenRecord(std::string filename, float fps, int release_time /*microsecond*/);
void destroyScreenRecord();

#endif // _SCREEN_RECORD_H_
