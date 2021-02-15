// Flag.h

#ifndef _FLAG_h
#define _FLAG_h

#include "arduino.h"
/*
	// se comporta como un array de booleanas

	int lunes		= 0b0000001;
	int martes		= 0b0000010;
	int miercoles	= 0b0000100;
	int jueves		= 0b0001000;
	int viernes		= 0b0010000;
	int sabado		= 0b0100000;
	int domingo		= 0b1000000;

	Flags alarm = Flags(lunes | martes | miercoles | jueves | viernes);

	if (alarm.contain(martes))
		Serial.println("hay alarma el marte");
	if (alarm.check(sabado | domingo))
		Serial.println("hay alarmas el sabado y el domingo");

	if (alarm.contain(sabado | domingo))
		Serial.println("hay alarma el sabado o el domingo o ambas");

	alarm.remove(martes);
	if (!alarm.contain(martes))
		Serial.println("no hay alarma el martes");

	alarm.remove(jueves | domingo);

	if (alarm.check(lunes | miercoles | viernes))
		Serial.println("hay 3 alarmas el lunes|miercoles|viernes");

	alarm.add(domingo);
	if (alarm.contain(domingo))
		Serial.println("toca misa!");

	static int days[7]{
		lunes ,martes ,miercoles ,jueves ,viernes ,sabado ,domingo
	};

	Flags f = lunes | miercoles | viernes | domingo;

	Serial.printf("same f == alarm %s\n", (f == alarm) ? "true" : "false");

	for (int i = 0; i < 7; i++)
		Serial.printf("day %d is %s\n", i+1, (alarm.contain(days[i])?"true":"false"));
*/
class Flags {

private:
	uint mFlags = 0;

public:

	Flags(uint flags = 0){ mFlags = flags;}

	void set(uint flags){ mFlags = flags;}
	
	// @return mFlags
	uint get(){ return mFlags; }
	
	// pone mFlags a 0
	void reset() { mFlags = 0; }

	// agrega un flags
	void add(uint flags){ mFlags |= flags; }

	// elimina un flags
	void remove(uint flags){
		// 1000110 mFlags
		// 0010110 flags
		// 0000110 mFlags & flags , elimina las q no estan encendidas en mFlags 
		// 1000000 0000110 ^ mflags 

		mFlags ^= (mFlags & flags);
	}

	// @return devuelve si cuando existe algunos de los flags
	bool contain(uint flags){
		return (mFlags & flags) > 0;
	}

	// @return devuelve si cuando existe todos los flags
	bool check(uint flags){
		return (mFlags & flags) == flags;
	};

	// compara
	bool operator==(Flags &right) { return (mFlags == right); }
	bool operator!=(Flags &right){ return !(*this == right); }
	//cast flag to uint
	operator uint()const { return mFlags;}
};

#endif

