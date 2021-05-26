#include <AUnit.h>
using aunit::TestRunner;

#define private public

#include "Model.h"

Tasker & tasker = Tasker::instance();
ClockTime & clockTime = ClockTime::instance();
Model * model = Model::instance();

//zone/alarms variables
const uint8_t duration = 5;
const uint8_t alarmsCount = 3;
const int totalDuration = duration * alarmsCount;

/**********************************************
*				helper/tools 
***********************************************/

// creamos zona con 3 alarmas 
int createZone(uint32_t time, const char* name = "zone")
{
	int id = model->addZone(name, 0, "01/01-12/31");

	for (int i = 0; i < alarmsCount; i++)
	{
		model->addAlarm(id, i,(duration * i) + time, duration);
	}

	return id;
}

int checkWatering(int secs) {
	int water = 0;
	for (int i = 0; i < secs; i++)
	{
		delay(1000);
		tasker.check();
		if (model->currentZone && model->isWatering())
		{
			water ++;
		}
		Serial.print(".");
	}
	Serial.printf(" %ds open \n",water);
	return water;
}

/**********************************************
*					tests
***********************************************/

test(_Zone) 
{
	Serial.println("Test Zone start");
	uint time = clockTime.local();

	int id = model->addZone("zona", 0, "01/01-12/31");

	//si es -1 es error 
	assertMore(id, -1);

	for (int i = 0; i < alarmsCount; i++)
	{
		int idAlarm = model->addAlarm(id, i, (duration * i) + time, duration);
		assertMore(idAlarm, -1);
	}
	
	delay(1000);
	tasker.check();

	assertTrue(model->isAlarmRunning);
	assertTrue(model->isWatering());
	//comprobamos que el grifo esta abierto 14sg
	Serial.print("\tcheck ");
	assertEqual(checkWatering(totalDuration), totalDuration-1);

	//borramos
	assertTrue(model->removeZone(0));
	
	assertEqual((int)model->alarms.size(),0);
	assertEqual((int)model->zones.size(),0);

	//check tasks is empty (-1)
	for (int i = 0; i < tasker.taskCount; i++)
	{
		assertEqual(tasker.tasks[i].mode , -1);
	}
}

test(Pause) 
{
	Serial.println("Test Pause start");

	createZone(clockTime.local());
	createZone(clockTime.local()-3600,"zone 1");

	tasker.check();

	assertTrue(model->isAlarmRunning);
	assertTrue(model->isWatering());

	Serial.print("\tpause(true) ");
	assertTrue(model->pauseWaterZone(true));
	// comprobamos q no se abre ningun grifo en 8sg
	assertEqual(checkWatering(8), 0);
	Serial.print("\t\t");
	//comprobamos q no se inicie otra
	assertFalse(model->waterZone(1));
	assertEqual(checkWatering(8), 0);

	Serial.print("\tpause(false) ");
	assertTrue(model->pauseWaterZone(false));
	//comprobamos q no se inicie otra
	assertFalse(model->waterZone(1));
	// comprobamos q se abre el grifo 15sg
	assertEqual(checkWatering(totalDuration+2), totalDuration-1);

	//riego manual zone
	Serial.print("\tmanual water zone ");
	assertTrue(model->waterZone(0));
	assertEqual(checkWatering(3), 3);

	Serial.print("\tpause(true) ");
	assertTrue(model->pauseWaterZone(true));
	assertEqual(checkWatering(6), 0);

	Serial.print("\tpause(false) ");
	assertTrue(model->pauseWaterZone(false));
	assertEqual(checkWatering(totalDuration-2), totalDuration-4);
	//borramos
	assertTrue(model->removeZone(0));
	assertTrue(model->removeZone(1));
}


test(_FindAlarm)
{
	int32_t time = tasker.timeNow() + 1;
	createZone(time);

	int pa = (int)model->findAlarm(time, 5);
	assertNotEqual(pa, 0);

	pa = (int)model->findAlarm(time, 7);
	assertNotEqual(pa, 0);

	pa = (int)model->findAlarm(time + 6, 5);
	assertNotEqual(pa, 0);

	pa = (int)model->findAlarm(time, 100);
	assertNotEqual(pa, 0);

	pa = (int)model->findAlarm(time - 100, 105);
	assertNotEqual(pa, 0);

	pa = (int)model->findAlarm(time - 15, 10);
	assertEqual(pa, 0);

	pa = (int)model->findAlarm(time + 16, 10);
	assertEqual(pa, 0);

	assertTrue(model->removeZone(0));
}

test(_EditAlarm)
{
	Serial.println("Test EditAlarm start");

	createZone(clockTime.local() - 3600);

	int32_t time = tasker.timeNow();

	int i = 0;
	AlarmItem* alarms[3];
	for (auto elem : model->alarms) {
		if (elem.second->zoneId == 0)
			alarms[i++] = elem.second;
	}

	for (i = 0; i < 3; i++)
	{
		AlarmItem* a = alarms[i];

		uint32_t start = (time + (a->duration * i)) % TASK_TICKS_24H;
		/*Serial.printf("edit new %s old %s %d %d\n",
			Tasker::formatTime(start),
			Tasker::formatTime(a->time),
			(a->duration),
			(a->duration * i)
		);*/
		assertTrue(
			model->editAlarm(a->id, a->tapId, start, a->duration)
		);
	}

	Serial.print("\tcheck ");
	assertEqual(checkWatering(totalDuration+1), totalDuration);

	assertTrue(model->removeZone(0));
}

test(_onlyOneAlarmRunning) 
{
	Serial.println("Test onlyOneAlarmRunning start");

	int id = model->addZone("huerta", 0, "01/01-12/31");
	uint32_t time = clockTime.local() + 2;

	uint8_t duration = 5;
	model->addAlarm(id, 0, time, duration);
	model->addAlarm(id, 1, time + 1, duration);

	Serial.print("\tcheck ");
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
/**********************************************
*				setup/loop
***********************************************/

void setup() 
{
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

	model->modes.begin();
	model->writeFileEnabled = false;

	TestRunner::exclude("_*");
	TestRunner::setTimeout(120);

	clockTime.setTimeNow(0, 0, 0, 1, 1, 2021);
}

void loop() 
{
	TestRunner::run();
}

/*
Opening port
Port open
TestRunner started on 5 test(s).
Test EditAlarm start
	check ................ 15s open
Test EditAlarm passed.
Test FindAlarm passed.
Test Pause start
	pause(true) ...... 0s open
	pause(false) ................ 15s open
	manual water zone ... 3s open
	pause(true) ...... 0s open
	pause(false) ............. 12s open
Test Pause passed.
Test Zone start
	check ............... 14s open
Test Zone passed.
Test onlyOneAlarmRunning start
	check ..........
Test onlyOneAlarmRunning passed.
TestRunner duration: 86.208 seconds.
TestRunner summary: 5 passed, 0 failed, 0 skipped, 0 timed out, out of 5 test(s).
*/