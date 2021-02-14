// modes.h
#ifndef _MODES_h
#define _MODES_h

#include "lib/Task.h"
#include "lib/Flag.h"
// declaramos dependencia con model(sin include)
class Model ;

//interface Interpreter
class Expression
{
protected:
	bool enabled = false;
public:
	Model * model;
	Expression();
	virtual void begin();
	// evalua entre 0 a 100 si se riega
	// evaluate() < 50 se riega
	virtual int8_t evaluate() = 0;
	// si true se salta la evaluacion y no se riega
	virtual bool skip() = 0;
	virtual void setEnabled(bool value);
	virtual bool getEnabled();
	virtual const char * getName() = 0;
};

//predicion meteo
class WeaterExpression :public Expression
{
public:
	virtual int8_t evaluate();
	virtual bool skip();
	virtual const char * getName();
};

//desactiva el riego 24h  
class Disable24Expression :public Expression
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

//predicion sensores
class SensorExpression :public Expression
{
public:
	virtual int8_t evaluate();
	virtual bool skip();
	virtual const char * getName();
};

//deactiva el riego un dia de la semana
class dayExpression :public Expression
{
public:
	uint8_t day = 0;
	virtual int8_t evaluate();
	virtual bool skip();
	virtual const char * getName();
};

//lista de interpreters que se comporta como uno solo
class ListExpression :public Expression
{
protected:
	Expression * expressionList[20];
public:
	ListExpression();
	void begin();
	virtual int8_t evaluate();
	virtual bool skip();
	virtual const char * getName();
	bool add(Expression * interpreter);
	Expression * get(uint8_t index);
	bool has(uint8_t index);
	uint8_t count = 0;
};

// root
class Modes : public ListExpression
{
public:
	Modes();

	static const uint disable24F	 = 0b0000000001;
	static const uint weatherF		 = 0b0000000010;
	static const uint sensorsF		 = 0b0000000100;
	static const uint mondayF		 = 0b0000001000;
	static const uint tuesdayF		 = 0b0000010000;
	static const uint WednesdayF	 = 0b0000100000;
	static const uint ThursdayF		 = 0b0001000000;
	static const uint fridayF		 = 0b0010000000;
	static const uint saturdayF		 = 0b0100000000;
	static const uint sundayF		 = 0b1000000000;

	static const uint countModes = 10;

	static const uint modesArr[countModes];

	uint getFlags();
	void setFlags(uint f);


private:
	Disable24Expression disable24;
	WeaterExpression weater;
	SensorExpression sensor;
	dayExpression week[7];
};

#endif

