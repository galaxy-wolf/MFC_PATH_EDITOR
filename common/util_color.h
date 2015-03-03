#ifndef UTIL_COLOR_H
#define UTIL_COLOR_H

#include <math.h>

namespace util
{

inline float linearToSRGB(float linearValue)
{
	if (linearValue > 1.0f)
	{
        return 1.0;
    }
	else if (linearValue < 0.0f)
	{
        return 0.0;
    }
	else if (linearValue < 0.0031308f)
	{
        return 12.92f * linearValue;
    }
	else
	{
        return 1.055f * pow(linearValue, 0.41666f) - 0.055f;
    }
}

inline float sRGBToLinear(float sRGBValue)
{
	if (sRGBValue <= 0.04045f)
	{
		return sRGBValue * (1.0f / 12.92f);
	}
	else
	{
		return pow((sRGBValue + 0.055f) * (1.0f / 1.055f), 2.4f);
	}
}

} // namespace util

#endif
