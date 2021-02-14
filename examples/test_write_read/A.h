// A.h

#ifndef _A_h
#define _A_h
#include "Arduino.h"
#include "B.h"

class A
{
 protected:

private:
	B mB;
public:
	A() :mB(this) {};
	void init();
	void t();
};


#endif


