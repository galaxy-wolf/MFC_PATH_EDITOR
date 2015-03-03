#ifndef BYTE_READER_H
#define BYTE_READER_H

#include "types.h"

#include <string.h>

class ByteReader
{
public:

	enum {bufferSize = 4096};
	enum
	{
		seek_set = 0,
		seek_cur,
		seek_end,
	};

	ByteReader()
	{
		clear();
	}

	ByteReader(const unsigned char *_dataCursor, const unsigned char *_dataEnd = 0)
	{
		init(_dataCursor, _dataEnd);
	}

	~ByteReader()
	{
		clear();
	}

	void clear()
	{
		bufferedBytes = 0;
		dataStart = 0;
		dataCursor = 0;
		dataEnd = 0;
		bufferPos = buffer;
		eofFlag = false;
	}

	void init(const unsigned char *_dataCursor, const unsigned char *_dataEnd)
	{
		clear();

		dataStart = _dataCursor;
		dataCursor = dataStart;
		dataEnd = _dataEnd;
	}

	int get(void *dst, int len)
	{
		int p = 0;
		while (p < len)
		{
			if (bufferedBytes <= 0)
				fillBuffer();

			if (eof())
				break;

			int bytesToRead = len - p;
			if (bytesToRead > bufferedBytes)
				bytesToRead = bufferedBytes;

			memcpy((unsigned char*)dst + p, bufferPos, bytesToRead);
			bufferPos += bytesToRead;
			bufferedBytes -= bytesToRead;
			p += bytesToRead;
		}
		return p;
	}

	bool seek(int offset, int base)
	{
		if (base == seek_set)
		{
		}
		else if (base == seek_cur)
		{
			offset += (int)(intptr_t)((dataCursor - bufferedBytes) - dataStart);
		}
		else
		{
			offset += (int)(intptr_t)(dataEnd - dataStart);
		}

		const unsigned char *newCursor = dataStart + offset;
		if (newCursor < dataStart || newCursor > dataEnd)
		{
			return false;
		}

		dataCursor = newCursor;
		bufferedBytes = 0;
		bufferPos = buffer;

		return true;
	}

	bool eof() const
	{
		return eofFlag;
	}

	int currentOffset() const
	{
		return int(dataCursor - bufferedBytes - dataStart);
	}

private:

	unsigned char buffer[bufferSize];
	int bufferedBytes;

	const unsigned char *dataStart;
	const unsigned char *dataCursor;
	const unsigned char *dataEnd;
	const unsigned char *bufferPos;

	bool eofFlag;

	void fillBuffer()
	{
		while (bufferedBytes < bufferSize && dataCursor < dataEnd)
		{
			buffer[bufferedBytes++] = *dataCursor++;
		}
		bufferPos = buffer;

		if (bufferedBytes == 0)
			eofFlag = true;
	}
};

#endif
