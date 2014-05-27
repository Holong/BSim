#ifndef __PREDICTOR_H__
#define __PREDICTOR_H__

#include <unistd.h>

class BranchResult
{
private:
	bool TrueOrFalse;
	unsigned long jmpAddress;
public:
	void setTorF(bool tf)
	{
		TrueOrFalse = tf;
	}

	void setJmpAddress(unsigned long addr)
	{
		jmpAddress = addr;
	}

	bool getTorF()
	{
		return TrueOrFalse;
	}

	unsigned long getJmpAddress()
	{
		return jmpAddress;
	}
};

class BPredictor
{
protected:
	pid_t pid;

public:
	char name[100];

	BPredictor(pid_t pid) {
		this->pid = pid;
	};

	virtual char* nameOfPredictor() = 0;
	virtual void do_predict(unsigned long ip, BranchResult& result) = 0;
};

#endif
