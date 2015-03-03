#ifndef TIMER_H
#define TIMER_H

#include "types.h"

class Timer
{
public:
	
	Timer();
	~Timer();
	
	void init();
	double get();
	double getElapsed();
	double frametime();

private:

	uint64_t startTime;
	uint64_t lastTime;
	uint64_t lastFrame;
};

#endif
