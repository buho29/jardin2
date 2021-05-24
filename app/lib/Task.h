// tools.h

#ifndef _TASK_h
#define _TASK_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifndef __AVR__
#include <functional>
#endif // __AVR__

#include "Clock.h"
class Task;

#ifndef __AVR__
typedef std::function<void(Task *)> p_callbackTask;
#else
typedef void(*p_callbackTask)(Task * task);
#endif // __AVR__

#define TASK_MAX_SIZE 300
#define TASK_INTERVAL 100
#define TASK_TICKS_24H 86400 //% 24h*60min*60s

class Task
{
	friend class Tasker;
public:
	Task() {};
	uint8_t id = 0;
	uint32_t start = 0;
	uint32_t stop = 0;
	bool runing = false;
	bool enabled = true; 
	void addEvent(p_callbackTask callback) { this->callback = callback; };

private:
	p_callbackTask callback;
	void setup(uint32_t dateStart, uint32_t dateStop,
		p_callbackTask callback)
	{
		this->start = dateStart;
		this->stop = dateStop;
		this->callback = callback;
	};
	int8_t mode = 0;// 1 setTimout | 0 ignora | -1 reusar | 2 setInterval
};


class Tasker
{

 public:
	 
	 static Tasker & instance() {
		 static  Tasker pInstance;
		 return pInstance;
	 };

	 Task* setTimeout(p_callbackTask callback, uint32_t s) {

		 uint32_t dateStart = getTickTime(clockTime.hour(), clockTime.min(), clockTime.sec() + s);

		 Task* t = add(dateStart, dateStart + s, callback);
		 t->mode = 1;// lo marcamos para q se ejecute solo una vez

		 return t;
	 };
	 Task* setDateout(p_callbackTask callback, uint32_t dateStart) {

		 dateStart %= TASK_TICKS_24H;
		 Task* t = add(dateStart, dateStart + 60, callback);
		 t->mode = 1;// lo marcamos para q se ejecute solo una vez

		 return t;
	 };
	Task* setTimeout(p_callbackTask callback, uint8_t h, uint8_t m, uint8_t s = 0)
	{
		uint32_t dateStart = getTickTime(h, m, s);

		Task* t = add(dateStart, dateStart + s, callback);
		t->mode = 1;// lo marcamos para q ejecute solo una vez

		return t;
	};
	Task *  setInterval(p_callbackTask callback, uint16_t s) 
	{

		uint32_t dateStart = getTickTime(clockTime.hour(), clockTime.min(), clockTime.sec() + s);

		Task * t = add(dateStart, dateStart + s, callback);
		t->mode = 2;// lo marcamos para q se repita

		return t;
	};
	Task *  setInterval(p_callbackTask callback, uint8_t h, uint8_t m, uint8_t s = 0)
	{
		uint32_t dateStart = getTickTime(h, m, s);

		Task * t = add(dateStart, dateStart + 1, callback);
		t->mode = 2;// lo marcamos para q se repita

		return t;
	};

	void timeHasChanged() {
		uint32_t tnow = timeNow();
		for (uint8_t i = 0; i < taskCount; i++)
		{
			Task* t = &tasks[i];
			//setInterval
			if (t->mode == 2) {
				int duration = t->stop - t->start;
				t->start = tnow;
				t->stop = tnow + duration;
			}
		}
	}

	Task * add(uint32_t dateStart, uint32_t dateStop,
		p_callbackTask callback) 
	{
		//buscar algun "borrado"
		Task *t = NULL;
		for (uint8_t i = 0; i < taskCount; i++)
		{
			t = &tasks[i];
			if (t->mode == -1)
				break;
			t = NULL;
		}

		// si lo encontramos lo reiniciamos
		if (t != NULL) {
			t->enabled = true;
			t->runing = false;
			t->mode = 0;
			t->setup(
				dateStart,	// inicio 
				dateStop,	// fin 
				callback	// callback
			);
			//Serial.printf("reciclado id %d!\n",t->id);
			return t;
		}
		// si no, se crea uno si se puede
		if (taskCount < TASK_MAX_SIZE) {
			t = &tasks[taskCount];
			t->setup(
				dateStart,	// inicio 
				dateStop,	// fin 
				callback	// callback
			);
			tasks[taskCount].id = taskCount;
			taskCount++;
			return t;
		}

		Serial.printf("excceded task ,max size is %d", TASK_MAX_SIZE);
		return NULL;
	};

	//marca task para su reutilizacion
	void remove(Task * task) 
	{
		if (task == nullptr) {
			Serial.println("Remove(task) is null");
			return;
		}
		task->mode = -1;
		taskCount--;
	};

	// Devuelve un puntero de la primera ocurencia de id
	Task* get(uint8_t id) {
		for (uint8_t i = 0; i < taskCount; i++)
		{
			Task *t = &tasks[i];
			if (t->id == id) return t;
		}
		Serial.printf("task not found - get(%d) \n",id);
		return nullptr;
	};
	
	// a meter en un loop
	void check() {

		if (millis() - time > TASK_INTERVAL) {
			time = millis();
			//update
			clockTime.timeNow();
			uint32_t current = timeNow();
			for (uint8_t i = 0; i < taskCount; i++)
			{
				Task * t = &tasks[i];

				//12:00 -> 12:01
				if (t->mode >= 0 && t->start < t->stop) {

					if (current >= t->start && current < t->stop
						&& !t->runing) {

						startTask(t);
					}
					else if (current > t->start && current >= t->stop
						&& t->runing) {

						stopTask(t);
					}
				}
				//23:59 -> 0:02
				else if (t->mode == 0 && t->start > t->stop) {


					if (current > t->start && (current > t->stop)
						&& !t->runing) {

						startTask(t);
					}
					else if (current < t->start && current > t->stop
						&& t->runing) {

						stopTask(t);
					}
				}
			}
		}

	};

	//devuelve los tick de el dia (24h)
	static uint32_t getTickTime(uint16_t h, uint16_t m, uint16_t s) {
		return (h * 3600 + m * 60 + s) % TASK_TICKS_24H;
	};
	//devuelve los tick de el tiempo actual (24h)
	uint32_t timeNow()
	{
		return clockTime.local() % TASK_TICKS_24H;
	};

	//convierte en string los ticks
	static void formatTime(char * result, uint32_t date) 
	{
		uint8_t h, m, s;
		uint32_t time = date;

		s = time % 60;
		time /= 60;
		m = time % 60;
		time /= 60;
		h = time % 24;

		snprintf(result, 9, "%02d:%02d:%02d", h, m, s);
	};

	//convierte en string los ticks
	static String formatTime(uint32_t date) {
		char buff[9];
		formatTime(buff, date);
		return String(buff);
	};

	static uint32_t getDuration(uint32_t init, uint32_t end) {
		if (init > end)
			return TASK_TICKS_24H - init + end;
		else return end - init;
	};

	uint32_t howTimeLeft(uint32_t date) {
		uint32_t _now = timeNow();
		return getDuration(_now, date);
	};
	static void printTask(Task *t) 
	{
		uint16_t d = Tasker::getDuration(t->start, t->stop);
		Serial.printf("--- task time: %s duration: %d id: %d mode: %d runing: %d enabled %d\n",
			Tasker::formatTime(t->start).c_str(), d, t->id, t->mode, t->runing, t->enabled);
	};
	
private:

	Tasker() {};
	Task tasks[TASK_MAX_SIZE];
	uint8_t taskCount;
	uint32_t time;
	void startTask(Task *t) {
		t->runing = true;
		if (t->mode == 1)// es un setTimeOut
			t->mode = -1;// lo reciclamos

		t->callback(t);

		if (t->mode == 2) {//es un setInterval

			// recuperamos el interval de tiempo
			uint16_t ms = t->stop - t->start;

			// si es diario (24h) nos salimos
			if (ms == 1) return;

			// lo detenemos
			t->runing = false;

			uint32_t dateStart = getTickTime(clockTime.hour(), clockTime.min(), clockTime.sec() + ms);

			t->start = dateStart;
			t->stop = t->start + ms;
		}
	};
	void stopTask(Task *t) {
		t->runing = false;
		if (t->mode == 0)
			t->callback(t);
	};
	ClockTime & clockTime = ClockTime::instance();
};

#endif

