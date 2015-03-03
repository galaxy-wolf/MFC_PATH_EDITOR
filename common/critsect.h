#ifndef CRITSECT_H
#define CRITSECT_H


class CriticalSection
{
public:

	CriticalSection();
	~CriticalSection();

	void enter();
	void leave();

private:
	void *cs;
};

#endif
