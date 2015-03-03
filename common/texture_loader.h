#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H


class TextureLoader
{
public:
	unsigned char *data;
	int width, height, channels;

	int mipmapCount;

	enum CompressedFormat
	{
		cf_none = 0,
		cf_dxt1,
		cf_dxt1a, // DXT1 with binary alpha
		cf_dxt3,
		cf_dxt5,
	};
	CompressedFormat compressedFormat;

	TextureLoader();
	virtual ~TextureLoader();
	virtual bool load(const char *filename) = 0;
	virtual void clear();

	virtual TextureLoader* halve() const;

	static TextureLoader* createFromFile(const char *name);

protected:
};

#endif