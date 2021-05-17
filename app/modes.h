// modes.h
#ifndef _MODES_h
#define _MODES_h

#include "lib/Task.h"
#include "lib/Flag.h"
// declaramos dependencia con model(sin include)
class Model ;
class ModesItem;

//interface 
class Interpreter
{
protected:
	bool enabled = false;
	int32_t targetId = -1;
	Model * model;
public:
	virtual void begin();
	// evalua entre 0 a 100 si se riega
	// evaluate() < 50 se riega
	virtual int8_t evaluate() = 0;
	// si true se salta la evaluacion y no se riega
	virtual bool skip() = 0;
	//
	virtual void setEnabled(bool value);
	virtual bool getEnabled();
	//guardamos el zone ID
	virtual void target(int id);
	virtual const char * getName() = 0;
};
//desactiva el riego 24h  
class Disable24Interp :public Interpreter
{
private:
	Task * task;
public:
	virtual int8_t evaluate();
	virtual bool skip();
	virtual void setEnabled(bool value);
	void onTimeOut(Task * current);
	virtual const char * getName();
};

//predicion meteo
class WeatherInterp :public Interpreter
{
public:
	virtual int8_t evaluate();
	virtual bool skip();
	virtual const char * getName();
};


//predicion sensores
class SensorInterp :public Interpreter
{
public:
	virtual int8_t evaluate();
	virtual bool skip();
	virtual const char * getName();
};

//desactiva el riego un dia de la semana
class DayInterp :public Interpreter
{
public:
	uint8_t day = 0;
	virtual int8_t evaluate();
	virtual bool skip();
	virtual const char * getName();
};


//activa el riego por intervalos de fechas
class DatesInterp :public Interpreter
{
public:
	static bool isValide(const char* rangs);
	virtual int8_t evaluate();
	virtual bool skip();
	virtual const char* getName();
};

//lista de interpreters que se comporta como uno solo
class ListInterpreter :public Interpreter
{
protected:
	Interpreter * expressionList[20];
public:
	ListInterpreter();
	void begin();
	virtual int8_t evaluate();
	virtual bool skip();
	virtual const char * getName();
	//list
	bool add(Interpreter * interpreter);
	Interpreter * get(uint8_t index);
	bool has(uint8_t index);
	virtual void target(int id);
	uint8_t count = 0;
};


// root
class Modes : public ListInterpreter
{
private:
	static const uint countModes = 11;
	static const uint modesArr[countModes];
	uint getFlags();
	void setFlags(uint f);
public:
	Modes();

	static const uint disable24F	 = 0b00000000001;
	static const uint weatherF		 = 0b00000000010;
	static const uint sensorsF		 = 0b00000000100;
	static const uint rangsF		 = 0b00000001000;
	static const uint mondayF		 = 0b00000010000;
	static const uint tuesdayF		 = 0b00000100000;
	static const uint WednesdayF	 = 0b00001000000;
	static const uint ThursdayF		 = 0b00010000000;
	static const uint fridayF		 = 0b00100000000;
	static const uint saturdayF		 = 0b01000000000;
	static const uint sundayF		 = 0b10000000000;

	virtual void target(int id);

private:
	Disable24Interp disable24;
	WeatherInterp weater;
	SensorInterp sensor;
	DayInterp week[7];
	DatesInterp rangs;
};

#endif

