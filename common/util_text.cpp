#include "util_text.h"

#include <stdio.h>

namespace util
{

bool IsWhiteSpace(int c)
{
	if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
		return true;
	return false;
}

int GetLine(char *buffer, int maxLen, void *file, bool allowLineContinuations)
{
	bool eatWhiteSpace = true;

	bool lineContinuation = false;
	int p = 0;
	while (p < maxLen - 1)
	{
		int c = getc((FILE*)file);
		if (c == EOF)
		{
			if (p == 0)
				return -1; // end of file and nothing read
			break;
		}

		if (eatWhiteSpace && IsWhiteSpace(c))
			continue;
		eatWhiteSpace = false;

		if (c == '\r' || c == '\n') // some files use only '\r' for line endings
		{
			if (lineContinuation)
			{
				// remove the line continuation character
				// it seems like inserting a space is the best thing to do for compatibility
				buffer[p - 1] = ' ';

				eatWhiteSpace = true;
				continue;
			}
			else
			{
				break;
			}
		}
		if (allowLineContinuations && c == '\\') // line continuation...
		{
			lineContinuation = true;
		}
		else
		{
			lineContinuation = false;
		}

		buffer[p++] = c;
	}
	buffer[p] = 0;
	return p;
}

int GetNonCommentLine(char *buffer, int maxLen, void *file, bool allowLineContinuations)
{
	int rval = 0;
	do
	{
		rval = GetLine(buffer, maxLen, file, allowLineContinuations);
	} while (rval != -1 && buffer[0] == '#');
	return rval;
}

} // namespace util
