// texture loading and updating functions

#include "texture.h"
#include "platform.h"
#include "render_includes.h"
#include "texture_loader.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TEXTURE_RELOAD
# include <windows.h>
DWORD WINAPI TextureUpdateCheckThread(void *lpParameter);
#endif

Texture::Texture()
{
	reset();
}

Texture::~Texture()
{
	reset();
}

void Texture::reset()
{
	if (handle)
		glDeleteTextures(1, &handle);
	handle = 0;

	width = 0;
	height = 0;
	name[0] = 0;
	mipmap = false;
	internalFormat = 0;
}

TextureOptions::TextureOptions()
{
	clamp_u = true;
	clamp_v = true;
	// may not want trilinear on all machines
	minFilter = GL_LINEAR_MIPMAP_LINEAR;
	magFilter = GL_LINEAR;
	anisotropy = 8;
}

TextureManager::TextureManager()
{
	textures = 0;
	freeTextures = 0;

#ifdef TEXTURE_RELOAD
	textureUpdateCount = 0;
	updateThreadHandle = 0;
#endif
}

TextureManager::~TextureManager()
{
	shutdown();
}

void TextureManager::shutdown()
{
	for (unsigned int n = 0; n < textures; n++)
	{
		texture[n].reset();
	}

	textures = 0;
	freeTextures = 0;

#ifdef TEXTURE_RELOAD
	if (updateThreadHandle)
	{
		TerminateThread(updateThreadHandle, 0);
		CloseHandle(updateThreadHandle);
		updateThreadHandle = 0;
	}
	textureUpdateCount = 0;
#endif
}

bool TextureManager::startup()
{
	maxTextureSize = -1;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
	if (maxTextureSize <= 0)
		return false;

	FILE *texturelog = fopen("texturelog.txt", "wt");
	if (texturelog)
	{
		fprintf(texturelog, "Max Texture Size: %d\n\n", maxTextureSize);
		fclose(texturelog);
	}

#ifdef TEXTURE_RELOAD
	DWORD threadid;
	updateThreadHandle = CreateThread(0, 0, TextureUpdateCheckThread, this, 0, &threadid);
	SetThreadPriority(updateThreadHandle, -1);
#endif

	return true;
}

void TextureManager::update()
{
#ifdef TEXTURE_RELOAD
	updateCS.enter();
	for (unsigned int n = 0; n < textureUpdateCount; n++)
	{
		reloadTexture(texture + textureUpdateList[n]);
	}
	textureUpdateCount = 0;
	updateCS.leave();
#endif
}

Texture* TextureManager::findByName(const char *name)
{
	if (!name)
		return 0;

	for (unsigned int n = 0; n < textures; n++)
	{
		if (strcmp(name, texture[n].name) == 0)
		{
			return texture + n;
		}
	}

	return 0;
}

int TextureManager::findBySubString(const char *substring, Texture **textureList, int maxListSize)
{
	if (!substring)
		return 0;
	if (maxListSize <= 0)
		return 0;

	int count = 0;
	for (unsigned int n = 0; n < textures; n++)
	{
		if (strstr(texture[n].name, substring) != 0)
		{
			textureList[count++] = texture + n;

			if (count >= maxListSize)
				break;
		}
	}

	return count;
}

int TextureManager::estimateSize(int width, int height, int mipmap, unsigned int internalFormat)
{
	int size = width * height;
	if (mipmap)
		size = int(size * 1.35f);

	switch (internalFormat)
	{
	case GL_LUMINANCE8:
		size *= 1;
		break;
	case GL_LUMINANCE8_ALPHA8:
		size *= 2;
		break;
	case GL_RGB8:
		size *= 4;
		break;
	case GL_RGBA8:
		size *= 4;
		break;
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		size /= 2;
		break;
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		size /= 2;
		break;
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		size *= 1;
		break;
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		size *= 1;
		break;

	default:
		size *= 0;
		break;
	}

	return size;
}

unsigned int TextureManager::upload2D(const TextureLoader *texData, bool &hasMipmaps)
{
	hasMipmaps = false;

	if (!texData)
	{
		assert(0);
		return 0;
	}

	if (texData->data == 0)
		return 0;

	int internalFormat, format;
	if (texData->channels == 1)
	{
		internalFormat = GL_LUMINANCE8;
		format = GL_LUMINANCE;
	}
	else if (texData->channels == 2)
	{
		internalFormat = GL_LUMINANCE8_ALPHA8;
		format = GL_LUMINANCE_ALPHA;
	}
	else if (texData->channels == 3)
	{
		internalFormat = GL_RGB8;
		format = GL_BGR;
	}
	else if (texData->channels == 4)
	{
		internalFormat = GL_RGBA8;
		format = GL_BGRA;
	}
	else
		return 0;

	bool compressed = false;
	int compressedBlockSize = 0;
	switch (texData->compressedFormat)
	{
	case TextureLoader::cf_none:
		break;

	case TextureLoader::cf_dxt1:
		internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		compressed = true;
		compressedBlockSize = 8;
		break;

	case TextureLoader::cf_dxt1a:
		internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		compressed = true;
		compressedBlockSize = 8;
		break;

	case TextureLoader::cf_dxt3:
		internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		compressed = true;
		compressedBlockSize = 16;
		break;

	case TextureLoader::cf_dxt5:
		internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		compressed = true;
		compressedBlockSize = 16;
		break;
	}

	// fit the texture into the hardware's max texture size if needed
	const TextureLoader *resized = texData;
	while ((resized->width > maxTextureSize || resized->height > maxTextureSize)
		&& resized->width > 1 && resized->height > 1)
	{
		const TextureLoader *halved = resized->halve();
		if (halved)
		{
			if (resized != texData) // don't delete the original copy
				delete resized;
			resized = halved;
		}
		else
		{
			// unable to halve, give up
			break;
		}
	}

	if (compressed)
	{
		int x = resized->width;
		int y = resized->height;
		unsigned char *readPtr = resized->data;
		for (int n = 0; n < resized->mipmapCount; n++)
		{
			int size = ((x + 3) / 4) * ((y + 3) / 4) * compressedBlockSize;

			glCompressedTexImage2D(GL_TEXTURE_2D, n, internalFormat, x, y, 0, size, readPtr);
			readPtr += size;

			x = (x + 1) >> 1;
			y = (y + 1) >> 1;
		}

		if (resized->mipmapCount > 1)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, resized->mipmapCount - 1);
			hasMipmaps = true;
		}
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, resized->width, resized->height, 0, format, GL_UNSIGNED_BYTE, resized->data);
	}

	if (resized != texData) // don't delete the original copy
		delete resized;

	return internalFormat;
}

Texture* TextureManager::createTexture(const char *name, TextureOptions options)
{
	if (!name)
		return createTexture("default");

	assert(strlen(name) < Texture::maxName);

	Texture *existing = findByName(name);
	if (existing)
	{
		return existing;
	}

	TextureLoader *texData = TextureLoader::createFromFile(name);
	if (!texData)
	{
		// failed to load
		return createTexture("default");
	}

	Texture *t = createTexture(texData, options);
	delete texData;

	strncpy(t->name, name, Texture::maxName - 1);
	t->name[Texture::maxName - 1] = 0;

	int size = estimateSize(t->width, t->height, t->mipmap, t->internalFormat);
	FILE *texturelog = fopen("texturelog.txt", "a+");
	if (texturelog)
	{
		fprintf(texturelog, "%.2fmb\t%dx%d\tmipmap=%d\t%s\n", (float)size / (1024 * 1024), t->width, t->height, (int)t->mipmap, t->name);
		fclose(texturelog);
	}

	return t;
}

Texture* TextureManager::createTexture(const TextureLoader *texData, TextureOptions options)
{
	if (!texData)
	{
		assert(0);
		return 0;
	}

	unsigned int id = maxTextures;
	if (freeTextures > 0)
	{
		id = freeTexture[freeTextures - 1];
		freeTextures--;
	}
	else
	{
		id = textures;
		textures++;
	}
	assert(id < maxTextures);

	texture[id].mipmap = options.minFilter == GL_NEAREST_MIPMAP_NEAREST 
			   || options.minFilter == GL_LINEAR_MIPMAP_NEAREST
		       || options.minFilter == GL_NEAREST_MIPMAP_LINEAR
		       || options.minFilter == GL_LINEAR_MIPMAP_LINEAR;

	glGenTextures(1, &texture[id].handle);
	glBindTexture(GL_TEXTURE_2D, texture[id].handle);

	assert(options.anisotropy >= 1);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)options.anisotropy);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options.minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options.magFilter);

	if (options.clamp_u)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	if (options.clamp_v)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	bool hasMipmaps = false;
	texture[id].internalFormat = upload2D(texData, hasMipmaps);

	if (texture[id].mipmap && !hasMipmaps)
	{
		glGenerateMipmapEXT(GL_TEXTURE_2D);
	}

	texture[id].width = texData->width;
	texture[id].height = texData->height;

	return texture + id;
}

void TextureManager::reloadTexture(Texture *t)
{
	if (t->name[0] == 0)
		return;

	TextureLoader *texData = TextureLoader::createFromFile(t->name);
	if (texData)
	{
		glBindTexture(GL_TEXTURE_2D, t->handle);

		bool hasMipmaps = false;
		upload2D(texData, hasMipmaps);
		delete texData;

		if (t->mipmap && !hasMipmaps)
		{
			glGenerateMipmapEXT(GL_TEXTURE_2D);
		}
	}
}

#ifdef TEXTURE_RELOAD
static DWORD WINAPI TextureUpdateCheckThread(void *lpParameter)
{
	return ((TextureManager*)lpParameter)->updateFileLoop();
}

int TextureManager::updateFileLoop()
{
	HANDLE hDir = CreateFile(
		".\\",
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL
	);

	if (hDir == INVALID_HANDLE_VALUE)
		return 0;

	unsigned char *fnbuffer[10000];
	while (1) // could check for a terminate signal...
	{
		FILE_NOTIFY_INFORMATION *fn = (FILE_NOTIFY_INFORMATION *)fnbuffer;
		int bytesreturned = 0;
		if (ReadDirectoryChangesW(hDir, (void *)fnbuffer, 10000, true, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME, (LPDWORD)&bytesreturned, 0, 0))
		{
			updateCS.enter();

			while (1)
			{
				if (fn->Action == FILE_ACTION_ADDED || fn->Action == FILE_ACTION_MODIFIED || fn->Action == FILE_ACTION_RENAMED_NEW_NAME)
				{
					char *name;
					name = new char [fn->FileNameLength / 2 + 1];
					wcstombs(name, fn->FileName, fn->FileNameLength / 2 + 1);

					for (unsigned int n = 0; n < fn->FileNameLength / 2; n++)
					{
						if (name[n] == '\\') // slash fixup
							name[n] = '/';

						if (name[n] == '.') // remove extension
						{
							name[n] = 0;
							break;
						}
					}

					// remove path (needed for the mod path stuff to work with this)
					const char *p = strrchr(name, '/');
					const char *p2 = strrchr(name, '\\');
					if (p2 > p)
						p = p2;
					if (!p)
						p = name;
					else
						p++; // skip the slash

					Texture *existing[maxTextures];
					int existingCount = findBySubString(p, existing, maxTextures);
					for (int e = 0; e < existingCount; e++)
					{
						unsigned int id = (unsigned int)(existing[e] - texture);
						// check for duplicates (seems to create two modified events)
						unsigned int i;
						for (i = 0; i < textureUpdateCount; i++)
						{
							if (textureUpdateList[i] == id)
								break;
						}
						if (i >= textureUpdateCount)
						{
							assert(textureUpdateCount < maxTextures);
							textureUpdateList[textureUpdateCount] = id;
							textureUpdateCount++;
						}
					}

					delete [] name;
				}

				if (fn->NextEntryOffset == 0)
					break;
				fn = (FILE_NOTIFY_INFORMATION *)(((unsigned char *)fn) + fn->NextEntryOffset);
			}

			updateCS.leave();
		}
		else // nothing happened
		{
			Sleep(100);
		}
	}

	CloseHandle(hDir);

	return 1;
}
#endif
