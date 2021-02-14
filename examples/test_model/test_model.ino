#define private public

#include "Model.h"

Tasker & tasker = Tasker::instance();
ClockTime & clockTime = ClockTime::instance();
Model * model = Model::instance();

void testRemoveZone() {

	//errors
	int e = 0;

	// creamos zona con 3 alarmas 
	createZone(clockTime.now() - 3600);

	if (!model->removeZone(0)) e++;

	if (model->alarms.size()) e++;
	if (model->zones.size()) e++;

	for (int i = 0; i < tasker.taskCount; i++)
	{
		if (tasker.tasks[i].mode > -1) e++;
	}

	Serial.printf("testRemoveZone acabada con %d errors\n////////////////////////////////\n", e);
}

void testPause() {
	//
	int e = 0;

	// creamos zona con 3 alarmas 
	// listo para dispararse
	createZone(clockTime.now() + 1);

	delay(1000);
	tasker.check();
	//pausamos
	Serial.printf("zone %d =1 pauseWaterZone %d = 1\n", model->currentZone != nullptr, model->pauseWaterZone(true));

	// probamos q no se abra ningun grifo en 5sg
	if (checkWatering(6, false)) {
		Serial.println("////////////////////////////////\n result ok :: Pause true\n////////////////////////////////\n");
	}
	else Serial.printf(" error fatal grifo abierto errors %d\n",++e);

	delay(1000);
	tasker.check();
	//reanudamos 
	Serial.printf("zone %d = 1 pauseWaterZone %d = 1\n", model->currentZone != nullptr, model->pauseWaterZone(false));

	uint32_t tSecs = 1;
	checkTime(tSecs, 15, false);
	if (tSecs == 15) {
		Serial.println("////////////////////////////////\n  result ok :: Pause false\n////////////////////////////////\n");
	}
	else Serial.printf(" error fatal duration incorrecta errors %d\n", ++e);

	//riego manual zone start
	Serial.printf("WaterZone %d = 1\n", model->waterZone(0));

	tSecs = 0;
	checkTime(tSecs, 3, false);
	if (tSecs == 3) {
		Serial.println("////////////////////////////////\n  result ok :: WaterZone 0\n////////////////////////////////\n");
	}
	else Serial.printf(" error fatal duration incorrecta errors %d\n", ++e);

	// volvemos a pausar 6sg
	Serial.printf("zone %d =1 pauseWaterZone %d = 1\n", model->currentZone != nullptr, model->pauseWaterZone(true));

	// probamos q no se abra ningun grifo
	if (checkWatering(3, false)) {
		Serial.println("////////////////////////////////\n  result ok :: Pause true\n////////////////////////////////\n");
	}
	else Serial.printf(" error fatal grifo abierto errors %d\n", ++e);

	tSecs++;
	delay(1000);
	tasker.check();

	//reanudamos
	Serial.printf("zone %d = 1 pauseWaterZone %d = 1\n", model->currentZone != nullptr, model->pauseWaterZone(false));

	checkTime(tSecs, 11, false);

	if (tSecs == 15) {
		Serial.println("////////////////////////////////\n  result ok :: Pause false\n////////////////////////////////\n");
	}
	else Serial.printf(" error fatal duration incorrecta errors %d\n", ++e);

	Serial.printf("testPause acabada con %d errors\n////////////////////////////////\n",e);
	model->removeZone(0);
}

void testEditAlarm() {

	//
	int e = 0;

	// creamos zona con 3 alarmas 
	createZone(clockTime.now() - 3600);

	int32_t time = Tasker::getTickNow() +1;

	int i=0;
	AlarmItem * alarms[3];
	for (auto elem : model->alarms) {
		if (elem.second->zoneId == 0)
			alarms[i++] = elem.second;
	}	

	for (i = 0; i < 3; i++)
	{
		AlarmItem* a = alarms[i];

		uint32_t r = (time + (a->duration*i)) % TASK_TICKS_24H;
		Serial.printf("edit new %s old %s %d %d\n",
			Tasker::formatTime(r),
			Tasker::formatTime(a->time),
			(a->duration),
			(a->duration*i)
		);

		model->editAlarm(a->id, a->tapId, r, a->duration);
	}

	//riego manual zone start
	//Serial.printf("WaterZone %d = 1\n", model.waterZone(0));

	uint tSecs = 0;
	checkTime(tSecs, 15, false);
	if (tSecs == 15) {
		Serial.println("////////////////////////////////\n  result ok :: WaterZone 0\n////////////////////////////////\n");
	}
	else Serial.printf(" error fatal duration incorrecta errors %d\n", ++e);


	model->printAlarms(0);

	Serial.printf("testEditAlarm acabada con %d errors\n////////////////////////////////\n", e);
}

void setup() {

	Serial.begin(115200);
	delay(1000);

	model->loadDefaultTaps();
	clockTime.begin();

	uint32_t c = millis();

	//testRemoveZone();
	//testPause();//40sg
	testEditAlarm();

	Serial.printf("elapsed time result  %d", (millis() - c));
}

void printZone() {

	Serial.printf("alarm id %d time %s %s <--> %s paused %s elapsed%d\n",
		model->currentAlarm->id,
		Tasker::formatTime(clockTime.now()),
		Tasker::formatTime(model->currentAlarm->time),
		Tasker::formatTime(model->currentAlarm->time + model->currentAlarm->duration),
		model->currentZone->paused ? "true" : "false",
		model->getElapsedAlarm()
	);
}

void createZone(uint32_t time) {
	int id = model->addZone("Cesped D", 0);
	uint8_t duration = 5;
	model->addAlarm(id, 0, time, duration);
	model->addAlarm(id, 1, duration + time, duration);
	model->addAlarm(id, 2, duration * 2 + time, duration);

	model->printAlarms(id);
}


void checkTime(uint &tsecs, int secs, bool print) {
	for (int i = 0; i < secs; i++)
	{
		delay(1000);
		tasker.check();
		if (model->currentZone) {

			if (print) printZone();

			tsecs++;
		}
		Serial.printf(" --------- %d %d---------\n", i, tsecs);
	}
}

bool checkWatering(int secs, bool print) {
	for (int i = 0; i < secs; i++)
	{
		delay(1000);
		tasker.check();
		if (model->currentZone) {

			if (print) printZone();

			if (model->isWatering()) return false;
		}
		Serial.printf(" --------- %d ---------\n", i);
	}
}

void loop() {
	//model.update();
}

