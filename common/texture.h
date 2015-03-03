#ifndef TEXTURE_H
#define TEXTURE_H

#include "platform.h"

// temporarily disabled, until the texture update thread is modified to shutdown properly
#if defined(OS_WINDOWS)
# define TEXTURE_RELOAD
#endif

#ifdef TEXTURE_RELOAD
# include "critsect.h"
#endif

class TextureLoader;

class Texture
{
	friend class TextureManager;
public:

	unsigned int handle;
	unsigned int width, height;
	enum {maxName = 128};
	char name[maxName];
	bool mipmap;
	unsigned int internalFormat;

private:
	Texture();
	~Texture();
	void reset();
};

class TextureOptions
{
public:
	TextureOptions();
	TextureOptions(bool _clamp_u, bool _clamp_v, unsigned int _minFilter, unsigned int _magFilter, int _anisotropy = 8)
		: clamp_u(_clamp_u), clamp_v(_clamp_v), minFilter(_minFilter), magFilter(_magFilter),
		anisotropy(_anisotropy)
	{
	}

	bool clamp_u, clamp_v;
	unsigned int minFilter, magFilter;
	int anisotropy;

private:
};

class TextureManager
{
public:

	enum {maxTextures = 512};
	Texture texture[maxTextures];
	unsigned int textures;

	int maxTextureSize;


	TextureManager();
	~TextureManager();

	bool startup();
	void shutdown();

	void update();

	Texture* createTexture(const char *name, TextureOptions options = TextureOptions());
	Texture* createTexture(const TextureLoader *texData, TextureOptions options = TextureOptions());

#ifdef TEXTURE_RELOAD
	int updateFileLoop();
#endif
	void reloadTexture(Texture *t);

	Texture* findByName(const char *name);
	int findBySubString(const char *substring, Texture **textureList, int maxListSize);

	static int estimateSize(int width, int height, int mipmap, unsigned int internalFormat);

private:

	unsigned int freeTexture[maxTextures];
	unsigned int freeTextures;

#ifdef TEXTURE_RELOAD
	void *updateThreadHandle;
	CriticalSection updateCS;
	unsigned int textureUpdateList[maxTextures];
	unsigned int textureUpdateCount;
#endif

	// returns internal format
	unsigned int upload2D(const TextureLoader *texData, bool &hasMipmaps);
};

#endif
