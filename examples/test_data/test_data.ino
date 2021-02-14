#include <Streaming.h>
#include "data.h"

DataTable<10, ZoneItem> zones;
DataTable<200, AlarmItem> alarms;
DataTable<20, TapItem> taps;
DataList<24, SensorsItem> sensors;

StringTable<30> lang;

enum Str1 {
	welcome, errorLogin, reqLogin
};


void setup() {

	Serial.begin(115200);
	delay(1000);

	loadDefault();

	return;

	for (auto pair : zones)
		Serial << "zones " << pair.first << " :: " << pair.second->name << endl;

	for (auto pair : taps)
		Serial << "taps " << pair.first << " :: " << pair.second->name << endl;

	for (auto pair : alarms)
		Serial << "alarm " << pair.first << " zoneId: " << pair.second->zoneId << endl;

	Serial.println("");

}   
  

void loadDefault() {

	Serial.println("loadDefault");
	loadDefaultLang();

	Serial << lang.get(Str1::welcome)+" "+ lang.get(Str1::errorLogin) << endl;
	//lang
	for (auto pair : lang)
		Serial << "lang key " << pair.first << " value: " << pair.second->value << endl;

	Serial << lang.serializeString() << endl;

	return;

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

	ZoneItem* _zone = addZone("Cesped D", 0);
	
	addAlarm(_zone->id, 0, duration, 7, 0, 0);
	addAlarm(_zone->id, 1, duration, 7, 0, duration);
	addAlarm(_zone->id, 2, duration, 7, 0, duration * 2);
	
	
	_zone = addZone("Cesped N", 0);
	addAlarm(_zone->id, 0, duration, 21, 0, 0);
	addAlarm(_zone->id, 1, duration, 21, 0, duration);
	addAlarm(_zone->id, 3, duration, 21, 0, duration * 2);

	_zone = addZone("Huerta", 0);
	addAlarm(_zone->id, 3, duration, 7, 0, duration * 3);

}

void loadDefaultLang()
{
	lang.add(Str1::welcome, "Bienvenido ");
	lang.add(Str1::errorLogin, "usuario o pass no valido ");
	lang.add(Str1::reqLogin, "Requierre logearse");

	//lang.add(Str::, "");
}

ZoneItem* addZone(char * name, uint32_t modes) {

	ZoneItem* zone = zones.getEmpty();
	zone->set(-1, 0, 0, modes, name);

	return zones.push(zone);
}

int8_t addAlarm(uint8_t zoneId, uint8_t tapId, uint32_t time, uint16_t duration)
{
	if (zones.has(zoneId)) {
		AlarmItem* alarm = alarms.getEmpty();
		alarm->set(-1, time, duration, tapId, zoneId);
		alarms.push(alarm);
		return alarm->id;
	}
	return -1;
}

int8_t addAlarm(uint8_t zoneId, uint8_t tapId, uint16_t duration, uint8_t h, uint8_t m, uint8_t s)
{
	int32_t alarmTime = Tasker::getTickTime(h, m, s);
	return addAlarm(zoneId, tapId, alarmTime, duration);
}

void loop() {

}