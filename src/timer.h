#pragma once
namespace NWin {
typedef unsigned long long timeMl;
class Timer {
private:
	timeMl _beg		= 0;
	timeMl _current = 0;
public:
	void   initialize();
	timeMl getFrameBegTime(); //Get time recorded at beginning of the frame
	timeMl getTime();
	void   update();
};
};