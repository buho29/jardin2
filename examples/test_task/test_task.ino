#include <Arduino.h>
#include"lib/Task.h"

char msg[9];
Tasker & tasker = Tasker::instance();
ClockTime & clockTime = ClockTime::instance();

void setup()
{
	Serial.begin(115200);

	delay(1000);

	clockTime.begin();

	//spain
	clockTime.setTimeZone(1);
	//irregularity, beginDay, beginMonth, endDay, endMonth, beginHour, endHour
	clockTime.setDst(0, 5, 3, 5, 10, 2, 3);



	char buff[9];

	//uint32_t t = Tasker::getTickTime(24, 0, 0);
	//Tasker::formatTime(buff, t);
	//Serial.printf("24:0:0 %s t %d\n",buff , t);
	////24:0:0  0:00:00 t 0

	//t = Tasker::getTickTime(23, 60, 0);
	//Tasker::formatTime(buff, t);
	//Serial.printf("23:60:0 %s t %d\n", buff, t);
	////23:60:0 0:00:00 t 0

	//t = Tasker::getTickTime(24, 6, 0);
	//Tasker::formatTime(buff, t);
	//Serial.printf("24:6:0 %s t %d\n", buff, t);
	////24:6:0 0:06:00 t 360

	//t = Tasker::getTickTime(23, 75, 0);
	//Tasker::formatTime(buff, t);
	//Serial.printf("23:75:0 %s t %d\n", buff, t);
	////23:75:0 0:15:00 t 900

	//t = Tasker::getTickTime(23, 57, 60);
	//Tasker::formatTime(buff, t);
	//Serial.printf("23:57:60 %s t %d\n\n", buff, t);
	////23:57:60 23:58:00 t 86280
	//
	// hours, min, sec, day, mon, year
	clockTime.setTimeLocal(23, 58, 50, 4, 8, 2018);

	Tasker::formatTime(buff, Tasker::getTickNow());
	Serial.printf("now %s\n\n", buff);

	////howTimeLeft

	//t = Tasker::getTickTime(0, 1, 0);
	//Tasker::formatTime(buff, t);
	//Serial.printf("0:1:00 %s falta %dsg\n", buff, Tasker::howTimeLeft(t));


	//t = Tasker::getTickTime(23, 59, 30);
	//Tasker::formatTime(buff, t);
	//Serial.printf("23:59:30 %s falta %dsg\n\n", buff, Tasker::howTimeLeft(t));

	//tasks
	Task * task;

	//task = tasker.add(
	//	Tasker::getTickTime(23, 59, 0),// inicio
	//	Tasker::getTickTime(23, 59, 30),// fin
	//	&taskCallback // callback
	//);
	//Serial.printf("id = %d 23:59:00 -> 23:59:30\n",task->id);
	//
	//
	//task = tasker.add(Tasker::getTickTime(23, 59, 30), Tasker::getTickTime(0, 1, 0), &taskCallback);
	//Serial.printf("id = %d 23:59:30 -> 0:01:30\n",task->id);

	//task = tasker.add(Tasker::getTickTime(23, 59, 40), Tasker::getTickTime(23, 60, 30), &taskCallback);
	//Serial.printf("id = %d 23:59:40 -> 23:60:30\n",task->id);

	task = tasker.setTimeout(&onTimeOut, 3);
	Serial.printf("id = %d timeOut 23:58:53\n", task->id);

	task = tasker.setTimeout(&onTimeOut1, 15);
	Serial.printf("id = %d timeOut 23:58:55\n", task->id);

	task = tasker.setInterval(&onSetInterval, 2);
	Serial.printf("id = %d interval 23:58:55\n", task->id);

	//tasker.remove(task);

	/*

	now 23:58:50
	id = 0 23:59:00 -> 23:59:30
	id = 1 23:59:30 -> 0:01:30
	id = 2 23:59:40 -> 23:60:30
	id = 3 timeOut 23:58:53
	id = 4 timeOut 23:58:55
	onTimeOut 23:58:53 now: 23:58:53 3s
	onTimeOut1 23:58:55 now: 23:58:55 5s
	task id:0 Start 23:59:00 -> 23:59:30 now: 23:59:00
	task id:3 Start 23:59:00 -> 23:59:30 now: 23:59:00
	task id:0 Stoped 23:59:00 -> 23:59:30 now: 23:59:30
	task id:3 Stoped 23:59:00 -> 23:59:30 now: 23:59:30
	task id:1 Start 23:59:30 -> 00:01:00 now: 23:59:31
	setInterval 23:59:40 now: 23:59:40 50s
	task id:2 Start 23:59:40 -> 00:00:30 now: 23:59:41
	setInterval 00:00:30 now: 00:00:30 50s
	task id:2 Stoped 23:59:40 -> 00:00:30 now: 00:00:31
	task id:1 Stoped 23:59:30 -> 00:01:00 now: 00:01:01
	setInterval 00:01:20 now: 00:01:20 50s
	setInterval 00:02:10 now: 00:02:10 50s
	*/
}

void taskCallback(Task *current) {
	Tasker::printTask(current);
}


void onTimeOut(Task *current)
{
	Serial.printf("onTimeOut %s ", msg);
	Tasker::printTask(current);

	//tasker.add(Tasker::getTickTime(23, 59, 0), Tasker::getTickTime(23, 59, 30), &taskCallback);
}


void onTimeOut1(Task *current)
{
	Serial.printf("onTimeOut1 %s ", msg);
	Tasker::printTask(current);
}

void onSetInterval(Task *current)
{
	Serial.printf("setInterval %s", msg);
	Tasker::printTask(current);
}

void loop() {

	Tasker::formatTime(msg, Tasker::getTickNow());
	tasker.check();
	Serial.print(msg);

	Serial.printf(" - %02d / %02d / %d \n",
		clockTime.day(), clockTime.month(), clockTime.year()
	);

	delay(1000);

}
