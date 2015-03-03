#ifndef TEXT_READER_H
#define TEXT_READER_H

#include "fixed_string.h"

class TextReader
{
public:

	enum {bufferSize = 4096};
	typedef FixedString<bufferSize + 1> BufferString;

	TextReader()
	{
		clear();
	}

	TextReader(const unsigned char *_dataSource, const unsigned char *_dataEnd = 0)
	{
		init(_dataSource, _dataEnd);
	}

	~TextReader()
	{
		clear();
	}

	void clear()
	{
		bufferedBytes = 0;
		dataSource = 0;
		dataEnd = 0;
		bufferPos = buffer;
		eofFlag = false;
	}

	void init(const unsigned char *_dataSource, const unsigned char *_dataEnd = 0)
	{
		clear();

		dataSource = (const char*)_dataSource;
		dataEnd = (const char*)_dataEnd;

		if (dataEnd == 0)
		{
			const char *p = dataSource;
			while (*p) p++;
			dataEnd = p;
		}
	}

	int getChar()
	{
		if (bufferedBytes <= 0)
			fillBuffer();

		int rval = -1;
		if (bufferedBytes)
		{
			rval = *bufferPos++;
			bufferedBytes--;
			if (rval == '\r') // remove \r code
				rval = getChar();
		}
		return rval;
	}

	bool isWhiteSpace(int c)
	{
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			return true;
		return false;
	}

	int getLine(char *dst, int maxLen, bool ignoreLeadingWhiteSpace = false)
	{
		int p = 0;
		while (p < maxLen - 1)
		{
			int c = getChar();
			if (c == -1)
				break;
			if (ignoreLeadingWhiteSpace && isWhiteSpace(c))
				continue;
			if (c == '\n')
				break;
			ignoreLeadingWhiteSpace = false;
			dst[p++] = c;
		}
		dst[p] = 0;
		return p;
	}

	template <int N>
	int getLine(FixedString<N> &dst, bool ignoreLeadingWhiteSpace = false)
	{
		dst.clear();

		int p = 0;
		while (p < N - 1)
		{
			int c = getChar();
			if (c == -1)
				break;
			if (ignoreLeadingWhiteSpace && isWhiteSpace(c))
				continue;
			if (c == '\n')
				break;
			ignoreLeadingWhiteSpace = false;
			dst += c;
			p++;
		}
		return p;
	}

	int getAll(char *dst, int maxLen)
	{
		int p = 0;
		while (p < maxLen - 1)
		{
			int c = getChar();
			if (c == -1)
				break;
			dst[p++] = c;
		}
		dst[p] = 0;
		return p;
	}

	template <int N>
	int getAll(FixedString<N> &dst)
	{
		dst.clear();

		int p = 0;
		while (p < N - 1)
		{
			int c = getChar();
			if (c == -1)
				break;
			dst += c;
			p++;
		}
		return p;
	}

	BufferString getBuffer()
	{
		return BufferString(bufferPos, bufferedBytes);
	}

	bool eof() const
	{
		return eofFlag;
	}

private:

	char buffer[bufferSize];
	int bufferedBytes;

	const char *dataSource;
	const char *dataEnd;
	const char *bufferPos;

	bool eofFlag;

	void fillBuffer()
	{
		while (bufferedBytes < bufferSize && dataSource < dataEnd)
		{
			buffer[bufferedBytes++] = *dataSource++;
		}
		bufferPos = buffer;

		if (bufferedBytes == 0)
			eofFlag = true;
	}
};

#endif
