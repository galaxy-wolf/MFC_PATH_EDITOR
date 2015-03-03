
namespace util
{

const char LogTable256[] = 
{
  0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
};
// returns the log base 2, or the position of the highest bit set
int GetBitIndex(unsigned int v)
{
	register unsigned int t, tt; // temporaries

	if ((tt = (v >> 16)))
	{
	  return (t = (v >> 24)) ? 24 + LogTable256[t] : 16 + LogTable256[tt & 0xFF];
	}
	else 
	{
	  return (t = (v >> 8)) ? 8 + LogTable256[t] : LogTable256[v];
	}
}

// PackBits - PSD files
void RLEUnpack(const char *in, char *out, int inlen)
{
	int i = 0;
	int o = 0;
	while (i < inlen)
	{
		if (in[i] < 0)
		{
			for (int n = 0; n < -in[i] + 1; n++)
			{
				out[o] = in[i + 1];
				o++;
			}
			i += 2;
		}
		else
		{
			for (int n = 0; n < in[i] + 1; n++)
			{
				out[o] = in[i + n + 1];
				o++;
			}
			i += in[i] + 2;
		}
	}
}

} // namespace util
