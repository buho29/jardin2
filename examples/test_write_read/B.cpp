#include "B.h"
// We include "A.h" in the cpp file
#include "A.h" 

B::B(A * a)
{
	mA = a;
}

void B::init()
{
	mA->t();
	Serial.println("B init");
}


