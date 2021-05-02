
#include <AUnit.h>
using aunit::TestRunner;

#define private public

#include "Model.h"

Tasker & tasker = Tasker::instance();
ClockTime & clockTime = ClockTime::instance();
Model * model = Model::instance();

/**********************************************
*				helper/tools 
***********************************************/

// creamos zona con 3 alarmas 
void createZone(uint32_t time) 
{
	uint8_t duration = 5;
	
	int id = model->addZone("Cesped D", 0);

	model->addAlarm(id, 0, time, duration);
	model->addAlarm(id, 1, duration + time, duration);
	model->addAlarm(id, 2, duration * 2 + time, duration);
	//model->printAlarms(id);
}

void printZone()
{
	AlarmItem* currentAlarm = model->alarms[model->currentZone->alarmId];
	Serial.printf("alarm id %d time %s %s <--> %s paused %s elapsed%d\n",
		model->currentZone->alarmId,
		Tasker::formatTime(clockTime.timeNow()),
		Tasker::formatTime(currentAlarm->time),
		Tasker::formatTime(currentAlarm->time + currentAlarm->duration),
		model->currentZone->paused ? "true" : "false",
		model->getElapsedAlarm()
	);
}

void checkZone(int &tsecs, int secs, bool print = false) {
	for (int i = 0; i < secs; i++)
	{
		delay(1000);
		tasker.check();
		if (model->currentZone) {

			if (print) printZone();

			tsecs++;
		}
		Serial.print(".");
	}
	Serial.println();
}

bool checkWatering(int secs, bool print = false) {
	for (int i = 0; i < secs; i++)
	{
		delay(1000);
		tasker.check();
		if (model->currentZone) {
			if (print) printZone();

			if (model->isWatering()) return false;
		}
		Serial.print(".");
	}
	Serial.println();
	return true;
}

/**********************************************
*					tests
***********************************************/

test(_Zone) 
{
	int errors = 0;

	uint time = clockTime.timeNow() - 3600;


	int id = model->addZone("Cesped D", 0);
	//si es 0 o mas 
	assertMore(id, -1);

	uint8_t duration = 5;

	assertMore(model->addAlarm(id, 0, time, duration), -1);
	assertMore(model->addAlarm(id, 1, duration + time, duration), -1);
	assertMore(model->addAlarm(id, 2, duration * 2 + time, duration), -1);
	//model->printAlarms(id);

	//borramos
	assertTrue(model->removeZone(0));
	
	assertFalse(model->alarms.size());
	assertFalse(model->zones.size());

	//check tasks is empty
	for (int i = 0; i < tasker.taskCount; i++)
	{
		assertEqual(tasker.tasks[i].mode , -1);
	}
}

test(FindAlarm) {

	int32_t time = tasker.timeNow() + 1;
	createZone(time);

	int pa = (int)model->findAlarm(time, 5);
	assertNotEqual(pa, 0);

	pa = (int)model->findAlarm(time, 7);
	assertNotEqual(pa, 0);

	pa = (int)model->findAlarm(time+6, 5);
	assertNotEqual(pa, 0);

	pa = (int)model->findAlarm(time, 100);
	assertNotEqual(pa, 0);

	pa = (int)model->findAlarm(time-100, 105);
	assertNotEqual(pa, 0);

	pa = (int)model->findAlarm(time - 15, 10);
	assertEqual(pa, 0);

	pa = (int)model->findAlarm(time + 16, 10);
	assertEqual(pa, 0);

	//model->printAlarms(0);
	assertTrue(model->removeZone(0));
}

test(_EditAlarm) 
{
	createZone(clockTime.timeNow() - 3600);

	int32_t time = tasker.timeNow() + 1;

	int i = 0;
	AlarmItem* alarms[3];
	for (auto elem : model->alarms) {
		if (elem.second->zoneId == 0)
			alarms[i++] = elem.second;
	}

	for (i = 0; i < 3; i++)
	{
		AlarmItem* a = alarms[i];

		uint32_t r = (time + (a->duration * i)) % TASK_TICKS_24H;
		/*Serial.printf("edit new %s old %s %d %d\n",
			Tasker::formatTime(r),
			Tasker::formatTime(a->time),
			(a->duration),
			(a->duration * i)
		);*/
		assertTrue(
			model->editAlarm(a->id, a->tapId, r, a->duration)
		);
	}

	//riego manual zone start
	//assertTrue(model->waterZone(0));

	int tSecs = 0;
	checkZone(tSecs, 15);
	assertEqual(tSecs, 15);

	//model->printAlarms(0);
	assertTrue(model->removeZone(0));
}

test(_onlyOneAlarmRunning) {

	int id = model->addZone("popo", 0);
	uint32_t time = clockTime.timeNow() + 2;

	uint8_t duration = 5;
	model->addAlarm(id, 0, time, duration);
	model->addAlarm(id, 1, time+1, duration);

	//model->printAlarms(id);

	bool error = false;
	for (int i = 0; i < 10; i++)
	{
		delay(1000);
		tasker.check();

		uint openCount = 0;
		for (auto& it : model->taps) {
			TapItem* tap = it.second;
			if (tap->open) {
				openCount++;
				//Serial.println(tap->name + String(" open"));
			}
		}

		if (openCount > 1) error = true;

		Serial.print(".");
	}
	Serial.println();

	assertFalse(error);
	assertTrue(model->removeZone(0));
}

test(_Pause) {

	createZone(clockTime.timeNow() + 1);

	delay(1000);
	tasker.check();

	assertTrue(model->isAlarmRunning);
	assertTrue(model->isWatering());

	//pausamos
	assertTrue(model->pauseWaterZone(true));

	// probamos q no se abra ningun grifo en 6sg
	if (!checkWatering(6)) {
		assertTrue(model->removeZone(0));
		assertFalse(true);//fail();
	}

	delay(1000);
	tasker.check();
	//reanudamos 
	assertTrue(model->pauseWaterZone(false));

	int tSecs = 0;
	checkZone(tSecs, 15);
	assertEqual(tSecs, 15);

	//intento riego manual zone start
	assertFalse(model->waterZone(0));

	tSecs = 0;
	checkZone(tSecs, 3);
	assertEqual(tSecs, 3);

	// volvemos a pausar
	assertTrue(model->pauseWaterZone(true));

	// probamos q no se abra ningun grifo
	if (!checkWatering(6)) {
		assertTrue(model->removeZone(0));
		assertFalse(true);
		//fail();
	}

	tSecs++;
	delay(1000);
	tasker.check();

	//reanudamos
	assertTrue(model->pauseWaterZone(false));

	checkZone(tSecs, 11);
	assertEqual(tSecs, 15);
	//borramos
	assertTrue(model->removeZone(0));
}

/**********************************************
*				setup/loop
***********************************************/

void setup() {

	Serial.begin(115200);
	delay(1000);

	//model->loadDefaultTaps();
	clockTime.begin();

	//fills taps
	uint8_t pins[] = {23,25,26,27};
	for (size_t i = 0; i < 4; i++)
	{
		TapItem * tap = model->taps.getEmpty();
		tap->set(i, pins[i], (String("tap ") + i).c_str());
		model->taps.push(tap);
	}

	TestRunner::exclude("_*");
	TestRunner::setTimeout(120);//"remove" timeout65535
}
void loop() {
	TestRunner::run();
}

