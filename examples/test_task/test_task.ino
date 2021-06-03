#include <AUnit.h>
using aunit::TestRunner;

#define private public

#include <Arduino.h>
#include"lib/Task.h"

char msg[9];
Tasker & tasker = Tasker::instance();
ClockTime & clockTime = ClockTime::instance();

void check(uint secs) {
	for (uint i = 0; i < secs; i++)
	{
		clockTime.timeNow();
		tasker.check();
		//Tasker::formatTime(msg, tasker.timeNow());
		//Serial.print(msg);

		//Serial.printf(" - %02d / %02d / %d \n",
		//	clockTime.day(), clockTime.month(), clockTime.year()
		//);

		delay(1000);
	}
}

test(FormatTime) {

	char buff[9];

	int t = Tasker::getTickTime(24, 0, 0);
	assertEqual(t, 0);
	Tasker::formatTime(buff, t);
	assertEqual(buff,"00:00:00");

	t = Tasker::getTickTime(23, 60, 0);
	assertEqual(t, 0);
	Tasker::formatTime(buff, t);
	assertEqual(buff,"00:00:00");

	t = Tasker::getTickTime(24, 6, 0);
	assertEqual(t, 360);
	Tasker::formatTime(buff, t);
	assertEqual(buff,"00:06:00");

	t = Tasker::getTickTime(23, 75, 0);
	assertEqual(t, 900);
	Tasker::formatTime(buff, t);
	assertEqual(buff,"00:15:00");

	t = Tasker::getTickTime(23, 57, 60);
	assertEqual(t, 86280);
	Tasker::formatTime(buff, t);
	assertEqual(buff,"23:58:00");
}

test(TimeLeft) 
{
	// hours, min, sec, day, mon, year
	clockTime.setTimeLocal(23, 58, 50, 4, 8, 2018);

	//howTimeLeft

	int t = Tasker::getTickTime(0, 1, 0);
	assertEqual((int)tasker.howTimeLeft(t), 130); 

	t = Tasker::getTickTime(23, 59, 30);
	assertEqual((int)tasker.howTimeLeft(t), 40); 
}

int elapsedTask = 0;

void taskCallback(Task* current) 
{
	Serial.printf("task ");
	Tasker::printTask(current);
	if (current->runing) elapsedTask = tasker.timeNow();
	else elapsedTask = tasker.timeNow() - elapsedTask;
}
test(TestTask) 
{
	// hours, min, sec, day, mon, year
	clockTime.setTimeLocal(0, 0, 0, 4, 8, 2018);

	//tasks
	Task* task;
	task = tasker.add(
		Tasker::getTickTime(0, 0, 0),// inicio
		Tasker::getTickTime(0, 0, 4),// fin
		&taskCallback // callback
	);

	check(6);
	assertEqual(elapsedTask, 4);

	for (int i = 0; i < tasker.taskCount; i++)
	{
		Tasker::printTask(&tasker.tasks[i]);
	}
	tasker.remove(task);
	assertEqual(tasker.taskCount, 0);
	Tasker::printTask(task);
	task = tasker.add(
		Tasker::getTickTime(0, 0, 1),// inicio
		Tasker::getTickTime(0, 0, 4),// fin
		&taskCallback // callback
	);
	Tasker::printTask(task);

}

void onTimeOut(Task *current)
{
	Serial.printf("onTimeOut ");
	Tasker::printTask(current);
	elapsedTask = tasker.timeNow() - elapsedTask;
}
test(setTimeout) {
	// hours, min, sec, day, mon, year
	clockTime.setTimeLocal(23, 58, 50, 4, 8, 2018);

	elapsedTask = tasker.timeNow();
	Task* task = tasker.setTimeout(&onTimeOut, 3);
	
	check(4);
	assertEqual(elapsedTask, 3);
}

void onSetInterval(Task *current)
{
	Serial.printf("setInterval ");
	Tasker::printTask(current);
	elapsedTask++;
}
test(setInterval) {
	// hours, min, sec, day, mon, year
	clockTime.setTimeLocal(23, 58, 50, 4, 8, 2018);

	elapsedTask = 0;
	Task* task = tasker.setInterval(&onSetInterval, 2);
	
	check(5);
	assertEqual(elapsedTask, 2);

	tasker.remove(task);
}

void onSetInterval1(Task *current)
{
	Serial.printf("timeHasChanged ");
	Tasker::printTask(current);
	elapsedTask++;
}
test(timeHasChanged) {
	// hours, min, sec, day, mon, year
	clockTime.setTimeLocal(23, 58, 50, 4, 8, 2018);

	Task* task = tasker.setInterval(&onSetInterval1, 2);

	// hours, min, sec, day, mon, year
	clockTime.setTimeLocal(1, 0, 0, 5, 8, 2018);
	tasker.timeHasChanged();

	elapsedTask = 0;
	check(6);
	assertEqual(elapsedTask, 3);

	tasker.remove(task);
}

void setup()
{
	Serial.begin(115200);

	delay(1000);

	clockTime.begin();

	//spain
	clockTime.setTimeZone(1);
	//irregularity, beginDay, beginMonth, endDay, endMonth, beginHour, endHour
	clockTime.setDst(0, 5, 3, 5, 10, 2, 3);

	TestRunner::exclude("_*");
	TestRunner::setTimeout(120);
}

void loop() 
{
	TestRunner::run();
}

/*
Port open
TestRunner started on 6 test(s).
Test FormatTime passed.
task --- task time: 00:00:00 duration: 4 id: 0 mode: 0 runing: 1 enabled 1
task --- task time: 00:00:00 duration: 4 id: 0 mode: 0 runing: 0 enabled 1
Test TestTask passed.
Test TimeLeft passed.
setInterval --- task time: 23:58:52 duration: 2 id: 0 mode: 2 runing: 1 enabled 1
setInterval --- task time: 23:58:54 duration: 2 id: 0 mode: 2 runing: 1 enabled 1
Test setInterval passed.
onTimeOut --- task time: 23:58:53 duration: 3 id: 0 mode: -1 runing: 1 enabled 1
Test setTimeout passed.
timeHasChanged --- task time: 01:00:00 duration: 2 id: 0 mode: 2 runing: 1 enabled 1
timeHasChanged --- task time: 01:00:02 duration: 2 id: 0 mode: 2 runing: 1 enabled 1
timeHasChanged --- task time: 01:00:04 duration: 2 id: 0 mode: 2 runing: 1 enabled 1
Test timeHasChanged passed.
TestRunner duration: 20.000 seconds.
TestRunner summary: 6 passed, 0 failed, 0 skipped, 0 timed out, out of 6 test(s).
*/
