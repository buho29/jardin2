#define private public

#include "Model.h"

Tasker & tasker = Tasker::instance();
ClockTime & clockTime = ClockTime::instance();
Model * model = Model::instance();

void testRemoveZone() {

	//errors
	int e = 0;

	// creamos zona con 3 alarmas 
	createZone(clockTime.timeNow() - 3600);

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
	createZone(clockTime.timeNow() + 1);

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
	createZone(clockTime.timeNow() - 3600);

	int32_t time = tasker.timeNow() +1;

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

void onTimeOut(Task* current)
{
	Tasker::printTask(current);

	//tasker.add(Tasker::getTickTime(23, 59, 0), Tasker::getTickTime(23, 59, 30), &taskCallback);
}

void testLoadTime() {
	/*
	02:11:40:146 -> Temperatura min 2° max 15° sol se levanta a 07:24:00 se acuesta a 19:48:00
proxima actualizacion sol a 19:48:00
faltan 23328 06:28:48 sun1 1616788080 sun 1616743440 now 13:19:12
--- task time: 19:48:00 duration: 23328 id: 15 mode: 1 runing: 0 enabled 1
*/

	uint32_t s;

	//uint32_t t = 1616766950;//13:55:50
	clockTime.setTimeNow(1616788550);//19:55:50
	uint32_t t = clockTime.timeNow() % TASK_TICKS_24H;
	uint32_t sun1 = 1616743440 % TASK_TICKS_24H;//07:24:00 
	uint32_t sun2 = 1616788080 % TASK_TICKS_24H;//19:48:00
	bool isday = t > sun1 && t < sun2;

	if (isday) s = sun2 - t;
	else s = TASK_TICKS_24H - t + sun1;

	char buff[9], buff1[9];
	Tasker::formatTime(buff, sun1);
	Tasker::formatTime(buff1, sun2);

	Serial.printf("sol se levanta a %s se acuesta a %s\n", buff, buff1);
	Serial.printf("proxima actualizacion sol a %s\n", isday ? buff1 : buff);
	Serial.printf("faltan %d %s sun1 %d sun %d now %s %d\n", s, Tasker::formatTime(s).c_str(),
		sun2, sun1, Tasker::formatTime(t).c_str(),t);

	using namespace std::placeholders;
	Task* ta = tasker.setTimeout(onTimeOut, s);
	Tasker::printTask(ta);
	Serial.println("teta");
}

void setup() {

	Serial.begin(115200);
	delay(1000);

	//model->loadDefaultTaps();
	clockTime.begin();

	uint32_t c = millis();

	//testRemoveZone();
	//testPause();//40sg
	//testEditAlarm();
	testLoadTime();

	Serial.printf("elapsed time result  %d", (millis() - c));
}

void printZone() {

	Serial.printf("alarm id %d time %s %s <--> %s paused %s elapsed%d\n",
		model->currentAlarm->id,
		Tasker::formatTime(clockTime.timeNow()),
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

