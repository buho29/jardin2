#include <AUnit.h>
using aunit::TestRunner;

#include "data.h"

DataTable<10, ZoneItem> zones;
DataTable<200, AlarmItem> alarms;
DataTable<20, TapItem> taps;
DataList<24, SensorItem> sensors;

StringTable<30> lang;

enum Str1 {
	welcome, errorLogin, reqLogin
};

test(testLang) 
{
	Serial.println("testLang");

	lang.add(Str1::welcome, "Bienvenido ");
	lang.add(Str1::errorLogin, "usuario o pass no valido ");
	lang.add(Str1::reqLogin, "Requierre logearse");

	Serial.printf("%s : %s\n",
		lang.get(Str1::welcome).c_str(),
		lang.get(Str1::errorLogin).c_str() 
	);
	//lang
	for (auto pair : lang)
		Serial.printf("lang key :%d :: %s\n", pair.first, pair.second->value);

	const char* json = "[{\"id\":0,\"value\":\"Bienvenido \"},{\"id\":1,\"value\":\"usuario o pass no valido \"},{\"id\":2,\"value\":\"Requierre logearse\"}]";
	assertEqual(json, lang.serializeString().c_str());

	//Serial.println(lang.serializeString());
}

ZoneItem* addZone(char * name) 
{
	ZoneItem* zone = zones.getEmpty();
	zone->set(Item::CREATE_NEW, 0, 0, name);

	return zones.push(zone);
}
AlarmItem* addAlarm(uint8_t zoneId, uint8_t tapId, uint32_t time, uint16_t duration)
{
	if (zones.has(zoneId)) {
		AlarmItem* alarm = alarms.getEmpty();
		alarm->set(-1, time, duration, tapId, zoneId);
		alarms.push(alarm);
		return alarm;
	}
	return nullptr;
}
AlarmItem* addAlarm(uint8_t zoneId, uint8_t tapId, uint16_t duration, uint8_t h, uint8_t m, uint8_t s)
{
	int32_t alarmTime = Tasker::getTickTime(h, m, s);
	return addAlarm(zoneId, tapId, alarmTime, duration);
}
test(testAppData) 
{
	Serial.println("testAppData");

	uint8_t pins[4] = { 27,14,21,17 };
	char buffer[20] = "Grifo 1000";

	for (size_t i = 0; i < 4; i++)
	{
		sprintf(buffer, "Grifo %d", i);
		TapItem* tap = taps.getEmpty();
		tap->set(-1, pins[i], buffer);
		taps.push(tap);
	}

	uint8_t duration = 60; 

	ZoneItem* _zone = addZone("Cesped D");
	assertTrue(_zone);
	assertTrue(addAlarm(_zone->id, 0, duration, 7, 0, 0));
	assertTrue(addAlarm(_zone->id, 1, duration, 7, 0, duration));
	assertTrue(addAlarm(_zone->id, 2, duration, 7, 0, duration * 2));
	
	_zone = addZone("Cesped N");
	assertTrue(_zone);
	assertTrue(addAlarm(_zone->id, 0, duration, 21, 0, 0));
	assertTrue(addAlarm(_zone->id, 1, duration, 21, 0, duration));
	assertTrue(addAlarm(_zone->id, 3, duration, 21, 0, duration * 2));

	_zone = addZone("Huerta");
	assertTrue(_zone);
	assertTrue(addAlarm(_zone->id, 3, duration, 7, 0, duration * 3));


	for (auto pair : zones)
		Serial.printf("zones %d :: %s\n", pair.first, pair.second->name);

	for (auto pair : taps)
		Serial.printf("taps %d :: %s\n", pair.first, pair.second->name);

	for (auto pair : alarms)
		Serial.printf("alarms %d :: %d\n", pair.first, pair.second->time);

	Serial.println("");

	assertEqual((int)zones.size(), 3);
	assertEqual((int)taps.size(), 4);
	assertEqual((int)alarms.size(), 7);

	assertTrue(zones.remove(_zone->id));
	assertEqual((int)zones.size(), 2);
}


Config config;
test(testDstConfig) {
	assertTrue(config.setDst("0,5,2,5,10,2,3"));
	assertEqual(config.getDst().c_str(), "0,5,02,5,10,2,3");
	assertFalse(config.setDst("s,69,re,rzes,fdsf,qsdqs,qsdqd"));
	assertFalse(config.setDst("te,ta"));
}
test(testAdmin) {
	assertTrue(config.setAdmin("teta","pass"));
	assertFalse(config.setAdmin("this have more 32 characters long","pass"));
}
test(testWifi) {
	assertTrue(config.setWifi("teta", "pass"));
	assertFalse(config.setWifi("this have more 32 characters long", "pass"));
}

void setup() 
{
	delay(1000);
	Serial.begin(115200);
	TestRunner::exclude("_*");
}   
void loop() {
	TestRunner::run();
}

/*
Opening port
Port open
TestRunner started on 5 test(s).
Test testAdmin passed.
testAppData
zones 0 :: Cesped D
zones 1 :: Cesped N
zones 2 :: Huerta
taps 0 :: Grifo 0
taps 1 :: Grifo 1
taps 2 :: Grifo 2
taps 3 :: Grifo 3
alarms 0 :: 25200
alarms 1 :: 25260
alarms 2 :: 25320
alarms 3 :: 75600
alarms 4 :: 75660
alarms 5 :: 75720
alarms 6 :: 25380

Test testAppData passed.
Test testDstConfig passed.
testLang
Bienvenido  : usuario o pass no valido
lang key :0 :: Bienvenido
lang key :1 :: usuario o pass no valido
lang key :2 :: Requierre logearse
Test testLang passed.
Test testWifi passed.
TestRunner duration: 0.040 seconds.
TestRunner summary: 5 passed, 0 failed, 0 skipped, 0 timed out, out of 5 test(s).

*/