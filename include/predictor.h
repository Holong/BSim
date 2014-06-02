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
	char name[100];

public:

	BPredictor() {
	};

	char* nameOfPredictor() {
		return name;
	};

	void setPid(pid_t pid)
	{
		this->pid = pid;
	};

	virtual void do_predict(unsigned long ip, BranchResult& result) = 0;
	virtual void after_predict(unsigned long ip, unsigned long nextIP, bool success) = 0;
};

#endif
