// B.h

#ifndef _B_h
#define _B_h
// forward declaration
class A; 

class B
{
 
private:

 public:
	B(A* a);
	A* mA;
	void init();
};


#endif

