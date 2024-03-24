#include "timer.h"
#include <windows.h>
namespace NWin {

void Timer::initialize() {
	_beg = GetTickCount64();
}

//Get time recorded at beginning of the frame
timeMl Timer::getFrameBegTime() {
	return _current;
}

timeMl Timer::getTime() {
	return GetTickCount64() - _beg;
}

void   Timer::update() {
	_current = GetTickCount64() - _beg;
}
};