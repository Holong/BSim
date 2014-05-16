#ifndef __PREDICTOR_H__
#define __PREDICTOR_H__

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

	unsigned long getJmpAddress(unsigned long)
	{
		return jmpAddress;
	}
};

class BPredictor
{
public:
	virtual void do_predict(unsigned long ip, BranchResult& result);
}














#endif
