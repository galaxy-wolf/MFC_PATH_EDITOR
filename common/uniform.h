#ifndef UNIFORM_H
#define UNIFORM_H
class UniformBlock
{
public:

	UniformBlock(unsigned int size);
	virtual ~UniformBlock();

	virtual const char* name() const = 0;
	virtual const char* glslString() const = 0;

	virtual void update() = 0;

//////
	unsigned int getHandle() const
	{
		return _handle;
	}

protected:

	unsigned int _size;
	unsigned int _handle;
	void *_data;
	void *_dataPrev;

	bool _firstUpload;

	void uploadData();
};

#endif
