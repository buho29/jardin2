#include "Model.h"
#include "modes.h"


/*			Expression			*/


void Interpreter::begin()
{
	model = Model::instance(); 
}

void Interpreter::setEnabled(bool value){enabled = value;}

bool Interpreter::getEnabled(){return enabled;}

void Interpreter::target(int id)
{
	targetId = id;
}

int8_t WeatherInterp::evaluate()
{
	/*{
	  "minTemp": 5,
	  "maxTemp": 18,
	  "cloudCover": 95,
	  "precipitationProbability": 75,
	  "totalLiquid": 4.5,
	  "hoursOfPrecipitation": 3,
	}*/
	if (this->model && this->model->loadedForescast) {
		WeatherData::Forecast *fore = this->model->weather[0].getCurrent();
		int16_t result = fore->cloudCover;
		result += fore->precipitationProbability;
		result += fore->hoursOfPrecipitation * 100;
		result = MIN(result / 3, 100);
		/*Serial.printf("weater evaluate %d cloud%d precipitation %d%% hoursOfPrecipitation %dh\n", result,
			fore->cloudCover, fore->precipitationProbability, fore->hoursOfPrecipitation);*/

		Serial.printf("%s evaluate %d\n", getName(), (int8_t)result);
		return (int8_t)result;
	}
	return -1;
}

bool WeatherInterp::skip()
{
	if (this->model && this->model->loadedForescast) 
	{
		// cancelamos cuando el viento es mas de 8km/h
		bool result = this->model->weather[0].speedWin() > 8;
		if (result) Serial.printf("%s skip\n", getName());
		return result;
	}
	return false;
}

const char * WeatherInterp::getName(){return "Weater";}

int8_t SensorInterp::evaluate()
{
	float averageTmp = 0, averageHum = 0;

	auto & sensors = model->getSensorsLog();

	if (!sensors.size()) return -1;

	for (SensorItem* item : sensors)
	{
		averageTmp += item->temperature;
		averageHum += item->humidity;
	};

	averageTmp /= sensors.size();
	averageHum /= sensors.size();

	float result = map(averageTmp, 0, 30, 100, 0);
	result += map(averageHum, 30, 80, 0, 100);

	result /= 2;

	/*Serial.printf("tmp %.0f hum %.0f ", averageTmp,averageHum);
	Serial.printf("sensor evaluate %.0f tmp %d hum %d \n", result,
		map(averageTmp, 0, 30, 100, 0), map(averageHum, 30, 80, 0, 100));*/
	//Serial.printf("tmp1 %d tmp2 %d \n",map(10,0,30,100,0), map(15,0,30,100,0));
	//Serial.printf("tmp3 %d tmp4 %d \n",map(20,0,30,100,0), map(25,0,30,100, 0));
	result = MIN(result, 100);
	Serial.printf("%s evaluate %d\n", getName(), (int8_t)result);
	return (int8_t) result;
}


bool SensorInterp::skip()
{
	// cancelamos cuando la temp es menos 5°
	bool result = this->model->currentSensor.temperature < 5;
	if (result) Serial.printf("%s skip\n", getName());
	return result;
}

const char * SensorInterp::getName(){return "sensors";}


ListInterpreter::ListInterpreter(){enabled = true;}

void ListInterpreter::begin()
{
	Interpreter::begin();
	int i = -1;
	while (has(++i))
	{
		get(i)->begin();
	}
}

int8_t ListInterpreter::evaluate()
{
	uint16_t result = 0;
	uint8_t c = 0;
	for (uint8_t i = 0; i < count; i++)
	{
		Interpreter * exp = expressionList[i];
		if (exp->getEnabled()) {
			int8_t ev = exp->evaluate();
			if (ev > -1) {
				result += ev;
				c++;
			}
		}
	}
	if (c == 0) return -1;
	return (int8_t)(result / c);
}

bool ListInterpreter::skip()
{
	for (uint8_t i = 0; i < count; i++)
	{
		Interpreter * exp = expressionList[i];
		if (exp->getEnabled() && exp->skip())
			return true;
	}
	return false;
}

bool ListInterpreter::add(Interpreter * exp)
{
	if (count < 20) {
		expressionList[count++] = exp;
		return true;
	}
	return false;
}

Interpreter * ListInterpreter::get(uint8_t index)
{
	if (index < count) {
		return expressionList[index];
	}
	return nullptr;
}

bool ListInterpreter::has(uint8_t index){return get(index) != nullptr;}

void ListInterpreter::target(int id)
{
	targetId = id;

	int i = -1;
	while (has(++i))
	{
		get(i)->target(id);
	}
}

const char * ListInterpreter::getName(){return "List";}

int8_t Disable24Interp::evaluate(){ return -1; }

bool Disable24Interp::skip()
{ 
	Serial.printf("%s skip\n", getName());
	return true; 
}

void Disable24Interp::setEnabled(bool value)
{
	if (value == enabled) return;

	Tasker& tasker = Tasker::instance();

	this->enabled = value;
	if (value) {
		if (task != nullptr)
			tasker.remove(task);

		using namespace std::placeholders;
		task = tasker.setTimeout(std::bind(&Disable24Interp::onTimeOut, this, _1), 24 * 60 * 60);
	}
	else {
		if (task != nullptr)
			tasker.remove(task);
		task = nullptr;
	}
}

void Disable24Interp::onTimeOut(Task * current) { setEnabled(false); }

const char * Disable24Interp::getName() { return "Disable24"; }

int8_t DayInterp::evaluate() { return -1; }

bool DayInterp::skip()
{
	bool result = ClockTime::instance().dayWeek() == day;
	if (result) Serial.printf("%s skip\n",getName());
	return result;
}

const char * DayInterp::getName() { return "day"; }

Modes::Modes()
{
	add(&disable24);
	add(&weater);
	add(&sensor);
	add(&dates);

	for (uint8_t i = 0; i < 7; i++)
	{
		DayInterp * day = &week[i];
		day->day = i + 1;
		add(day);
	}
}

uint Modes::getFlags()
{
	Flags fr;
	int i = -1;
	while (has(++i))
	{
		Interpreter * exp = get(i);
		if (exp->getEnabled()) {
			fr.add(modesArr[i]);
		}
	}
	return fr;
}


void Modes::setFlags(uint f)
{
	Flags fl = Flags(f);
	int i = -1;
	while (has(++i))
	{
		Interpreter * exp = get(i);
		bool enabled = fl.contain(modesArr[i]);

		exp->setEnabled(enabled);
		//Serial.printf("%s %s, ", exp->getName(), (exp->getEnabled() ? "true" : "false"));
	}
}

void Modes::target(int id)
{
	ModesItem* cur = model->getModesItem(id);
	if (cur) setFlags(cur->modes);
	ListInterpreter::target(id);
}

const uint Modes::modesArr[countModes] = {
		disable24F, weatherF, sensorsF, datesF, mondayF, tuesdayF,
		WednesdayF, ThursdayF, fridayF, saturdayF, sundayF
};

bool DatesInterp::isValide(const char* dates)
{
	char datesArray[10][12];

	char str[120];
	strcpy(str, dates);

	char* pch;
	int i = 0;
	pch = strtok(str, "|");
	while (pch != NULL)
	{
		strcpy(datesArray[i++], pch);
		//printf("%s %d\n", pch, i);
		pch = strtok(NULL, "|");
	}
	int count = i;

	for (i = 0;i < count;i++) {

		char buffSt[6];
		char* st = strtok(datesArray[i], "-");
		strcpy(buffSt, st);

		char buffEnd[6];
		st = strtok(NULL, "-");
		if (st == NULL) return false;
		strcpy(buffEnd, st);

		char* mSt = strtok(buffSt, "/");
		char* dSt = strtok(NULL, "/");

		char* mEn = strtok(buffEnd, "/");
		char* dEn = strtok(NULL, "/");

		// TODO no comprueba q sean numeros validos
		if(dSt == NULL || dEn == NULL) 
			return false;
	}
	return true;
}

int8_t DatesInterp::evaluate()
{
	return -1;
}

bool DatesInterp::skip()
{

	ClockTime& clockTime = ClockTime::instance();

	uint32_t date = clockTime.utc();
	uint16_t year = clockTime.year();

	ModesItem* cur = model->getModesItem(targetId);

	if (!cur) return true;

	char datesStr[120];
	strcpy(datesStr, cur->datesStr);

	char datesArray[10][12];

	char str[120];
	strcpy(str, datesStr);

	char* pch;

	int i = 0;
	pch = strtok(str, "|");
	while (pch != NULL)
	{
		strcpy(datesArray[i++], pch);
		//printf("%s %d\n", pch, i);
		pch = strtok(NULL, "|");
	}
	int count = i;
	//atoi(pch)

	for (i = 0;i < count;i++) {

		char buff[12];
		strcpy(buff, datesArray[i]);

		char buffSt[6];
		char* st = strtok(buff, "-");
		strcpy(buffSt, st);

		char buffEnd[6];
		st = strtok(NULL, "-");
		strcpy(buffEnd, st);

		char* mSt = strtok(buffSt, "/");
		char* dSt = strtok(NULL, "/");

		char* mEn = strtok(buffEnd, "/");
		char* dEn = strtok(NULL, "/");

		DateTime stDt = DateTime(year, atoi(mSt), atoi(dSt));
		DateTime enDt = DateTime(year, atoi(mEn), atoi(dEn));

		uint32_t ust = stDt.unixtime();
		uint32_t uen = enDt.unixtime();

		if (date >= ust && date <= uen) return false;
	}
	Serial.printf("%s skip\n",getName());
	return true;
}

const char* DatesInterp::getName(){
	return "Dates";
}
