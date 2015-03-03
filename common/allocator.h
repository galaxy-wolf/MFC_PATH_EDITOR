#ifndef ALLOCATOR_H
#define ALLOCATOR_H

// uses an existing memory buffer
// does not allow freeing allocations
class ScratchAllocator
{
public:
	ScratchAllocator()
		: memory(0)
		, next(0)
		, size(0)
	{
	}

	ScratchAllocator(unsigned char *_memory, unsigned int _size)
	{
		set(_memory, _size);
	}

	void set(unsigned char *_memory, unsigned int _size)
	{
		memory = _memory;
		next = _memory;
		size = _size;
	}

	unsigned char* allocate(unsigned int amount)
	{
		unsigned char *rval = next;

		if (rval + amount > memory + size)
			return 0;

		next += amount;
		return rval;
	}

	void reset()
	{
		next = memory;
	}

	unsigned char *memory;
	unsigned char *next;
	unsigned int size;

private:
};

// uses an existing memory buffer
// allows freeing allocations
// all allocations must be <= chunkSize, and will be rounded up to chunkSize
// could write a layer class on top of this that aggregates several pools of different sizes
class PoolAllocator
{
public:
	PoolAllocator()
		: memory(0)
		, memorySize(0)
		, chunkSize(0)
		, nextFreeChunk(0)
		, allocatedBytes(0)
		, maxAllocatedBytes(0)
	{
	}

	PoolAllocator(unsigned char *_memory, unsigned int _memorySize, unsigned int _chunkSize)
	{
		set(_memory, _memorySize, _chunkSize);
	}

	void set(unsigned char *_memory, unsigned int _memorySize, unsigned int _chunkSize)
	{
		memory = _memory;
		memorySize = _memorySize;

		chunkSize = _chunkSize;
		if (chunkSize < sizeof(unsigned int))
			chunkSize = sizeof(unsigned int); // need enough room to store next free offset

		nextFreeChunk = 0;

		// store a linked list of offsets to the next free chunk inside of unused chunks
		unsigned int chunkCount = memorySize / chunkSize;
		for (unsigned int n = 0; n < chunkCount; n++)
		{
			unsigned int nextFreeIndex = n + 1;
			if (n == chunkCount - 1)
				nextFreeIndex = (unsigned int)-1;
			*(unsigned int*)(memory + n * chunkSize) = nextFreeIndex;
		}

		allocatedBytes = 0;
		maxAllocatedBytes = 0;
	}

	unsigned char* allocate(unsigned int amount)
	{
		if (nextFreeChunk == (unsigned int)-1)
			return 0;
		if (amount > chunkSize)
			return 0;

		unsigned char *rval = memory + nextFreeChunk * chunkSize;

		nextFreeChunk = *(unsigned int*)rval;

		allocatedBytes += chunkSize;
		if (allocatedBytes > maxAllocatedBytes)
			maxAllocatedBytes = allocatedBytes;

		return rval;
	}

	// returns false if the memory doesn't belong to this pool
	bool deallocate(void *ptr)
	{
		if (ptr < memory || ptr >= memory + memorySize)
			return false; // doesn't belong to this pool

		unsigned int chunkIndex = (unsigned int)(((unsigned char*)ptr - memory) / chunkSize);
		unsigned char *chunkPtr = memory + chunkIndex * chunkSize;

		*(unsigned int*)chunkPtr = nextFreeChunk;
		nextFreeChunk = chunkIndex;

		allocatedBytes -= chunkSize;

		return true;
	}

	unsigned char *memory;
	unsigned int memorySize;
	unsigned int chunkSize;
	unsigned int nextFreeChunk;

	unsigned int allocatedBytes;
	unsigned int maxAllocatedBytes;

private:
};

#endif
