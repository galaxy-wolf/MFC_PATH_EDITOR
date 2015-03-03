#ifndef FIXED_STRING_H
#define FIXED_STRING_H

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "util.h" // for min max
#include "types.h"

template <size_t N>
class FixedString
{
public:

	mutable char buffer[N]; // mutable to allow c_str() on const object
	size_t len;

	size_t size() const
	{
		return len;
	}
	size_t length() const
	{
		return len;
	}
	size_t max_size() const
	{
		return N;
	}
	size_t capacity() const
	{
		return N;
	}
	bool empty() const
	{
		return len == 0;
	}

	char* begin()
	{
		return buffer;
	}
	const char* begin() const
	{
		return buffer;
	}
	char* end()
	{
		return buffer + len;
	}
	const char* end() const
	{
		return buffer + len;
	}

	char& operator[](size_t n)
	{
		assert(n < len);
		return buffer[n];
	}
	const char& operator[](size_t n) const
	{
		assert(n < len);
		return buffer[n];
	}

	const char* c_str() const
	{
		buffer[len] = 0;
		return buffer;
	}
	const char* data() const
	{
		return buffer;
	}

	FixedString()
	{
		len = 0;
	}
	template<size_t sN> FixedString(const FixedString<sN> &s)
	{
		len = s.size();
		assert(len < N - 1);
		memcpy(buffer, s.data(), len);
	}
	template<size_t sN> FixedString(const FixedString<sN> &s, size_t pos)
	{
		len = s.size() - pos;
		assert(len < N - 1);
		memcpy(buffer, s.data() + pos, len);
	}
	template<size_t sN> FixedString(const FixedString<sN> &s, size_t pos, size_t n)
	{
		len = util::min(n, s.size() - pos);
		assert(len < N - 1);
		memcpy(buffer, s.data() + pos, n);
	}
	FixedString(const char *s)
	{
		len = strlen(s);
		assert(len < N - 1);
		memcpy(buffer, s, len);
	}
	FixedString(const char *s, size_t n)
	{
		len = n;
		assert(len < N - 1);
		memcpy(buffer, s, n);
	}
	FixedString(size_t n, char c)
	{
		len = n;
		assert(len < N - 1);
		memset(buffer, c, n);
	}
	FixedString(const char *first, const char *last)
	{
		assert(first <= last);
		len = last - first;
		assert(len < N - 1);
		memcpy(buffer, first, len);
	}

	~FixedString()
	{
	}

	template<size_t sN> FixedString& operator=(const FixedString<sN> &s)
	{
		len = s.length();
		assert(len < N - 1);
		memcpy(buffer, s.data(), len);
		return *this;
	}
	FixedString& operator=(const char *s)
	{
		len = strlen(s);
		assert(len < N - 1);
		memcpy(buffer, s, len);
		return *this;
	}
	FixedString& operator=(char c)
	{
		len = 1;
		assert(len < N - 1);
		buffer[0] = c;
		return *this;
	}

	void reserve(size_t n) const
	{
		assert(n < N - 1);
	}

	template<size_t sN> void swap(FixedString<sN> &s)
	{
		const size_t s_len = s.size();
		assert(s_len < N - 1);
		assert(len < sN - 1);

		if (len < s_len)
		{
			for (size_t n = 0; n < len; n++)
				util::swap(buffer[n], s.buffer[n]);
			for (size_t n = len; n < s_len; n++)
				buffer[n] = s.buffer[n];
		}
		else
		{
			for (size_t n = 0; n < s_len; n++)
				util::swap(buffer[n], s.buffer[n]);
			for (size_t n = s_len; n < len; n++)
				s.buffer[n] = buffer[n];
		}

		s.len = len;
		len = s_len;
	}

	FixedString& insert(char *pos, char x)
	{
		assert(pos >= begin());
		assert(pos <= end());
		assert(len < N - 1 - 1);

		for (char *c = end(); c > pos; --c)
			*c = *(c - 1);

		*pos = x;
		len = len + 1;
		return *this;
	}
	void insert(char *pos, const char *first, const char *last)
	{
		assert(first <= last);
		assert(pos >= begin());
		assert(pos <= end());
		const size_t s_len = last - first;
		assert(len < N - 1 - s_len);

		for (char *c = end() + s_len - 1; c > pos; --c)
			*c = *(c - s_len);

		memcpy(pos, first, s_len);
		len = len + s_len;
	}
	void insert(char *pos, size_t n, char x)
	{
		assert(pos >= begin());
		assert(pos <= end());
		assert(len < N - 1 - n);

		for (char *c = end() + n - 1; c > pos; --c)
			*c = *(c - n);

		memset(pos, x, n);
		len = len + n;
	}
	template<size_t sN> FixedString& insert(size_t pos, const FixedString<sN> &s)
	{
		assert(pos <= len);
		const size_t s_len = s.len;
		assert(len < N - 1 - s_len);

		for (char *c = end() + s_len - 1; c > buffer + pos; --c)
			*c = *(c - s_len);

		memcpy(buffer + pos, s.buffer, s_len);
		len = len + s_len;
		return *this;
	}
	template<size_t sN> FixedString& insert(size_t pos, const FixedString<sN> &s, size_t pos1, size_t n)
	{
		assert(pos <= len);
		const size_t s_len = util::min(n, s.len - pos1);
		assert(len < N - 1 - s_len);

		for (char *c = end() + s_len - 1; c > buffer + pos; --c)
			*c = *(c - s_len);

		memcpy(buffer + pos, s.buffer + pos1, s_len);
		len = len + s_len;
		return *this;
	}
	FixedString& insert(size_t pos, const char *s)
	{
		assert(pos <= len);
		const size_t s_len = strlen(s);
		assert(len < N - 1 - s_len);

		for (char *c = end() + s_len - 1; c > buffer + pos; --c)
			*c = *(c - s_len);

		memcpy(buffer + pos, s, s_len);
		len = len + s_len;
		return *this;
	}
	FixedString& insert(size_t pos, const char *s, size_t n)
	{
		assert(pos <= len);
		const size_t s_len = n;
		assert(len < N - 1 - s_len);

		for (char *c = end() + s_len - 1; c > buffer + pos; --c)
			*c = *(c - s_len);

		memcpy(buffer + pos, s, s_len);
		len = len + s_len;
		return *this;
	}
	FixedString& insert(size_t pos, size_t n, char x)
	{
		assert(pos <= len);
		const size_t s_len = n;
		assert(len < N - 1 - s_len);

		for (char *c = end() + s_len - 1; c > buffer + pos; --c)
			*c = *(c - s_len);

		memset(buffer + pos, x, s_len);
		len = len + s_len;
		return *this;
	}

	template<size_t sN> FixedString& append(const FixedString<sN> &s)
	{
		insert(len, s);
		return *this;
	}
	template<size_t sN> FixedString& append(const FixedString<sN> &s, size_t pos, size_t n)
	{
		insert(len, s, pos, n);
		return *this;
	}
	FixedString& append(const char *s)
	{
		insert(len, s);
		return *this;
	}
	FixedString& append(const char *s, size_t n)
	{
		insert(len, s, n);
		return *this;
	}
	FixedString& append(size_t n, char x)
	{
		insert(len, n, x);
		return *this;
	}
	FixedString& append(const char *first, const char *last)
	{
		insert(len, first, last);
		return *this;
	}
	FixedString& appendf(const char *format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		vappendf(format, argptr);
		va_end(argptr);

		return *this;
	}
	FixedString& vappendf(const char *format, va_list argptr)
	{
		if (format == 0 || format[0] == 0)
			return *this;
		if (N - len - 1 <= 0)
			return *this;

		// assemble the output string
		int bytesWritten = vsnprintf(buffer + len, N - len - 1, format, argptr);

		if (bytesWritten == -1)
		{
			len = N - 1; // overflowed (safely)
		}
		else
		{
			len += bytesWritten;
		}

		return *this;
	}

	void push_back(char c)
	{
		assert(len < N - 1 - 1);
		buffer[len] = c;
		len++;
	}

	template<size_t sN> FixedString& operator+=(const FixedString<sN> &s)
	{
		append(s);
		return *this;
	}
	FixedString& operator+=(const char *s)
	{
		append(s);
		return *this;
	}
	FixedString& operator+=(char x)
	{
		append(1, x);
		return *this;
	}

	char* erase(char *p)
	{
		assert(p >= begin());
		assert(p < end());

		for (char *c = p; c < end() - 1; ++c)
			*c = *(c + 1);

		len--;

		return p;
	}
	char* erase(char *first, char *last)
	{
		assert(first <= last);
		assert(first >= begin());
		assert(first < end());
		assert(last <= end());
		const size_t erase_size = last - first;

		for (char *c = first; c < end() - erase_size; ++c)
			*c = *(c + erase_size);

		len -= erase_size;

		return first;
	}
	FixedString& erase()
	{
		len = 0;
		return *this;
	}
	FixedString& erase(size_t pos)
	{
		assert(pos <= len);
		len -= len - pos;
		return *this;
	}
	FixedString& erase(size_t pos, size_t n)
	{
		assert(pos + n <= len);
		const size_t erase_size = n;

		for (char *c = buffer + pos; c < end() - erase_size; ++c)
			*c = *(c + erase_size);

		len -= erase_size;
		return *this;
	}

	void clear()
	{
		len = 0;
	}

	void resize(size_t n, char x = 0)
	{
		assert(n < N - 1);
		for (size_t i = len; i < n; ++i)
			buffer[i] = x;
		len = n;
	}

	template<size_t sN> FixedString& assign(const FixedString<sN> &s)
	{
		operator=(s);
		return *this;
	}
	template<size_t sN> FixedString& assign(const FixedString<sN> &s, size_t pos, size_t n)
	{
		len = n;
		assert(len < N - 1);
		memcpy(buffer, s.buffer + pos, n);
		return *this;
	}
	FixedString& assign(const char *s, size_t n)
	{
		len = n;
		assert(len < N - 1);
		memcpy(buffer, s, n);
		return *this;
	}
	FixedString& assign(const char *s)
	{
		operator=(s);
		return *this;
	}
	FixedString& assign(size_t n, char x)
	{
		len = n;
		assert(len < N - 1);
		memset(buffer, x, n);
		return *this;
	}
	FixedString& assign(const char *first, const char *last)
	{
		assert(first <= last);
		len = last - first;
		assert(len < N - 1);
		memcpy(buffer, first, len);
		return *this;
	}

	template<size_t sN> FixedString& replace(size_t pos, size_t n, const FixedString<sN> &s)
	{
		erase(pos, n);
		insert(pos, s);
		return *this;
	}
	template<size_t sN> FixedString& replace(size_t pos, size_t n, const FixedString<sN> &s, size_t pos1, size_t n1)
	{
		erase(pos, n);
		insert(pos, s, pos1, n1);
		return *this;
	}
	FixedString& replace(size_t pos, size_t n, const char *s, size_t n1)
	{
		erase(pos, n);
		insert(pos, s, n1);
		return *this;
	}
	FixedString& replace(size_t pos, size_t n, const char *s)
	{
		erase(pos, n);
		insert(pos, s);
		return *this;
	}
	FixedString& replace(size_t pos, size_t n, size_t n1, char x)
	{
		erase(pos, n);
		insert(pos, n1, x);
		return *this;
	}
	template<size_t sN> FixedString& replace(char *first, char *last, const FixedString<sN> &s)
	{
		insert(erase(first, last), s.begin(), s.end());
		return *this;
	}
	FixedString& replace(char *first, char *last, const char *s, size_t n)
	{
		insert(erase(first, last), s, n);
		return *this;
	}
	FixedString& replace(char *first, char *last, const char *s)
	{
		insert(erase(first, last), s, s + strlen(s));
		return *this;
	}
	FixedString& replace(char *first, char *last, size_t n, char x)
	{
		insert(erase(first, last), n, x);
		return *this;
	}
	FixedString& replace(char *first, char *last, const char *f, const char *l)
	{
		insert(erase(first, last), f, l);
		return *this;
	}

	size_t copy(char *buf, size_t n, size_t pos = 0) const
	{
		const size_t copy_size = util::min(n, len - pos);
		memcpy(buf, buffer + pos, copy_size);
		return copy_size;
	}

	template<size_t sN> size_t find(const FixedString<sN> &s, size_t pos = 0) const
	{
		for (size_t i = pos; i <= len - s.len; ++i)
		{
			size_t j = 0;
			for (j = 0; j < s.len && i + j < len; ++j)
			{
				if (s.buffer[j] != buffer[i + j])
					break;
			}
			if (j == len)
				return i;
		}
		return size_t(-1);
	}
	size_t find(const char *s, size_t pos, size_t n) const
	{
		const size_t s_len = n;
		for (size_t i = pos; i <= len - s_len; ++i)
		{
			size_t j = 0;
			for (j = 0; j < s_len && i + j < len; ++j)
			{
				if (s[j] != buffer[i + j])
					break;
			}
			if (j == len)
				return i;
		}
		return size_t(-1);
	}
	size_t find(const char *s, size_t pos = 0) const
	{
		const size_t s_len = strlen(s);
		for (size_t i = pos; i <= len - s_len; ++i)
		{
			size_t j = 0;
			for (j = 0; j < s_len && i + j < len; ++j)
			{
				if (s[j] != buffer[i + j])
					break;
			}
			if (j == len)
				return i;
		}
		return size_t(-1);
	}
	size_t find(char x, size_t pos = 0) const
	{
		for (size_t i = pos; i < len; ++i)
		{
			if (buffer[i] == x)
				return i;
		}
		return size_t(-1);
	}

	template<size_t sN> size_t rfind(const FixedString<sN> &s, size_t pos = size_t(-1)) const
	{
		for (size_t i = util::min(pos, len - s.len); i != size_t(-1); --i)
		{
			size_t j = 0;
			for (j = 0; j < s.len && i + j < len; ++j)
			{
				if (s.buffer[j] != buffer[i + j])
					break;
			}
			if (j == len)
				return i;
		}
		return size_t(-1);
	}
	size_t rfind(const char *s, size_t pos, size_t n) const
	{
		const size_t s_len = n;
		for (size_t i = util::min(pos, len - s_len); i != size_t(-1); --i)
		{
			size_t j = 0;
			for (j = 0; j < s_len && i + j < len; ++j)
			{
				if (s[j] != buffer[i + j])
					break;
			}
			if (j == len)
				return i;
		}
		return size_t(-1);
	}
	size_t rfind(const char *s, size_t pos = size_t(-1)) const
	{
		const size_t s_len = strlen(s);
		for (size_t i = util::min(pos, len - s_len); i != size_t(-1); --i)
		{
			size_t j = 0;
			for (j = 0; j < s_len && i + j < len; ++j)
			{
				if (s[j] != buffer[i + j])
					break;
			}
			if (j == len)
				return i;
		}
		return size_t(-1);
	}
	size_t rfind(char x, size_t pos = size_t(-1)) const
	{
		for (size_t i = util::min(pos, len - 1); i != size_t(-1); --i)
		{
			if (buffer[i] == x)
				return i;
		}
		return size_t(-1);
	}

	template<size_t sN> size_t find_first_of(const FixedString<sN> &s, size_t pos = 0) const
	{
		for (size_t i = pos; i < len; ++i)
		{
			for (size_t j = 0; j < s.len; ++j)
			{
				if (buffer[i] == s.buffer[j])
					return i;
			}
		}
		return size_t(-1);
	}
	size_t find_first_of(const char *s, size_t pos, size_t n) const
	{
		const size_t s_len = n;
		for (size_t i = pos; i < len; ++i)
		{
			for (size_t j = 0; j < s_len; ++j)
			{
				if (buffer[i] == s[j])
					return i;
			}
		}
		return size_t(-1);
	}
	size_t find_first_of(const char *s, size_t pos = 0) const
	{
		const size_t s_len = strlen(s);
		for (size_t i = pos; i < len; ++i)
		{
			for (size_t j = 0; j < s_len; ++j)
			{
				if (buffer[i] == s[j])
					return i;
			}
		}
		return size_t(-1);
	}
	size_t find_first_of(char x, size_t pos = 0) const
	{
		return find(x, pos);
	}

	template<size_t sN> size_t find_first_not_of(const FixedString<sN> &s, size_t pos = 0) const
	{
		for (size_t i = pos; i < len; ++i)
		{
			size_t j = 0;
			for (j = 0; j < s.len; ++j)
			{
				if (buffer[i] == s.buffer[j])
					break;
			}
			if (j == s.len)
				return i;
		}
		return size_t(-1);
	}
	size_t find_first_not_of(const char *s, size_t pos, size_t n) const
	{
		const size_t s_len = n;
		for (size_t i = pos; i < len; ++i)
		{
			size_t j = 0;
			for (j = 0; j < s_len; ++j)
			{
				if (buffer[i] == s[j])
					break;
			}
			if (j == s_len)
				return i;
		}
		return size_t(-1);
	}
	size_t find_first_not_of(const char *s, size_t pos = 0) const
	{
		const size_t s_len = strlen(s);
		for (size_t i = pos; i < len; ++i)
		{
			size_t j = 0;
			for (j = 0; j < s_len; ++j)
			{
				if (buffer[i] == s[j])
					break;
			}
			if (j == s_len)
				return i;
		}
		return size_t(-1);
	}
	size_t find_first_not_of(char x, size_t pos = 0) const
	{
		for (size_t i = pos; i < len; ++i)
		{
			if (buffer[i] != x)
				return i;
		}
		return size_t(-1);
	}

	template<size_t sN> size_t find_last_of(const FixedString<sN> &s, size_t pos = 0) const
	{
		for (size_t i = util::min(pos, len - 1); i != size_t(-1); --i)
		{
			for (size_t j = 0; j < s.len; ++j)
			{
				if (buffer[i] == s.buffer[j])
					return i;
			}
		}
		return size_t(-1);
	}
	size_t find_last_of(const char *s, size_t pos, size_t n) const
	{
		const size_t s_len = n;
		for (size_t i = util::min(pos, len - 1); i != size_t(-1); --i)
		{
			for (size_t j = 0; j < s_len; ++j)
			{
				if (buffer[i] == s[j])
					return i;
			}
		}
		return size_t(-1);
	}
	size_t find_last_of(const char *s, size_t pos = 0) const
	{
		const size_t s_len = strlen(s);
		for (size_t i = util::min(pos, len - 1); i != size_t(-1); --i)
		{
			for (size_t j = 0; j < s_len; ++j)
			{
				if (buffer[i] == s[j])
					return i;
			}
		}
		return size_t(-1);
	}
	size_t find_last_of(char x, size_t pos = 0) const
	{
		return rfind(x, pos);
	}

	template<size_t sN> size_t find_last_not_of(const FixedString<sN> &s, size_t pos = 0) const
	{
		for (size_t i = util::min(pos, len - 1); i != size_t(-1); --i)
		{
			size_t j = 0;
			for (j = 0; j < s.len; ++j)
			{
				if (buffer[i] == s.buffer[j])
					break;
			}
			if (j == s.len)
				return i;
		}
		return size_t(-1);
	}
	size_t find_last_not_of(const char *s, size_t pos, size_t n) const
	{
		const size_t s_len = n;
		for (size_t i = util::min(pos, len - 1); i != size_t(-1); --i)
		{
			size_t j = 0;
			for (j = 0; j < s_len; ++j)
			{
				if (buffer[i] == s[j])
					break;
			}
			if (j == s_len)
				return i;
		}
		return size_t(-1);
	}
	size_t find_last_not_of(const char *s, size_t pos = 0) const
	{
		const size_t s_len = strlen(s);
		for (size_t i = util::min(pos, len - 1); i != size_t(-1); --i)
		{
			size_t j = 0;
			for (j = 0; j < s_len; ++j)
			{
				if (buffer[i] == s[j])
					break;
			}
			if (j == s_len)
				return i;
		}
		return size_t(-1);
	}
	size_t find_last_not_of(char x, size_t pos = 0) const
	{
		for (size_t i = util::min(pos, len - 1); i != size_t(-1); --i)
		{
			if (buffer[i] != x)
				return i;
		}
		return size_t(-1);
	}

	FixedString substr() const
	{
		return *this;
	}
	FixedString substr(size_t pos) const
	{
		return FixedString(*this, pos);
	}
	FixedString substr(size_t pos, size_t n) const
	{
		return FixedString(*this, pos, n);
	}

	template<size_t sN> int compare(const FixedString<sN> &s) const
	{
		const size_t stop = util::min(len, s.len);
		for (size_t i = 0; i < stop; ++i)
		{
			if (buffer[i] != s.buffer[i])
				return buffer[i] - s.buffer[i];
		}
		if (len != s.len)
			return int(len - s.len);
		return 0;
	}
	template<size_t sN> int compare(size_t pos, size_t n, const FixedString<sN> &s) const
	{
		const size_t stop = util::min(util::min(len - pos, s.len), n);
		for (size_t i = 0; i < stop; ++i)
		{
			if (buffer[i + pos] != s.buffer[i])
				return buffer[i + pos] - s.buffer[i];
		}
		if (n != s.len)
			return n - s.len;
		return 0;
	}
	template<size_t sN> int compare(size_t pos, size_t n, const FixedString<sN> &s, size_t pos1, size_t n1) const
	{
		const size_t stop = util::min(util::min(util::min(len - pos, s.len - pos1), n), n1);
		for (size_t i = 0; i < stop; ++i)
		{
			if (buffer[i + pos] != s.buffer[i + pos1])
				return buffer[i + pos] - s.buffer[i + pos1];
		}
		if (util::min(n, len - pos) != util::min(n1, s.len))
			return util::min(n, len - pos) - util::min(n1, s.len);
		return 0;
	}
	int compare(const char *s) const
	{
		const size_t s_len = strlen(s);
		const size_t stop = util::min(len, s_len);
		for (size_t i = 0; i < stop; ++i)
		{
			if (buffer[i] != s[i])
				return buffer[i] - s[i];
		}
		if (len != s_len)
			return int(len - s_len);
		return 0;
	}
	int compare(size_t pos, size_t n, const char *s) const
	{
		const size_t s_len = strlen(s);
		const size_t stop = util::min(util::min(len - pos, s_len), n);
		for (size_t i = 0; i < stop; ++i)
		{
			if (buffer[i + pos] != s[i])
				return buffer[i + pos] - s[i];
		}
		if (util::min(n, len - pos) != s_len)
			return util::min(n, len - pos) - s_len;
		return 0;
	}
	int compare(size_t pos, size_t n, const char *s, size_t n1) const
	{
		const size_t s_len = strlen(s);
		const size_t stop = util::min(util::min(util::min(len - pos, s_len), n), n1);
		for (size_t i = 0; i < stop; ++i)
		{
			if (buffer[i + pos] != s[i])
				return buffer[i + pos] - s[i];
		}
		if (util::min(n, len - pos) != util::min(n1, s_len))
			return util::min(n, len - pos) - util::min(n1, s_len);
		return 0;
	}


	// extensions

	// returns starting position for next extraction
	// returns -1 if no more tokens available
	template<size_t sN> int extractToken(int pos, int delimiter, FixedString<sN> &token) const
	{
		token.clear();

		if (pos < 0 || (size_t)pos >= len)
			return -1; // done

		const size_t stop = util::min<size_t>(pos + sN, len);
		for (size_t i = pos; i < stop; i++)
		{
			if (buffer[i] == delimiter)
				return int(i + 1);

			token += buffer[i];
		}

		return stop;
	}
};

template<size_t N1, size_t N2> FixedString<N1> operator+(const FixedString<N1> &s1, const FixedString<N2> &s2)
{
	FixedString<N1> r(s1);
	r.append(s2);
	return r;
}
template<size_t N> FixedString<N> operator+(const char *s1, const FixedString<N> &s2)
{
	FixedString<N> r(s1);
	r.append(s2);
	return r;
}
template<size_t N> FixedString<N> operator+(const FixedString<N> &s1, const char *s2)
{
	FixedString<N> r(s1);
	r.append(s2);
	return r;
}
template<size_t N> FixedString<N> operator+(char s1, const FixedString<N> &s2)
{
	FixedString<N> r();
	r.push_back(s1);
	r.append(s2);
	return r;
}
template<size_t N> FixedString<N> operator+(const FixedString<N> &s1, char s2)
{
	FixedString<N> r(s1);
	r.append(s2);
	return r;
}

template<size_t N1, size_t N2> bool operator==(const FixedString<N1> &s1, const FixedString<N2> &s2)
{
	return s1.compare(s2) == 0;
}
template<size_t N> bool operator==(const char *s1, const FixedString<N> &s2)
{
	return s2.compare(s1) == 0;
}
template<size_t N> bool operator==(const FixedString<N> &s1, const char *s2)
{
	return s1.compare(s2) == 0;
}

template<size_t N1, size_t N2> bool operator!=(const FixedString<N1> &s1, const FixedString<N2> &s2)
{
	return s1.compare(s2) != 0;
}
template<size_t N> bool operator!=(const char *s1, const FixedString<N> &s2)
{
	return s2.compare(s1) != 0;
}
template<size_t N> bool operator!=(const FixedString<N> &s1, const char *s2)
{
	return s1.compare(s2) != 0;
}

template<size_t N1, size_t N2> bool operator<(const FixedString<N1> &s1, const FixedString<N2> &s2)
{
	return s1.compare(s2) < 0;
}
template<size_t N> bool operator<(const char *s1, const FixedString<N> &s2)
{
	return s2.compare(s1) > 0;
}
template<size_t N> bool operator<(const FixedString<N> &s1, const char *s2)
{
	return s1.compare(s2) < 0;
}

template<size_t N1, size_t N2> void swap(FixedString<N1> &s1, FixedString<N2> &s2)
{
	s1.swap(s2);
}

#endif