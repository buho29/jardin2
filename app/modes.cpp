#include "modes.h"
#include "Model.h"


/*			Expression			*/

Expression::Expression() { }

void Expression::begin()
{
	model = Model::instance(); 
}

void Expression::setEnabled(bool value){enabled = value;}

bool Expression::getEnabled(){return enabled;}


int8_t WeaterExpression::evaluate()
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
		WeatherData::Forecast *fore = this->model->weather.getCurrent();
		int16_t result = fore->cloudCover;
		result += fore->precipitationProbability;
		result += fore->hoursOfPrecipitation * 100;
		result = MIN(result / 3, 100);
		Serial.printf("weater evaluate %d cloud%d precipitation %d%% hoursOfPrecipitation %dh\n", result,
			fore->cloudCover, fore->precipitationProbability, fore->hoursOfPrecipitation);

		return (int8_t)result;
	}
	return -1;
}

bool WeaterExpression::skip()
{
	if (this->model && this->model->loadedForescast)
		// cancelamos cuando el viento es mas de 8km/h
		return this->model->weather.speedWin() > 8;
	return false;
}

const char * WeaterExpression::getName(){return "Weater";}

int8_t SensorExpression::evaluate()
{
	float averageTmp = 0, averageHum = 0;

	auto & sensors = model->getSensorsLog();

	if (!sensors.size()) return -1;

	for (SensorsItem* item : sensors)
	{
		averageTmp += item->temperature;
		averageHum += item->humidity;
	};


	Serial.printf("tmp %.0f hum %.0f size %d \n", averageTmp,averageHum, sensors.size());

	averageTmp /= sensors.size();
	averageHum /= sensors.size();

	float result = map(averageTmp, 0, 30, 100, 0);
	result += map(averageHum, 30, 80, 0, 100);

	result /= 2;

	Serial.printf("tmp %.0f hum %.0f \n", averageTmp,averageHum);
	Serial.printf("sensor evaluate %.0f tmp %d hum %d \n", result,
		map(averageTmp, 0, 30, 100, 0), map(averageHum, 30, 80, 0, 100));
	//Serial.printf("tmp1 %d tmp2 %d \n",map(10,0,30,100,0), map(15,0,30,100,0));
	//Serial.printf("tmp3 %d tmp4 %d \n",map(20,0,30,100,0), map(25,0,30,100, 0));

	return  MIN((int8_t)(result), 100);
}


bool SensorExpression::skip()
{
	// cancelamos cuando la temp es menos 5°
	return this->model->currentSensor.temperature < 5;
}

const char * SensorExpression::getName(){return "sensors";}


ListExpression::ListExpression(){enabled = true;}

void ListExpression::begin()
{
	uint i = -1;
	while (has(++i))
	{
		get(i)->begin();
	}
}

int8_t ListExpression::evaluate()
{
	uint16_t result = 0;
	uint8_t c = 0;
	for (uint8_t i = 0; i < count; i++)
	{
		Expression * exp = expressionList[i];
		int8_t ev = exp->evaluate();
		if (exp->getEnabled() && ev > -1) {

			result += ev;
			c++;
		}
	}
	if (c == 0) return -1;
	return (int8_t)(result / c);
}

bool ListExpression::skip()
{
	for (uint8_t i = 0; i < count; i++)
	{
		Expression * exp = expressionList[i];
		if (exp->getEnabled() && exp->skip())
			return true;
	}
	return false;
}

bool ListExpression::add(Expression * exp)
{
	if (count < 20) {
		expressionList[count++] = exp;
		return true;
	}
	return false;
}

Expression * ListExpression::get(uint8_t index)
{
	if (index < count) {
		return expressionList[index];
	}
	return nullptr;
}

bool ListExpression::has(uint8_t index){return get(index) != nullptr;}

const char * ListExpression::getName(){return "List";}

int8_t Disable24Expression::evaluate(){ return -1; }

bool Disable24Expression::skip() { return true; }

void Disable24Expression::setEnabled(bool value)
{
	if (value == enabled) return;

	Tasker & tasker = Tasker::instance();

	this->enabled = value;
	if (value) {
		if (task != nullptr)
			tasker.remove(task);

		using namespace std::placeholders;
		task = tasker.setTimeout(std::bind(&Disable24Expression::onTimeOut, this, _1), 24 * 60 * 60);
	}
	else {
		if (task != nullptr)
			tasker.remove(task);
		task = nullptr;
	}
}

void Disable24Expression::onTimeOut(Task * current) { setEnabled(false); }

const char * Disable24Expression::getName() { return "Disable24"; }

int8_t dayExpression::evaluate() { return -1; }

bool dayExpression::skip()
{
	return ClockTime::instance().dayWeek() == day;
}

const char * dayExpression::getName() { return "day"; }

Modes::Modes()
{
	add(&disable24);
	add(&weater);
	add(&sensor);
	for (uint8_t i = 0; i < 7; i++)
	{
		dayExpression * day = &week[i];
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
		Expression * exp = get(i);
		if (exp->getEnabled()) {
			fr.add(modesArr[i]);
		}
	}
	return fr.get();
}

void Modes::setFlags(uint f)
{
	Flags fl = Flags(f);
	int i = -1;
	while (has(++i))
	{
		Expression * exp = get(i);
		bool enabled = fl.contain(modesArr[i]);

		exp->setEnabled(enabled);
		//Serial.printf("%s %s, ", exp->getName(), (exp->getEnabled() ? "true" : "false"));
	}
}

const uint Modes::modesArr[countModes] = {
		disable24F ,weatherF,sensorsF ,mondayF, tuesdayF ,
		WednesdayF ,ThursdayF,fridayF, saturdayF,sundayF
};