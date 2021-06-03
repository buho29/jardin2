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
uint createZone(uint32_t time, const char* name = "zone")
{
	ZoneItem* z = model->addZone(name, 0, "");

	for (int i = 0; i < alarmsCount; i++)
	{
		model->addAlarm(z->id, i,(duration * i) + time, duration);
	}

	return z->id;
}

int checkWatering(int secs) {
	int water = 0;
	for (int i = 0; i < secs; i++)
	{
		delay(1000);
		clockTime.timeNow();
		tasker.check();
		if (model->currentZone && model->isWatering())
		{
			AlarmItem* a = model->alarms[model->currentZone->alarmId];
			//Tasker::printTask(a->task);
			water ++;
		}
		//Serial.print(".");
		//Serial.printf("%s \n", Tasker::formatTime(clockTime.local()));
		//Serial.printf("%d %d\n",model->currentZone , model->isWatering());
	}
	Serial.printf(" %ds open \n",water);
	return water;
}

/**********************************************
*					tests
***********************************************/

test(Zone) 
{
	Serial.println("Test Zone start");

	uint time = clockTime.timeNow();

	ZoneItem* z = model->addZone("zona", 0, "");
	//si es 0 es error 
	assertTrue(z);

	for (int i = 0; i < alarmsCount; i++)
	{
		AlarmItem* alarm = model->addAlarm(z->id, i, (duration * i) + time, duration);
		assertTrue(alarm);
	}

	clockTime.timeNow();
	tasker.check();

	assertTrue(model->isAlarmRunning);
	assertTrue(model->isWatering());
	//comprobamos que el grifo esta abierto 14sg
	//Serial.print("\tcheck ");
	assertEqual(checkWatering(totalDuration+1), totalDuration-1);

	//borramos
	assertTrue(model->removeZone(0));

	assertEqual((int)model->alarms.size(), 0);
	assertEqual((int)model->modesItems.size(), 0);
	assertEqual((int)model->zones.size(),0);

	delay(1000);
}

test(Pause) 
{
	Serial.println("Test Pause start");

	clockTime.timeNow();
	createZone(clockTime.local());
	createZone(clockTime.local()-3600,"zone 1");

	//model->printZones();
	tasker.check();

	assertTrue(model->isAlarmRunning);
	assertTrue(model->isWatering());
	assertTrue(model->currentZone);

	Serial.print("pause(true) ");
	assertTrue(model->pauseWaterZone(true));
	// comprobamos q no se abre ningun grifo en +5sg
	assertTrue(model->currentZone);
	assertEqual(checkWatering(8), 0);

	assertFalse(model->isWatering());
	assertTrue(model->isPaused());

	//comprobamos q no se inicie otra
	assertFalse(model->waterZone(1));
	assertEqual(checkWatering(8), 0);

	Serial.print("pause(false) ");
	assertTrue(model->pauseWaterZone(false));
	//comprobamos q no se inicie otra
	assertFalse(model->waterZone(1));
	// comprobamos q se abre el grifo 15sg
	assertEqual(checkWatering(totalDuration+2), totalDuration-1);

	//riego manual zone
	Serial.print("manual water zone ");
	assertTrue(model->waterZone(0));
	assertEqual(checkWatering(3), 3);

	Serial.print("pause(true) ");
	assertTrue(model->pauseWaterZone(true));
	assertEqual(checkWatering(6), 0);

	Serial.print("pause(false) ");
	assertTrue(model->pauseWaterZone(false));
	assertEqual(checkWatering(totalDuration-2), totalDuration-4);
	//borramos
	assertTrue(model->removeZone(0));
	assertTrue(model->removeZone(1));

	assertEqual((int)model->alarms.size(), 0);
	assertEqual((int)model->modesItems.size(), 0);
	assertEqual((int)model->zones.size(), 0);

	delay(1000);
}

test(FindAlarm)
{
	clockTime.timeNow();
	int32_t time = tasker.timeNow() + 1;
	createZone(time);

	assertNotEqual((int)model->findAlarm(time, 5), 0);
	assertNotEqual((int)model->findAlarm(time, 7), 0);
	assertNotEqual((int)model->findAlarm(time + 6, 5), 0);
	assertNotEqual((int)model->findAlarm(time, 100), 0);
	assertNotEqual((int)model->findAlarm(time - 100, 105), 0);
	assertEqual((int)model->findAlarm(time - 15, 10), 0);
	assertEqual((int)model->findAlarm(time + 16, 10), 0);

	assertTrue(model->removeZone(0));
}

test(EditAlarm)
{
	Serial.println("Test EditAlarm start");

	//update
	clockTime.timeNow();
	createZone(clockTime.local() - 3600);

	int32_t time = tasker.timeNow();

	int i = 0;
	AlarmItem* alarms[3];
	for (auto elem : model->alarms) {
		if (elem.second->zoneId == 0)
			alarms[i++] = elem.second;
	}
	Serial.println("init edit");
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
		//Tasker::printTask(a->task);
		assertTrue(
			model->editAlarm(a->id, a->tapId, start, a->duration)
		);
		//Tasker::printTask(a->task);
	}
	Serial.println("end edit");

	Serial.print("check \n");
	assertEqual(checkWatering(totalDuration+1), totalDuration-1);

	assertTrue(model->removeZone(0));
	delay(1000);
}

test(onlyOneAlarmRunning) 
{
	Serial.println("Test onlyOneAlarmRunning start");

	ZoneItem* z = model->addZone("huerta", 0, "01/01-12/31");
	uint32_t time = clockTime.local() + 2;

	uint8_t duration = 5;
	model->addAlarm(z->id, 0, time, duration);
	model->addAlarm(z->id, 1, time + 1, duration);

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

	//spain
	clockTime.setTimeZone(1);
	//irregularity, beginDay, beginMonth, endDay, endMonth, beginHour, endHour
	clockTime.setDst(0, 5, 3, 5, 10, 2, 3);
	clockTime.setTimeLocal(10, 10, 0, 1, 1, 2021);
	//clockTime.setTimeNow(0, 0, 0, 1, 1, 2021);
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
init edit
end edit
check
 14s open
Test EditAlarm passed.
Test FindAlarm passed.
Test Pause start
pause(true)  0s open
 0s open
pause(false)  14s open
manual water zone  3s open
pause(true)  0s open
pause(false)  11s open
Test Pause passed.
Test Zone start
 14s open
Test Zone passed.
Test onlyOneAlarmRunning start
	check ..........
Test onlyOneAlarmRunning passed.
TestRunner duration: 100.265 seconds.
TestRunner summary: 5 passed, 0 failed, 0 skipped, 0 timed out, out of 5 test(s).s
*/