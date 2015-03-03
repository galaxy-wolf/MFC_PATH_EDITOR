#ifndef RANDOM_H
#define RANDOM_H

#include "types.h"

class Random
{
public:

	Random(uint32_t seed = 4357);

	void setseed(uint32_t seed);

	uint32_t genuint();
	float genfloat();
	float gaussian();

private:

	enum {N = 624, M = 397};	/* Period parameters */
	uint32_t	mt[N];				/* the array for the state vector  */
	int32_t	mti;				/* mti==N+1 means mt[N] is not initialized */

	float	y2;
	bool	gaussianPair;
};

#endif
